// Copyright (c) 2015 Kerem KAT 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files(the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Do not hesisate to contact me about usage of the code or to make comments 
// about the code. Your feedback will be appreciated.
//
// http://dissipatedheat.com/
// http://github.com/krk/

/**
\file WaitApiStackParser.cpp

Implements WaitApiStackParser for stack trace outputs calling kernel wait APIs.
*/

#include "WaitApiStackParser.h"

std::map<std::string, std::pair<unsigned long, unsigned long>> WaitApiStackParser::_symbol_object = {
	/* symbol_name, count_arg_number, address_arg_number */
	{ "ntdll!NtWaitForSingleObject", std::make_pair(1, KernelObjectDescriptor::ADDRESS_IS_IMMEDIATE) },
	{ "ntdll!NtWaitForMultipleObjects", std::make_pair(2, 1) },
	{ "ntdll!NtSignalAndWaitForSingleObject", std::make_pair(2, KernelObjectDescriptor::ADDRESS_IS_IMMEDIATE) },

	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms687069(v=vs.85).aspx
};

/**
Parses a line of stack trace output from WinDbg.

\param line WinDbg kv command output.
*/
PartialStackFrame WaitApiStackParser::ParseStackFrame(const std::string& line)
{
	// min length for a valid line is 49.
	// 00 12f2de1c 757e112f 00000001 12f2e1c0 00000000 n

	if (line.length() < 49)
	{
		auto invalid = PartialStackFrame();
		invalid._is_valid = false;

		return invalid;
	}

	if (!(line[2] == ' ' && line[11] == ' ' && line[20] == ' ' && line[29] == ' ' && line[38] == ' ' && line[47] == ' '))
	{
		auto invalid = PartialStackFrame();
		invalid._is_valid = false;

		return invalid;
	}

	auto ret = PartialStackFrame();

	auto child_ebp = line.substr(3, 8);
	auto ret_address = line.substr(12, 8);
	auto arg1 = line.substr(21, 8);
	auto arg2 = line.substr(30, 8);
	auto arg3 = line.substr(39, 8);
	auto rest = line.substr(48);

	auto first_space_index = rest.find(' ');
	auto symbol_name = first_space_index == std::string::npos ? rest : rest.substr(0, first_space_index);

	auto first_plus_index = symbol_name.find('+');
	auto symbol_offset_text = first_plus_index == std::string::npos ? "0" : symbol_name.substr(first_plus_index + 3);

	if (first_plus_index != std::string::npos)
	{
		symbol_name = symbol_name.substr(0, first_plus_index);
	}

	ret.child_ebp = std::stoul(child_ebp, nullptr, 16);
	ret.ret_address = std::stoul(ret_address, nullptr, 16);
	ret.arg1 = std::stoul(arg1, nullptr, 16);
	ret.arg2 = std::stoul(arg2, nullptr, 16);
	ret.arg3 = std::stoul(arg3, nullptr, 16);
	ret.symbol_name = symbol_name;
	ret.symbol_offset = std::stoul(symbol_offset_text, nullptr, 16);

	return ret;
}

/**
Parses lines of stack trace output from WinDbg.

\param stackFrame Stack frame that has the symbol name and first three argument values.
*/
KernelObjectDescriptor WaitApiStackParser::ParseObjectDescriptor(const PartialStackFrame& stackFrame)
{
	if (_symbol_object.find(stackFrame.symbol_name) == _symbol_object.end())
	{
		// symbol not mapped.
		return KernelObjectDescriptor(KernelObjectDescriptor::VALUE_NOT_FOUND, 0);
	}

	auto symbol_object = _symbol_object[stackFrame.symbol_name];

	auto address_argument = symbol_object.first;
	auto object_count = symbol_object.second;

	unsigned long value;

	switch (address_argument)
	{
	case 1:
		value = stackFrame.arg1;
		break;
	case 2:
		value = stackFrame.arg2;
		break;
	case 3:
		value = stackFrame.arg3;
		break;
	default:
		value = KernelObjectDescriptor::VALUE_NOT_FOUND;
		break;
	}

	return KernelObjectDescriptor(value, object_count);
}

/**
Parses lines of stack trace output from WinDbg.

\param lines WinDbg kv command output for multiple threads.
*/
std::vector<const KernelObjectDescriptor>* WaitApiStackParser::Parse(const std::string& lines)
{
	auto ret = new std::vector<const KernelObjectDescriptor>();

	std::istringstream iss(lines);

	std::string line;

	int currentLine = 0;

	while (std::getline(iss, line))
	{
		if (line.find("Evaluate expression: ") != std::string::npos)
		{
			/* New thread stack trace. */
			auto threadIdText = line.substr(line.length() - 8);
			auto threadId = std::stoul(threadIdText, nullptr, 16);

			/* Next line must be the comment line. */
			std::getline(iss, line);

			if (line != " # ChildEBP RetAddr  Args to Child              ")
			{
				// go to next thread.
				continue;
			}

			/* Read frame */
			std::getline(iss, line);

			while (line.find("WARNING: ") != std::string::npos)
			{
				/* Skip warnings. */
				std::getline(iss, line);
			}

			auto stackFrame = ParseStackFrame(line);

			if (!stackFrame.isValid())
			{
				continue;
			}

			auto objectDescriptor = ParseObjectDescriptor(stackFrame);

			if (objectDescriptor.get_value() == KernelObjectDescriptor::VALUE_NOT_FOUND)
			{
				continue;
			}

			objectDescriptor.set_thread_id(threadId);

			ret->push_back(objectDescriptor);
		}
	}

	return ret;

	/*
	Evaluate expression: 16288 = 00003fa0
	# ChildEBP RetAddr  Args to Child
	00 0063b2a4 0f0a565e fd14c10d 73dcc294 0063b3bc user32!NtUserWaitMessage+0xc (FPO: [0,0,0])
	Evaluate expression: 18384 = 000047d0
	# ChildEBP RetAddr  Args to Child
	WARNING:
	00 0222f6e4 767b112f 00000003 0222f8b8 00000001 ntdll!NtWaitForMultipleObjects+0xc (FPO: [5,0,0])
	Evaluate expression: 16684 = 0000412c
	# ChildEBP RetAddr  Args to Child
	00 0233fc3c 767a2cc7 00000144 00000000 00000000 ntdll!NtWaitForSingleObject+0xc (FPO: [3,0,0])
	WARNING:
	*/
}

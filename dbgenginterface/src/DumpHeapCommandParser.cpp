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
\file EEHeapParser.cpp

Implements EEHeapParser class that parses native !eeheap -gc output.
*/

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "DumpHeapCommandParser.h"

/**
Executes dumpheap command and parses the output.

\param handle Value of the handle.
*/
DumpHeapCommandOutput DumpHeapCommandParser::execute_by_mt(unsigned long method_table)
{
	std::string output;
	
	std::stringstream sstream;
	sstream << std::hex << method_table;
	auto mt_hex = sstream.str();

	auto command = _command_mt + " " + mt_hex;

	if (!_executor->ExecuteCommand(command, output))
	{
		_logger->Log("Cannot get dumpheap info.\n");

		return DumpHeapCommandOutput();
	}

	auto ranges = Parse(output);

	return DumpHeapCommandOutput(ranges);
}

/**
Executes dumpheap command and parses the output.

\param handle Value of the handle.
*/
DumpHeapCommandOutput DumpHeapCommandParser::execute(const std::string& clr_partial_type_name)
{
	std::string output;

	auto command = _command + " " + clr_partial_type_name;

	if (!_executor->ExecuteCommand(command, output))
	{
		_logger->Log("Cannot get dumpheap info.\n");

		return DumpHeapCommandOutput();
	}

	auto ranges = Parse(output);

	return DumpHeapCommandOutput(ranges);
}

/**
Parses lines of an dumpheap output to find the address information.

\param lines DumpHeap output lines.
*/
std::vector<unsigned long>* DumpHeapCommandParser::Parse(const std::string& lines)
{
	auto ret = new std::vector<unsigned long>();

	std::istringstream iss(lines);

	std::string line;

	while (std::getline(iss, line))
	{
		if (line.size() < 8)
		{
			continue;
		}

		auto addressText = line.substr(0, 8);

		try{
			auto address = std::stoul(addressText, nullptr, 16);

			ret->push_back(address);
		}
		catch (std::invalid_argument)
		{
			_logger->Log("Address cannot be read: %s\n", line);
		}
	}

	return ret;
}

MethodTableOutput DumpHeapCommandParser::find_method_tables(const std::string& clr_exact_type_name)
{
	std::string output;

	auto command = _command_stat + " " + clr_exact_type_name;

	if (!_executor->ExecuteCommand(command, output))
	{
		_logger->Log("Cannot get method table info.\n");

		return MethodTableOutput();
	}

	auto tables = ParseTables(clr_exact_type_name, output);

	return MethodTableOutput(tables);
}

/**
Parses lines of an dumpheap output to find the method table information.

\param lines DumpHeap -stat output lines.
*/
std::vector<unsigned long>* DumpHeapCommandParser::ParseTables(const std::string& clr_exact_type_name, const std::string& lines)
{
	auto ret = new std::vector<unsigned long>();

	std::istringstream iss(lines);

	std::string line;

	auto min_line_length = 31 + clr_exact_type_name.size();

	while (std::getline(iss, line))
	{
		if (line.size() < min_line_length)
		{
			continue;
		}

		auto class_name = line.substr(31);

		if (class_name != clr_exact_type_name)
		{
			continue;
		}

		auto methodTableText = line.substr(0, 8);

		try{
			auto methodTable = std::stoul(methodTableText, nullptr, 16);

			ret->push_back(methodTable);
		}
		catch (std::invalid_argument)
		{
			_logger->Log("Method table cannot be read: %s\n", line);
		}
	}

	return ret;
}
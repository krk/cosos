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
\file HandleCommandParser.cpp

Implements HandleCommandParser class that parses the output of the !handle command.
*/

#include "HtraceCommandParser.h"
#include "HtraceCommandOutput.h"
#include <sstream>

/**
Executes a htrace command and parses the output.

\param handle Value of the handle.
*/
HtraceCommandOutput HtraceCommandParser::execute(unsigned long handle)
{
	std::stringstream sstream;
	sstream << std::hex << handle;
	auto hex_handle = sstream.str();

	auto command = _command + " " + hex_handle;

	std::string htrace_output;

	if (!_executor->ExecuteCommand(command, htrace_output))
	{
		_logger->Log("Cannot get handle trace.\n");

		return HtraceCommandOutput();
	}

	/* Find last opener. */
	auto open_index = htrace_output.find(" - OPEN");
	bool is_thread_id_found = false;

	if (open_index != std::string::npos)
	{
		auto thread_id_index = htrace_output.find("Thread ID = 0x", open_index);

		if (thread_id_index != std::string::npos)
		{
			auto thread_id_hex = htrace_output.substr(thread_id_index + 14, 8);
			auto last_opener_thread = std::stoul(thread_id_hex, nullptr, 16);

			return HtraceCommandOutput(last_opener_thread);
		}
	}

	return HtraceCommandOutput();
}

/**
Returns true if handle tracing is enabled for the session.
*/
bool HtraceCommandParser::is_enabled()
{
	std::string htrace_detect_output;

	if (!_executor->ExecuteCommand("!htrace 1", htrace_detect_output))
	{
		_logger->Log("Cannot get htrace state.\n");

		return false;
	}

	// Warning suggests to turn on htrace.
	auto is_htrace_enabled = htrace_detect_output.find("!htrace -enable") == std::string::npos;

	return is_htrace_enabled;
}
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

#include "HandleCommandParser.h"
#include "HandleCommandOutput.h"
#include <sstream>

/**
Executes a handle command and parses the output.

\param handle Value of the handle.
*/
HandleCommandOutput HandleCommandParser::execute(unsigned long handle)
{
	std::stringstream sstream;
	sstream << std::hex << handle;
	auto hex_handle = sstream.str();

	auto handle_command = _command + " " + hex_handle;

	std::string handle_output;

	if (!_executor->ExecuteCommand(handle_command, handle_output))
	{
		_logger->Log("Cannot get handle type for %x.\n", handle);

		return HandleCommandOutput("");
	}

	std::string handle_type;

	/* Get last word */
	auto last_tab_index = handle_output.find_last_of('\t');

	if (last_tab_index != std::string::npos)
	{
		handle_type = handle_output.substr(last_tab_index + 1, handle_output.length() - last_tab_index - 2);
	}

	return HandleCommandOutput(handle_type);
}

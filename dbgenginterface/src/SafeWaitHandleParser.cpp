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

#include "SafeWaitHandleParser.h"
#include "SafeWaitHandleOutput.h"

/**
Executes a htrace command and parses the output.

\param handle Value of the handle.
*/
SafeWaitHandleOutput SafeWaitHandleParser::execute(const DumpHeapCommandOutput& dump_heap_output)
{
	if (!dump_heap_output.has_addresses())
	{
		return SafeWaitHandleOutput();
	}

	auto map = parse(*dump_heap_output.get_addresses());

	auto handle_addresses = std::shared_ptr<std::map<unsigned long, unsigned long>>(map);

	return SafeWaitHandleOutput(handle_addresses);
}

/**
Build a map of SafeWaitHandle values to SafeWaitHandle object addresses.
*/
std::map<unsigned long, unsigned long>* SafeWaitHandleParser::parse(const std::vector<unsigned long>& object_addresses)
{
	auto ret = new std::map<unsigned long, unsigned long>();

	for (auto address : object_addresses)
	{
		if (address == 0)
		{
			continue;
		}

		unsigned long handle_value = 0;

		unsigned long bytes_read = 0;

		// read memory.
		_reader->ReadMemory(address + 4, &handle_value, sizeof(unsigned long), &bytes_read);

		if (!bytes_read)
		{
			continue;
		}

		(*ret)[handle_value] = address;
	}

	return ret;
}
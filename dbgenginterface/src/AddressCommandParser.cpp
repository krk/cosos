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
\file AddressParser.cpp

Implements AddressParser class that parses native !address output.
*/

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "AddressCommandParser.h"

/**
Executes address command and parses the output.

\param handle Value of the handle.
*/
AddressCommandOutput AddressCommandParser::execute()
{
	std::string output;

	if (!_executor->ExecuteCommand(_command, output))
	{
		_logger->Log("Cannot get address info.\n");

		return AddressCommandOutput();
	}

	auto ranges = Parse(output);

	return AddressCommandOutput(RangeList(ranges));
}

/**
Parses tokenized strings in a single line of an address output to find the usage information.

\param items Line tokens.
*/
Usage AddressCommandParser::GetUsage(const std::vector<std::string>& items)
{
	auto ret = Usage::Undefined;

	for (auto item : items)
	{
		if (item.find("RegionUsageIsVAD") != std::string::npos)
			ret = Usage::VirtualAlloc;
		else if (item.find("Free") != std::string::npos)
			ret = Usage::Free;
		else if (item.find("Image") != std::string::npos)
			ret = Usage::Image;
		else if (item.find("Stack") != std::string::npos)
			ret = Usage::Stack;
		else if (item.find("Teb") != std::string::npos)
			ret = Usage::TEB;
		else if (item.find("Heap") != std::string::npos)
			ret = Usage::Heap;
		else if (item.find("PageHeap") != std::string::npos)
			ret = Usage::PageHeap;
		else if (item.find("Peb") != std::string::npos)
			ret = Usage::PEB;
		else if (item.find("ProcessParameters") != std::string::npos)
			ret = Usage::ProcessParameters;
		else if (item.find("EnvironmentBlock") != std::string::npos)
			ret = Usage::EnvironmentBlock;
	}

	return ret;
}

/**
Parses tokenized strings in a single line of an address output to find the state information.

\param items Line tokens.
*/
State AddressCommandParser::GetState(const std::vector<std::string>& items)
{
	auto ret = State::Commit;

	for (auto item : items)
	{
		if (item.find("MEM_COMMIT") != std::string::npos)
			ret = State::Commit;
		else if (item.find("MEM_FREE") != std::string::npos)
			ret = State::Free;
		else if (item.find("MEM_RESERVE") != std::string::npos)
			ret = State::Reserve;
	}

	return ret;
}

/**
Parses lines of an address output to find the range information.

\param lines Address output lines.
*/
std::vector<const MemoryRange>* AddressCommandParser::Parse(const std::string& lines)
{
	auto ret = new std::vector<const MemoryRange>();

	std::istringstream iss(lines);

	std::string line;

	int currentLine = 0;

	while (std::getline(iss, line) && line.find("BaseAddr") == std::string::npos)
	{
		currentLine++;
	}

	// Skip one line.
	std::getline(iss, line);
	currentLine++;

	while (std::getline(iss, line))
	{
		if (line.size() == 0)
			break;

		//look for +    50000    51000     1000 MEM_IMAGE   MEM_COMMIT  PAGE_READONLY                      Image
		if (line[0] != '-')
		{
			//create a new block and read all the lines until we reach the next address line
			//if we have reached the FullPath line we are also done so we can just look for the next - line
			auto addressText = line.substr(2, 8);
			auto sizeText = line.substr(20, 8);
			auto restText = line.substr(28, line.size() - 28);

			std::istringstream issRest(restText);

			std::vector<std::string> tokens{
				std::istream_iterator<std::string>{issRest},
				std::istream_iterator<std::string>{} };

			std::vector<std::string> rest;

			std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(rest), [](std::string item){return item.size() > 0; });

			unsigned long address;
			unsigned long size;

			address = std::stoul(addressText, nullptr, 16);

			size = std::stoul(sizeText, nullptr, 16);

			auto usage = GetUsage(rest);
			auto state = GetState(rest);

			auto mo = MemoryRange(address, size, state, usage);

			ret->push_back(mo);
		}
		else
		{
			break;
		}

		currentLine++;
	}

	return ret;
}
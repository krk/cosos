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

#include "EEHeapParser.h"

/**
Parses lines of an address output to find the range information.

\param lines Address output lines.
*/
std::vector<const MemoryRange>* EEHeapParser::Parse(const std::string& lines)
{
	auto ret = new std::vector<const MemoryRange>();

	std::istringstream iss(lines);

	std::string line;

	int currentLine = 0;

	//parse the lines, finding
	/*
	segment    begin allocated     size
	02d90000 02d90038  06d83540 0x03ff3508(67056904)
	16fd0000 16fd0038  1a894fb4 0x038c4f7c(59527036)
	1efd0000 1efd0038  22a1d074 0x03a4d03c(61132860)
	Large object heap starts at 0x0ad90038
	segment    begin allocated     size
	0ad90000 0ad90038  0ada2b68 0x00012b30(76592)
	Heap Size  0xb317ff0(187793392)
	*/


	//go until we are out of lines or reach the "GC Heap Size" line
	while (std::getline(iss, line) && line.find("GC Heap Size") == std::string::npos)
	{
		if (line.find("allocated") != std::string::npos)
		{
			currentLine++;
			//get the small object heaps
			while (std::getline(iss, line) && line.find("Large") == std::string::npos)
			{
				auto addressText = line.substr(0, 8);

				auto openParanIndex = line.find('(');
				auto closeParanIndex = line.find(')');

				auto sizeText = line.substr(openParanIndex + 1, closeParanIndex - openParanIndex - 1);

				unsigned long address;
				unsigned long size;

				address = std::stoul(addressText, nullptr, 16);

				size = std::stoul(sizeText, nullptr, 10);

				auto mo = MemoryRange(address, size, State::Commit, Usage::GCHeap);

				ret->push_back(mo);

				currentLine++;
			}

			// Skip one line.
			std::getline(iss, line);
			currentLine++;

			//get the Large object heaps
			while (std::getline(iss, line) && line.find("Total") == std::string::npos)
			{
				auto addressText = line.substr(0, 8);

				auto openParanIndex = line.find('(');
				auto closeParanIndex = line.find(')');

				auto sizeText = line.substr(openParanIndex + 1, closeParanIndex - openParanIndex - 1);

				unsigned long address;
				unsigned long size;

				address = std::stoul(addressText, nullptr, 16);

				size = std::stoul(sizeText, nullptr, 10);

				auto mo = MemoryRange(address, size, State::Commit, Usage::GCLOHeap);

				ret->push_back(mo);

				currentLine++;
			}
		}

		currentLine++;
	}

	return ret;
}
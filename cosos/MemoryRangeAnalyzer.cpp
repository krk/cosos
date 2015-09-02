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
\file MemoryRangeAnalyzer.cpp

Implements MemoryRangeAnalyzer class that can find details of a group of MemoryRange instances.
*/

#include "MemoryRangeAnalyzer.h"

#include <algorithm>
#include <iterator>

/**
Finds the maximum contiguous free block size in the given ranges.

\param ranges Memory ranges.
*/
unsigned long MemoryRangeAnalyzer::GetMaxContiguousFreeBlockSize(RangeList ranges)
{
	if (ranges->size() == 0)
	{
		return UNDETERMINED_SIZE;
	}

	std::vector<const MemoryRange> non_free_ranges;

	std::copy_if(ranges->begin(), ranges->end(), std::back_inserter(non_free_ranges), [](const MemoryRange &item) { return item.Usage != Usage::Free; });

	std::sort(non_free_ranges.begin(), non_free_ranges.end(), [](const MemoryRange& left, const MemoryRange& right){ return left.Address < right.Address; });

	unsigned long max = 0;

	unsigned long prev_finish_address = 0;

	for (auto range : non_free_ranges)
	{
		auto free_block_size = range.Address - prev_finish_address;

		if (free_block_size > max)
		{
			max = free_block_size;
		}

		prev_finish_address = range.Address + range.Size;
	}

	return max;
}

/**
Finds the minimum contiguous LOH size in the given ranges.

\param ranges Memory ranges.
*/
unsigned long MemoryRangeAnalyzer::GetMinContiguousLOHHeapSize(RangeList ehRanges)
{
	if (ehRanges->size() == 0)
	{
		return UNDETERMINED_SIZE;
	}

	auto range = *std::min_element(ehRanges->begin(), ehRanges->end(), [](const MemoryRange& left, const MemoryRange& right){ return left.Usage != Usage::GCLOHeap ? LONG_MAX < 0 : left.Size < right.Size; });

	return 0;
}

/**
Finds the minimum contiguous SOH size in the given ranges.

\param ranges Memory ranges.
*/
unsigned long MemoryRangeAnalyzer::GetMinContiguousSOHHeapSize(RangeList ehRanges)
{
	if (ehRanges->size() == 0)
	{
		return UNDETERMINED_SIZE;
	}

	auto range = *std::min_element(ehRanges->begin(), ehRanges->end(), [](const MemoryRange& left, const MemoryRange& right){ return left.Usage != Usage::GCHeap ? LONG_MAX < 0 : left.Size < right.Size; });

	return range.Size;
}
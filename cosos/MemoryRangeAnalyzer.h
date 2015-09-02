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
\file MemoryRangeAnalyzer.h

Defines the MemoryRangeAnalyzer class.
*/

#ifndef __MEMORYRANGEANALYZER_H__

#define __MEMORYRANGEANALYZER_H__

#include <vector>
#include <memory>

#include "MemoryRange.h"

/**
\class MemoryRangeAnalyzer

Analyzes memory ranges for maximum contiguous free block and other details.
*/
class MemoryRangeAnalyzer
{
public:
	static const unsigned long UNDETERMINED_SIZE = ULONG_MAX - 1;

	static unsigned long MemoryRangeAnalyzer::GetMaxContiguousFreeBlockSize(RangeList ranges);
	static unsigned long MemoryRangeAnalyzer::GetMinContiguousLOHHeapSize(RangeList ehRanges);
	static unsigned long MemoryRangeAnalyzer::GetMinContiguousSOHHeapSize(RangeList ehRanges);
};

#endif // #ifndef __MEMORYRANGEANALYZER_H__
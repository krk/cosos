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
\file MemoryRangeAnalyzerTest.cpp

Implements MemoryRangeAnalyzerTest class defines unit tests for MemoryRangeAnalyzer class.
*/

#include "..\stdafx.h"

#include "MemoryRangeAnalyzer.h"

TEST(MemoryRangeAnalyzer, GetMaxContiguousFreeBlockSize_nullptr)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto size = analyzer.GetMaxContiguousFreeBlockSize(nullptr);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMaxContiguousFreeBlockSize_empty)
{
	auto analyzer = MemoryRangeAnalyzer();

	RangeList ranges = RangeList();

	auto size = analyzer.GetMaxContiguousFreeBlockSize(ranges);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMaxContiguousFreeBlockSize_oneblock)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 1, State::Commit, Usage::EnvironmentBlock));

	auto size = analyzer.GetMaxContiguousFreeBlockSize(RangeList(ranges));

	EXPECT_EQ(size, 0);
}

TEST(MemoryRangeAnalyzer, GetMaxContiguousFreeBlockSize_twoblocks)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 1, State::Commit, Usage::EnvironmentBlock));
	ranges->push_back(MemoryRange(101, 1001, State::Commit, Usage::EnvironmentBlock));

	auto size = analyzer.GetMaxContiguousFreeBlockSize(RangeList(ranges));

	EXPECT_EQ(size, 100);
}

TEST(MemoryRangeAnalyzer, GetMaxContiguousFreeBlockSize_threeblocks)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 1, State::Commit, Usage::EnvironmentBlock));
	ranges->push_back(MemoryRange(101, 1000, State::Commit, Usage::EnvironmentBlock));
	ranges->push_back(MemoryRange(10101, 1010101, State::Commit, Usage::EnvironmentBlock));

	auto size = analyzer.GetMaxContiguousFreeBlockSize(RangeList(ranges));

	EXPECT_EQ(size, 9000);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousLOHHeapSize_nullptr)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto size = analyzer.GetMinContiguousLOHHeapSize(nullptr);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousLOHHeapSize_empty)
{
	auto analyzer = MemoryRangeAnalyzer();

	RangeList ranges = RangeList();

	auto size = analyzer.GetMinContiguousLOHHeapSize(ranges);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousLOHHeapSize_oneblock_nonloh)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 1, State::Commit, Usage::EnvironmentBlock));

	auto size = analyzer.GetMinContiguousLOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousLOHHeapSize_oneblock)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 101, State::Commit, Usage::GCLOHeap));

	auto size = analyzer.GetMinContiguousLOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, 101);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousLOHHeapSize_threeblocks)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 10000, State::Commit, Usage::GCLOHeap));
	ranges->push_back(MemoryRange(20000, 20000, State::Commit, Usage::GCLOHeap));
	ranges->push_back(MemoryRange(50000, 0x2f000, State::Commit, Usage::GCLOHeap));

	auto size = analyzer.GetMinContiguousLOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, 10000);
}


TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_nullptr)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto size = analyzer.GetMinContiguousSOHHeapSize(nullptr);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_empty)
{
	auto analyzer = MemoryRangeAnalyzer();

	RangeList ranges = RangeList();

	auto size = analyzer.GetMinContiguousSOHHeapSize(ranges);

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_oneblock_nonsoh)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 1, State::Commit, Usage::EnvironmentBlock));

	auto size = analyzer.GetMinContiguousSOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_oneblock_nonsoh2)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 101, State::Commit, Usage::GCLOHeap));

	auto size = analyzer.GetMinContiguousSOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, MemoryRangeAnalyzer::UNDETERMINED_SIZE);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_oneblock)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 101, State::Commit, Usage::GCHeap));

	auto size = analyzer.GetMinContiguousSOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, 101);
}

TEST(MemoryRangeAnalyzer, GetMinContiguousSOHHeapSize_threeblocks)
{
	auto analyzer = MemoryRangeAnalyzer();

	auto ranges = std::make_shared<std::vector<const MemoryRange>>();

	ranges->push_back(MemoryRange(0, 10000, State::Commit, Usage::GCHeap));
	ranges->push_back(MemoryRange(20000, 20000, State::Commit, Usage::GCHeap));
	ranges->push_back(MemoryRange(50000, 0x2f000, State::Commit, Usage::GCHeap));

	auto size = analyzer.GetMinContiguousSOHHeapSize(RangeList(ranges));

	EXPECT_EQ(size, 10000);
}
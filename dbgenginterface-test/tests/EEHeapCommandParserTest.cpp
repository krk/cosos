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
\file EEHeapCommandParserTest.cpp

Implements EEHeapCommandParserTest class defines unit tests for EEHeapCommandParser class.
*/

#include "..\stdafx.h"

#include "EEHeapCommandParser.h"
#include "FakeDebuggerCommandExecutor.h"
#include "FakeLogger.h"

TEST(EEHeapCommandParser, CannotRunCommand)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return false;
	}));

	auto logger = new FakeLogger();

	auto parser = EEHeapCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_FALSE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 1);

	delete executor;
	delete logger;
}

TEST(EEHeapCommandParser, InvalidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(abc)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = EEHeapCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_FALSE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}


TEST(EEHeapCommandParser, ValidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(Number of GC Heaps: 1
generation 0 starts at 0x3def26f8
generation 1 starts at 0x3de57a5c
generation 2 starts at 0x02851000
ephemeral segment allocation context: none
         segment             begin         allocated  size
02850000  02851000  0384fe0c  0xffee0c(16772620)
47980000  47981000  4897fe8c  0xffee8c(16772748)
29730000  29731000  2a72fde0  0xffede0(16772576)
2c730000  2c731000  2d72fce8  0xffece8(16772328)
22520000  22521000  2351fdc8  0xffedc8(16772552)
26ab0000  26ab1000  27aaff8c  0xffef8c(16773004)
2b730000  2b731000  2c72fe4c  0xffee4c(16772684)
326e0000  326e1000  3342f7d8  0xd4e7d8(13952984)
2d730000  2d731000  2e348998  0xc17998(12679576)
2e730000  2e731000  2f53b420  0xe0a420(14722080)
346e0000  346e1000  35555808  0xe74808(15157256)
3f980000  3f981000  40971fb0  0xff0fb0(16715696)
379d0000  379d1000  38950710  0xf7f710(16250640)
399d0000  399d1000  3a9cff28  0xffef28(16772904)
3d9d0000  3d9d1000  3e50deec  0xb3ceec(11783916)
Large object heap starts at 0x03851000
         segment             begin         allocated  size
03850000  03851000  0481d4b8  0xfcc4b8(16565432)
253a0000  253a1000  261c3ac8  0xe22ac8(14822088)
23520000  23521000  23564140  0x43140(274752)
2a730000  2a731000  2a897398  0x166398(1467288)
2f730000  2f731000  30594c38  0xe63c38(15088696)
30730000  30731000  30f61780  0x830780(8587136)
389d0000  389d1000  3947cfb0  0xaabfb0(11190192)
Total Size:              Size: 0x12161d2c (303439148) bytes.
------------------------------
GC Heap Size:    Size: 0x12161d2c (303439148) bytes.)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = EEHeapCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_TRUE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 0);

	auto ranges = output.get_ranges();

	EXPECT_EQ(ranges->size(), 22);

	EXPECT_EQ(ranges->at(0).Address, 0x02851000);
	EXPECT_EQ(ranges->at(0).Size, 0xffee0c);
	EXPECT_EQ(ranges->at(0).State, State::Commit);
	EXPECT_EQ(ranges->at(0).Usage, Usage::GCHeap);

	EXPECT_EQ(ranges->at(21).Address, 0x389d1000);
	EXPECT_EQ(ranges->at(21).Size, 0xaabfb0);
	EXPECT_EQ(ranges->at(21).State, State::Commit);
	EXPECT_EQ(ranges->at(21).Usage, Usage::GCLOHeap);

	delete executor;
	delete logger;
}
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
\file AddressCommandParserTest.cpp

Implements AddressCommandParserTest class defines unit tests for AddressCommandParser class.
*/

#include "..\stdafx.h"

#include "AddressCommandParser.h"
#include "FakeDebuggerCommandExecutor.h"
#include "FakeLogger.h"

TEST(AddressCommandParser, CannotRunCommand)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return false;
	}));

	auto logger = new FakeLogger();

	auto parser = AddressCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_FALSE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 1);

	delete executor;
	delete logger;
}

TEST(AddressCommandParser, InvalidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(abc)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = AddressCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_FALSE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}


TEST(AddressCommandParser, ValidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(  BaseAddr EndAddr+1 RgnSize     Type       State                 Protect             Usage
-----------------------------------------------------------------------------------------------
+        0   3c0000   3c0000             MEM_FREE    PAGE_NOACCESS                      Free       
+   3c0000   3c1000     1000 MEM_IMAGE   MEM_COMMIT  PAGE_READONLY                      Image      
    3c1000   3c2000     1000 MEM_IMAGE   MEM_RESERVE                                    Image      
    3c2000   62b000   269000 MEM_IMAGE   MEM_COMMIT  PAGE_READONLY                      Image      
    62b000   62c000     1000 MEM_IMAGE   MEM_RESERVE                                    Image      
    62c000   62d000     1000 MEM_IMAGE   MEM_COMMIT  PAGE_READONLY                      Image      
    62d000   62e000     1000 MEM_IMAGE   MEM_RESERVE                                    Image      
    62e000   64b000    1d000 MEM_IMAGE   MEM_COMMIT  PAGE_READONLY                      Image      
    64b000   64c000     1000 MEM_IMAGE   MEM_RESERVE                                    Image      
)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = AddressCommandParser(executor, logger);

	auto output = parser.execute();

	EXPECT_TRUE(output.has_ranges());
	EXPECT_EQ(logger->_logs.size(), 0);

	auto ranges = output.get_ranges();

	EXPECT_EQ(ranges->size(), 9);

	EXPECT_EQ(ranges->at(0).Address, 0);
	EXPECT_EQ(ranges->at(0).Size, 0x3c0000);
	EXPECT_EQ(ranges->at(0).State, State::Free);
	EXPECT_EQ(ranges->at(0).Usage, Usage::Free);

	EXPECT_EQ(ranges->at(7).Address, 0x62e000);
	EXPECT_EQ(ranges->at(7).Size, 0x1d000);
	EXPECT_EQ(ranges->at(7).State, State::Commit);
	EXPECT_EQ(ranges->at(7).Usage, Usage::Image);

	delete executor;
	delete logger;
}
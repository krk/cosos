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
\file HandleCommandParserTest.cpp

Implements HandleCommandParserTest class defines unit tests for HandleCommandParser class.
*/

#include "..\stdafx.h"

#include "HandleCommandParser.h"
#include "FakeDebuggerCommandExecutor.h"
#include "FakeLogger.h"

TEST(HandleCommandParser, CannotRunCommand)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return false;
	}));

	auto logger = new FakeLogger();

	auto parser = HandleCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_EQ(output.get_type().size(), 0);
	EXPECT_EQ(logger->_logs.size(), 1);

	delete executor;
	delete logger;
}

TEST(HandleCommandParser, InvalidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HandleCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_EQ(output.get_type().size(), 0);
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}


TEST(HandleCommandParser, ValidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc\n\tSemaphore\n";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HandleCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_EQ(output.get_type(), "Semaphore");
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}
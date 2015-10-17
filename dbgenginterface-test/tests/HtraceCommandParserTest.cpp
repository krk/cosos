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

#include "HtraceCommandParser.h"
#include "FakeDebuggerCommandExecutor.h"
#include "FakeLogger.h"

TEST(HtraceCommandParser, CannotRunCommand)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return false;
	}));

	auto logger = new FakeLogger();

	auto parser = HtraceCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_FALSE(output.has_thread_id());
	EXPECT_EQ(logger->_logs.size(), 1);

	delete executor;
	delete logger;
}

TEST(HtraceCommandParser, InvalidOutput)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HtraceCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_FALSE(output.has_thread_id());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}

TEST(HtraceCommandParser, InvalidOutput2)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = "abc\n - OPEN\nabc";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HtraceCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_FALSE(output.has_thread_id());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}

TEST(HtraceCommandParser, InvalidOutput3)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(--------------------------------------
Handle = 0x00000780 - OPEN
Thread ID = 0xQQQQQQQQ, Process ID = 0x00001674

0xeb39cf6d: +0xeb39cf6d
0xeafe0963 : +0xeafe0963
0x4d1140aa : +0x4d1140aa
0x50f03e8c : +0x50f03e8c
0x50f07d15 : +0x50f07d15
0x50ef1927 : +0x50ef1927
0x50f0df82 : +0x50f0df82
0x50f0dea7 : +0x50f0dea7
0x4d1405d7 : +0x4d1405d7
0x4d134047 : +0x4d134047
0x4d0efb7e : +0x4d0efb7e
0x7704979c : ntdll!NtCreateWaitCompletionPacket + 0x0000000c
0x7700f394 : ntdll!TpAllocWait + 0x00000084
0x77044650 : ntdll!RtlpWnfRegisterTpNotification + 0x0000003f
0x770445f5 : ntdll!RtlpInitializeWnf + 0x00000095
0x7703fe6d : ntdll!RtlSubscribeWnfStateChangeNotificationInternal + 0x0000003e

--------------------------------------
Parsed 0x17EF stack traces.
Dumped 0x1 stack traces.
			)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HtraceCommandParser(executor, logger);

	HtraceCommandOutput output;
	try{
		output = parser.execute(1);
	}
	catch (std::exception ex)
	{
		EXPECT_STREQ(ex.what(), "invalid stoul argument");
	}

	EXPECT_FALSE(output.has_thread_id());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}


TEST(HtraceCommandParser, ValidOutput3)
{
	IDebuggerCommandExecutor *executor = new FakeDebuggerCommandExecutor(FakeDebuggerCommandExecutor::OutputLambda([&](const std::string& command, std::string& output)
	{
		output = R"(--------------------------------------
Handle = 0x00000780 - OPEN
Thread ID = 0x12345678, Process ID = 0x00001674

0xeb39cf6d: +0xeb39cf6d
0xeafe0963 : +0xeafe0963
0x4d1140aa : +0x4d1140aa
0x50f03e8c : +0x50f03e8c
0x50f07d15 : +0x50f07d15
0x50ef1927 : +0x50ef1927
0x50f0df82 : +0x50f0df82
0x50f0dea7 : +0x50f0dea7
0x4d1405d7 : +0x4d1405d7
0x4d134047 : +0x4d134047
0x4d0efb7e : +0x4d0efb7e
0x7704979c : ntdll!NtCreateWaitCompletionPacket + 0x0000000c
0x7700f394 : ntdll!TpAllocWait + 0x00000084
0x77044650 : ntdll!RtlpWnfRegisterTpNotification + 0x0000003f
0x770445f5 : ntdll!RtlpInitializeWnf + 0x00000095
0x7703fe6d : ntdll!RtlSubscribeWnfStateChangeNotificationInternal + 0x0000003e

--------------------------------------
Parsed 0x17EF stack traces.
Dumped 0x1 stack traces.
			)";

		return true;
	}));

	auto logger = new FakeLogger();

	auto parser = HtraceCommandParser(executor, logger);

	auto output = parser.execute(1);

	EXPECT_EQ(output.get_thread_id(), 0x12345678);
	EXPECT_TRUE(output.has_thread_id());
	EXPECT_EQ(logger->_logs.size(), 0);

	delete executor;
	delete logger;
}

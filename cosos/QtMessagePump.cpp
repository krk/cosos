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
\file QtMessagePump.cpp

Implements QtMessagePump class that controls a windows message pump for handling Qt messages.
*/

#include "QtMessagePump.h"

#include <process.h>
#include <engextcpp.hpp>

/**
Implements a Windows message pump.
*/
unsigned int QtMessagePump::MessagePumpImpl()
{
	if (!app)
	{
		app = new QApplication(__argc, __argv);
	}

	window = new CososMainWindow();

	window->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

	window->show();

	SetEvent(hWindowReadyEvent);

	auto ret = app->exec();

	ResetEvent(hWindowReadyEvent);

	delete window;
	window = nullptr;

	delete app;
	app = nullptr;

	_endthreadex(0);

	return ret;
}

/**
Implements a Windows message pump.

\param pArguments Pointer to a QtMessagePump instance.
*/
unsigned int __stdcall QtMessagePump::MessagePump(void* pArguments)
{
	auto pump = static_cast<QtMessagePump*>(pArguments);

	return pump->MessagePumpImpl();
}

/**
Starts the message pump.
*/
void QtMessagePump::StartMessagePump()
{
	StopMessagePump();

	if (!hWindowReadyEvent)
	{
		hWindowReadyEvent = CreateEvent(NULL, true, false, L"WindowAlmostReady");

		if (!hWindowReadyEvent)
		{
			dprintf("ERROR: Cannot create event.");

			return;
		}
	}

	unsigned int threadId;

	hMessagePumpThread = (HANDLE) _beginthreadex(NULL, 0, &MessagePump, this, 0, &threadId);
}

/**
Stops the message pump.
*/
void QtMessagePump::StopMessagePump()
{
	if (hMessagePumpThread)
	{
		if (window)
		{
			QMetaObject::invokeMethod(window, "close", Qt::QueuedConnection);

			// Give window a chance to close.
			WaitForSingleObject(hMessagePumpThread, 5000);
		}

		CloseHandle(hMessagePumpThread);

		hMessagePumpThread = nullptr;
	}

	if (hWindowReadyEvent)
	{
		CloseHandle(hWindowReadyEvent);

		hWindowReadyEvent = nullptr;
	}
}

/**
Ensures there is a running message pump.
*/
void QtMessagePump::EnsureMessagePump()
{
	if (!window || !window->isVisible())
	{
		StopMessagePump();
		StartMessagePump();

		WaitForSingleObject(hWindowReadyEvent, INFINITE);
	}
}

/**
Posts an updateRanges message to the Qt Window.

\param ranges MemoryRanges.
*/
void QtMessagePump::postUpdateRangesMessage(RangeList ranges)
{
	window->GcViewDescriptor._ranges = ranges;

	QMetaObject::invokeMethod(window, "updateImages", Qt::QueuedConnection);
}

/**
Posts an updateGCRanges message to the Qt Window.

\param ranges MemoryRanges.
*/
void QtMessagePump::postUpdateGCRangesMessage(RangeList ranges)
{
	window->GcViewDescriptor._gcRanges = ranges;

	QMetaObject::invokeMethod(window, "updateImages", Qt::QueuedConnection);
}

/**
Posts an updateInfos message to the Qt Window.

\param freeBlockInfo Free block information.
\param gcInfo1 Text information line 1.
\param gcInfo2 Text informaiton line 2.
*/
void QtMessagePump::postUpdateInfosMessage(const std::string& freeBlockInfo, const std::string& gcInfo1, const std::string& gcInfo2)
{
	window->GcViewDescriptor._freeblockinfo = freeBlockInfo;
	window->GcViewDescriptor._gcInfo1 = gcInfo1;
	window->GcViewDescriptor._gcInfo2 = gcInfo2;

	QMetaObject::invokeMethod(window, "updateInfos", Qt::QueuedConnection);
}
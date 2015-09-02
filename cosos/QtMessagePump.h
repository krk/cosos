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
\file QtMessagePump.h

Defines the QtMessagePump class.
*/

#ifndef __DEBUGGERCONTROLLER_H__

#define __DEBUGGERCONTROLLER_H__

#define WIN32_LEAN_AND_MEAN

#include "memoryrange.h"
#include "cososmainwindow.h"

#include <windows.h>
#include <imagehlp.h>
#include <wdbgexts.h>
#include <dbgeng.h>
#include <vector>
#include <memory>
#include <QtWidgets/QApplication>

/**
\class QtMessagePump

Windows message pump for displaying Qt windows.
*/
class QtMessagePump
{
private:
	unsigned int MessagePumpImpl();

	static unsigned int __stdcall QtMessagePump::MessagePump(void* pArguments);

	QApplication *app = nullptr;
	CososMainWindow* window = nullptr;

	HANDLE hMessagePumpThread;
	HANDLE hWindowReadyEvent;

public:
	void postUpdateRangesMessage(RangeList ranges);

	void postUpdateGCRangesMessage(RangeList ranges);

	void postUpdateInfosMessage(const std::string& freeBlockInfo, const std::string& gcInfo1, const std::string& gcInfo2);

	void StartMessagePump();

	void StopMessagePump();

	void EnsureMessagePump();
};

#endif // #ifndef __DEBUGGERCONTROLLER_H__
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
\file StdioOutputCallbacks.cpp

Implements StdioOutputCallbacks class that handles debugging engine output callbacks.
*/

#include <windows.h>
#include <dbgeng.h>

#include "StdioOutputCallbacks.h"

StdioOutputCallbacks g_OutputCb;

/**
Implements IDebugOutputCallbacks::QueryInterface method.

\param InterfaceId InterfaceId.
\param Interface Interface.
*/
STDMETHODIMP
StdioOutputCallbacks::QueryInterface(
THIS_
IN REFIID InterfaceId,
OUT PVOID* Interface
)
{
	*Interface = NULL;
	if (IsEqualIID(InterfaceId, __uuidof(IUnknown)) ||
		IsEqualIID(InterfaceId, __uuidof(IDebugOutputCallbacks)))
	{
		*Interface = (IDebugOutputCallbacks *)this;
		AddRef();

		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

/**
Implements IDebugOutputCallbacks::AddRef method.
*/
STDMETHODIMP_(ULONG)
StdioOutputCallbacks::AddRef(
THIS
)
{
	// This class is designed to be static so
	// there's no true refcount.

	return 1;
}

/**
Implements IDebugOutputCallbacks::Release method.
*/
STDMETHODIMP_(ULONG)
StdioOutputCallbacks::Release(
THIS
)
{
	// This class is designed to be static so
	// there's no true refcount.

	return 0;
}

/**
Implements IDebugOutputCallbacks::Output method.

\param Mask Mask.
\param Text Text.
*/
STDMETHODIMP
StdioOutputCallbacks::Output(
THIS_
IN ULONG Mask,
IN PCSTR Text
)
{
	UNREFERENCED_PARAMETER(Mask);
	m_OutputBuffer += Text;

	return S_OK;
}

/**
Resets the buffer.
*/
void StdioOutputCallbacks::Reset()
{
	m_OutputBuffer.erase();
}
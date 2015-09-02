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
\file Extension.cpp

Implements Extension class that provides entry points to the debugging engine.
*/

#include <engextcpp.hpp>
#include <iostream>
#include <fstream>
#include <memory>

#include "AddressParser.h"
#include "EEHeapParser.h"
#include "QtMessagePump.h"
#include "MemoryRangeAnalyzer.h"
#include "StdioOutputCallbacks.h"
#include "GcViewDescriptor.h"

//----------------------------------------------------------------------------
//
// Base extension class.
// Extensions derive from the provided ExtExtension class.
//
// The standard class name is "Extension".  It can be
// overridden by providing an alternate definition of
// EXT_CLASS before including engextcpp.hpp.
//
//----------------------------------------------------------------------------

class EXT_CLASS : public ExtExtension
{
private:
	QtMessagePump _messagePump;

public:
	~EXT_CLASS();

	HRESULT Initialize(void) override;
	void Uninitialize(void) override;

	EXT_COMMAND_METHOD(gcview);
};

// EXT_DECLARE_GLOBALS must be used to instantiate
// the framework's assumed globals.
EXT_DECLARE_GLOBALS();

/**
Destructs an instance of the CososMainWindow class.
*/
EXT_CLASS::~EXT_CLASS()
{
	_messagePump.StopMessagePump();
}

/**
Handles Initialize callback from the debugging engine.
*/
HRESULT EXT_CLASS::Initialize()
{
	PDEBUG_CLIENT DebugClient;
	PDEBUG_CONTROL DebugControl;
	HRESULT Result = S_OK;

	DebugCreate(__uuidof(IDebugClient), (void **) &DebugClient);

	DebugClient->QueryInterface(__uuidof(IDebugControl), (void **) &DebugControl);

	ExtensionApis.nSize = sizeof(ExtensionApis);
	DebugControl->GetWindbgExtensionApis64(&ExtensionApis);

#if _DEBUG
	dprintf("COSOS v0.1.0 (%s) - Cousin of Son of Strike (DEBUG build) loaded.\n", __TIMESTAMP__);
#else
	dprintf("COSOS v0.1.0 (%s) - Cousin of Son of Strike loaded.\n", __TIMESTAMP__);
#endif

	DebugControl->Release();
	DebugClient->Release();

	return Result;
}

/**
Handles Uninitialize callback from the debugging engine.
*/
void EXT_CLASS::Uninitialize()
{
	_messagePump.StopMessagePump();

	this->Release();
}

/**
Implements gcview command of this extension.
*/
EXT_COMMAND(gcview,
	"Graphically shows the native and CLR heap memory layout of a process.",
	"{;x,o;;Bitmap file name without extension (optional)}" // Arguments: https://msdn.microsoft.com/en-us/library/windows/hardware/ff553340(v=vs.85).aspx
	)
{
	PDEBUG_CLIENT DebugClient;
	PDEBUG_CONTROL DebugControl;

	DebugCreate(__uuidof(IDebugClient), (void **) &DebugClient);

	DebugClient->QueryInterface(__uuidof(IDebugControl), (void **) &DebugControl);

	ExtensionApis.nSize = sizeof(ExtensionApis);
	DebugControl->GetWindbgExtensionApis64(&ExtensionApis);

	g_OutputCb.Reset();

	// Install output callbacks.
	if ((DebugClient->SetOutputCallbacks((PDEBUG_OUTPUT_CALLBACKS) &g_OutputCb)) != S_OK)
	{
		dprintf("Error while installing OutputCallback.\n\n");

		DebugControl->Release();
		DebugClient->Release();

		return;
	}

	dprintf("Reading addresses...\n");

	// Get address map.
	if (DebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT | //Send output to only outputcallbacks
		DEBUG_OUTCTL_OVERRIDE_MASK |
		DEBUG_OUTCTL_NOT_LOGGED,
		"!address",
		DEBUG_EXECUTE_DEFAULT) != S_OK)
	{
		dprintf("Executing !address failed\n");

		DebugControl->Release();
		DebugClient->Release();

		return;
	}

	auto addressOutput = std::string(g_OutputCb.GetOutputBuffer());

	g_OutputCb.Clear();

	auto addressParser = AddressParser();
	auto addresses = RangeList(addressParser.Parse(addressOutput));

	dprintf("Parsed %lu address blocks.\n", addresses->size());

	dprintf("Reading heap blocks...\n");

	// Get GC heap map.
	if (DebugControl->Execute(DEBUG_OUTCTL_THIS_CLIENT | //Send output to only outputcallbacks
		DEBUG_OUTCTL_OVERRIDE_MASK |
		DEBUG_OUTCTL_NOT_LOGGED,
		"!eeheap -gc",
		DEBUG_EXECUTE_DEFAULT) != S_OK)
	{
		dprintf("Executing !eeheap -gc failed\n");

		DebugControl->Release();
		DebugClient->Release();

		return;
	}

	auto eeheapOutput = std::string(g_OutputCb.GetOutputBuffer());

	g_OutputCb.Clear();

	auto eeheapParser = EEHeapParser();
	auto heapAddresses = RangeList(eeheapParser.Parse(eeheapOutput));

	dprintf("Parsed %lu eeheap blocks.\n", heapAddresses->size());

	bool save_only = this->HasUnnamedArg(0);
	auto filename = save_only ? this->GetUnnamedArgStr(0) : nullptr;

	if (!save_only)
	{
		_messagePump.EnsureMessagePump();

		_messagePump.postUpdateRangesMessage(addresses);

		_messagePump.postUpdateGCRangesMessage(heapAddresses);

		auto maxFreeBlockSize = MemoryRangeAnalyzer::GetMaxContiguousFreeBlockSize(addresses);
		auto minContigLOHBlockSize = MemoryRangeAnalyzer::GetMinContiguousLOHHeapSize(heapAddresses);
		auto minContigSOHBlockSize = MemoryRangeAnalyzer::GetMinContiguousSOHHeapSize(heapAddresses);

		auto message1 = std::string("Max contiguous free space size: ") + (maxFreeBlockSize == MemoryRangeAnalyzer::UNDETERMINED_SIZE ? "undetermined" : std::to_string(maxFreeBlockSize));
		auto message2 = std::string("Min contiguous LOH heap size: ") + (minContigLOHBlockSize == MemoryRangeAnalyzer::UNDETERMINED_SIZE ? "undetermined" : std::to_string(minContigLOHBlockSize));
		auto message3 = std::string("Min contiguous SOH heap size: ") + (minContigSOHBlockSize == MemoryRangeAnalyzer::UNDETERMINED_SIZE ? "undetermined" : std::to_string(minContigSOHBlockSize));

		_messagePump.postUpdateInfosMessage(message1, message2, message3);

		dprintf("gcview window updated.\n");
	}
	else
	{
		auto nativeFilename = std::string(filename) + ".png";
		auto gcFilename = std::string(filename) + "-gc.png";

		GcViewDescriptor::saveImages(addresses, heapAddresses, nativeFilename.c_str(), gcFilename.c_str());

		dprintf("gcview images saved.\n");
	}

	DebugClient->SetOutputCallbacks(nullptr);

	DebugControl->Release();
	DebugClient->Release();
}
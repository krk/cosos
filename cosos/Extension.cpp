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

#include "AddressCommandParser.h"
#include "EEHeapCommandParser.h"
#include "QtMessagePump.h"
#include "MemoryRangeAnalyzer.h"
#include "StdioOutputCallbacks.h"
#include "GcViewDescriptor.h"
#include "WaitApiStackParser.h"
#include "DbgEngCommandexecutor.h"
#include "HandleCommandParser.h"
#include "ILogger.h"
#include "DbgEngLogger.h"
#include "HtraceCommandParser.h"
#include "DbgEngMemoryReader.h"

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
	std::string ExecuteCommand(PDEBUG_CLIENT debug_client, PDEBUG_CONTROL debug_control, const std::string& command);

public:
	~EXT_CLASS();

	HRESULT Initialize(void) override;
	void Uninitialize(void) override;

	EXT_COMMAND_METHOD(gcview);
	EXT_COMMAND_METHOD(waitingforobjects);
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
	dprintf("COSOS v0.2.0 (%s) - Cousin of Son of Strike (DEBUG build) loaded.\n", __TIMESTAMP__);
#else
	dprintf("COSOS v0.2.0 (%s) - Cousin of Son of Strike loaded.\n", __TIMESTAMP__);
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
	"Graphically shows the native and CLR heap memory layout of a process (requires Qt 5.5).",
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

	IDebuggerCommandExecutor *executor = &DbgEngCommandExecutor(DebugClient, DebugControl);
	ILogger *logger = &DbgEngLogger();

	dprintf("Reading addresses...\n");

	// Get address map.
	auto addressCommandParser = AddressCommandParser(executor, logger);
	auto addressCommandOutput = addressCommandParser.execute();

	if (addressCommandOutput.has_ranges())
	{
		dprintf("Cannot get addresses.\n");

		return;
	}

	auto addresses = addressCommandOutput.get_ranges();

	dprintf("Parsed %lu address blocks.\n", addresses->size());

	dprintf("Reading heap blocks...\n");

	// Get GC heap map.
	auto eeheapParser = EEHeapCommandParser(executor, logger);
	auto eeheapOutput = eeheapParser.execute();

	if (eeheapOutput.has_ranges())
	{
		dprintf("Cannot get eeheap information.\n");

		return;
	}

	auto heapAddresses = eeheapOutput.get_ranges();

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

/**
Implements waitingforobjects command of this extension.
*/
EXT_COMMAND(waitingforobjects,
	"Finds kernel objects that are waited upon by threads.",
	"{dot;x,o;dot;Graphviz DOT file to export wait-chain to.}" // Arguments: https://msdn.microsoft.com/en-us/library/windows/hardware/ff553340(v=vs.85).aspx
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

	IDebuggerCommandExecutor *executor = &DbgEngCommandExecutor(DebugClient, DebugControl);
	ILogger *logger = &DbgEngLogger();

	// Get stack traces.
	std::string stackTracesOutput;

	if (!executor->ExecuteCommand("~*e ?@@c++(@$teb->ClientId.UniqueThread); kv 1;", stackTracesOutput))
	{
		dprintf("Cannot get stack traces.\n");

		return;
	}

	IMemoryReader *memory_reader = &DbgEngMemoryReader();

	auto wap = WaitApiStackParser(memory_reader, logger);

	auto handles = std::vector<std::pair<unsigned long, unsigned long>>();
	auto waited_upon_addresses = std::vector<std::pair<unsigned long, unsigned long>>();

	wap.GetHandlesAndAddresses(stackTracesOutput, handles, waited_upon_addresses);

	// Save wait graph.
	bool save_graph = this->HasUnnamedArg(0);
	auto filename = save_graph ? this->GetUnnamedArgStr(0) : nullptr;


	g_OutputCb.Clear();

	unsigned long prev_handle = 0;

	bool handles_found = false;

	std::ofstream dot_file;

	if (save_graph)
	{
		dot_file = std::ofstream(filename);

		dot_file << "digraph {\n";
	}

	// WaitOnAddress and waiting on handles are exclusive.
	for (auto waited_upon_address : waited_upon_addresses)
	{
		this->Dml("<?dml?><exec cmd=\"dd %x L1\">%x</exec> Address:\n", waited_upon_address.second, waited_upon_address.second);
		this->Dml("\t<?dml?><exec cmd=\"~~[%x]s\">%x</exec>\n", waited_upon_address.first, waited_upon_address.first);
	}

	HtraceCommandParser htraceCommandParser(executor, logger);

	auto is_htrace_enabled = htraceCommandParser.is_enabled();

	for (auto thread_handle : handles)
	{
		HandleCommandParser handleCommandParser(executor, logger);

		auto handle_output = handleCommandParser.execute(thread_handle.second);

		auto handle_type = handle_output.get_type();

		if (prev_handle != thread_handle.second)
		{
			if (is_htrace_enabled)
			{
				/* Find last opener. */
				auto htrace_output = htraceCommandParser.execute(thread_handle.second);

				bool is_thread_id_found = htrace_output.has_thread_id();

				if (is_thread_id_found)
				{
					auto last_opener_thread = htrace_output.get_thread_id();

					this->Dml("<?dml?><exec cmd=\"!handle %x f\">%x</exec> %s last opened by <?dml?><exec cmd=\"~~[%x]s\">%x</exec>:\n", thread_handle.second, thread_handle.second, handle_type.c_str(), last_opener_thread, last_opener_thread);

					if (save_graph)
					{
						dot_file << std::hex << "\"" << last_opener_thread << "\" [shape=cds]" << std::endl;
						dot_file << "\"" << thread_handle.second << " " << handle_type << "\" -> \"" << last_opener_thread << "\" [label = \"last opened by\"]" << std::endl;
					}
				}
				else
				{
					this->Dml("<?dml?><exec cmd=\"!handle %x f\">%x</exec> %s:\n", thread_handle.second, thread_handle.second, handle_type.c_str());
				}
			}
			else
			{
				this->Dml("<?dml?><exec cmd=\"!handle %x f\">%x</exec> %s:\n", thread_handle.second, thread_handle.second, handle_type.c_str());
			}
		}

		this->Dml("\t<?dml?><exec cmd=\"~~[%x]s\">%x</exec>\n", thread_handle.first, thread_handle.first);

		if (save_graph)
		{
			dot_file << std::hex << "\"" << thread_handle.first << "\" [shape=cds]" << std::endl;
			dot_file << "\"" << thread_handle.first << "\" -> \"" << std::hex << thread_handle.second << " " << handle_type << "\" [label = \"is waiting\"]" << std::endl;
		}

		handles_found = true;

		prev_handle = thread_handle.second;
	}

	if (save_graph)
	{
		dot_file << "}";
	}

	if (!handles_found)
	{
		dprintf("No waiting handles found in stack traces. WARNING: This does not prove threads aren't waiting on waitable objects.\n");
	}

	DebugClient->SetOutputCallbacks(nullptr);

	DebugControl->Release();
	DebugClient->Release();
}
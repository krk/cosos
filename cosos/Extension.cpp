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
#include "WaitApiStackParser.h"

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
	bool ExecuteCommand(PDEBUG_CLIENT debug_client, PDEBUG_CONTROL debug_control, const std::string& command, std::string& output);

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
	std::string addressOutput;

	if (!ExecuteCommand(DebugClient, DebugControl, "!address", addressOutput))
	{
		return;
	}

	auto addressParser = AddressParser();
	auto addresses = RangeList(addressParser.Parse(addressOutput));

	dprintf("Parsed %lu address blocks.\n", addresses->size());

	dprintf("Reading heap blocks...\n");

	// Get GC heap map.
	std::string eeheapOutput;

	if (!ExecuteCommand(DebugClient, DebugControl, "!eeheap -gc", eeheapOutput))
	{
		return;
	}

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

bool EXT_CLASS::ExecuteCommand(PDEBUG_CLIENT debug_client, PDEBUG_CONTROL debug_control, const std::string& command, std::string& output)
{
	g_OutputCb.Clear();

	if (debug_control->Execute(DEBUG_OUTCTL_THIS_CLIENT | //Send output to only outputcallbacks
		DEBUG_OUTCTL_OVERRIDE_MASK |
		DEBUG_OUTCTL_NOT_LOGGED,
		command.c_str(),
		DEBUG_EXECUTE_DEFAULT) != S_OK)
	{
		dprintf("handle details command failed\n");

		debug_control->Release();
		debug_client->Release();

		g_OutputCb.Clear();

		return false;
	}

	output = std::string(g_OutputCb.GetOutputBuffer());

	g_OutputCb.Clear();

	return true;
}

/**
Implements waitingforobjects command of this extension.
*/
EXT_COMMAND(waitingforobjects,
	"Finds kernel objects that are waited upon by threads.",
	"{;x,o;;msg}" // Arguments: https://msdn.microsoft.com/en-us/library/windows/hardware/ff553340(v=vs.85).aspx
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

	// Get stack traces.
	std::string stackTracesOutput;

	if (!ExecuteCommand(DebugClient, DebugControl, "~*e ?@@c++(@$teb->ClientId.UniqueThread); kv 1;", stackTracesOutput))
	{
		return;
	}

	auto wap = WaitApiStackParser();

	auto objectDescriptors = wap.Parse(stackTracesOutput);

	auto handles = std::vector<std::pair<unsigned long, unsigned long>>();

	for (auto descriptor : *objectDescriptors)
	{
		if (descriptor.is_value_address())
		{
			auto addresses = new ULONG[descriptor.get_count()];

			memset(addresses, 0xFFFFFFFF, descriptor.get_count());

			ULONG bytes_read;

			// read memory.
			ReadMemory(descriptor.get_value(), addresses, sizeof(ULONG) * descriptor.get_count(), &bytes_read);

			if (bytes_read)
			{
				for (int i = 0; i < descriptor.get_count(); i++)
				{
					handles.push_back(std::make_pair(descriptor.get_thread_id(), addresses[i]));
				}
			}

			delete addresses;
		}
		else
		{
			// value is handle.
			handles.push_back(std::make_pair(descriptor.get_thread_id(), descriptor.get_value()));
		}
	}

	delete objectDescriptors;

	std::sort(handles.begin(), handles.end(), [](std::pair<unsigned long, unsigned long> a, std::pair<unsigned long, unsigned long> b){ return a.second < b.second; });

	std::string htrace_detect_output;

	if (!ExecuteCommand(DebugClient, DebugControl, "!htrace 1", htrace_detect_output))
	{
		return;
	}

	// Save wait graph.
	bool save_graph = this->HasUnnamedArg(0);
	auto filename = save_graph ? this->GetUnnamedArgStr(0) : nullptr;

	// Warning suggests to turn on htrace.
	auto is_htrace_enabled = htrace_detect_output.find("!htrace -enable") == std::string::npos;

	g_OutputCb.Clear();

	unsigned long prev_handle = 0;

	bool handles_found = false;

	std::ofstream dot_file;

	if (save_graph)
	{
		dot_file = std::ofstream(filename);

		dot_file << "digraph {\n";
	}

	// TODO abstract-out. CommandExecutor interface, WinDbgCommandExecutor + UnitTestFakeCommandExecutor.
	for (auto thread_handle : handles)
	{
		g_OutputCb.Clear();

		std::stringstream sstream;
		sstream << std::hex << thread_handle.second;
		auto hex_handle = sstream.str();

		auto handle_command = std::string("!handle ") + hex_handle;

		std::string handle_output;

		if (!ExecuteCommand(DebugClient, DebugControl, handle_command, handle_output))
		{
			return;
		}

		std::string handle_type;

		/* Get last word */
		auto last_tab_index = handle_output.find_last_of('\t');

		if (last_tab_index != std::string::npos)
		{
			handle_type = handle_output.substr(last_tab_index + 1, handle_output.length() - last_tab_index - 2);
		}

		if (prev_handle != thread_handle.second)
		{
			if (is_htrace_enabled)
			{
				auto htrace_command = std::string("!htrace ") + hex_handle;

				std::string htrace_output;

				if (!ExecuteCommand(DebugClient, DebugControl, htrace_command, htrace_output))
				{
					return;
				}

				/* Find last opener. */
				auto open_index = htrace_output.find(" - OPEN");
				bool is_thread_id_found = false;

				if (open_index != std::string::npos)
				{
					auto thread_id_index = htrace_output.find("Thread ID = 0x", open_index);

					if (thread_id_index != std::string::npos)
					{
						auto thread_id_hex = htrace_output.substr(thread_id_index + 14, 8);
						auto last_opener_thread = std::stoul(thread_id_hex, nullptr, 16);

						this->Dml("<?dml?><exec cmd=\"!handle %x f\">%x</exec> %s last opened by <?dml?><exec cmd=\"~~[%x]s\">%x</exec>:\n", thread_handle.second, thread_handle.second, handle_type.c_str(), last_opener_thread, last_opener_thread);

						if (save_graph)
						{
							auto handle_node = "\"" + hex_handle + " " + handle_type + "\"";

							dot_file << last_opener_thread << " -> " << handle_node << "[label = \"last opened\"]" << std::endl;
						}

						is_thread_id_found = true;
					}
				}

				if (!is_thread_id_found)
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
			auto handle_node = "\"" + hex_handle + " " + handle_type + "\"";

			dot_file << thread_handle.first << " -> " << handle_node << "[label = \"is waiting\"]" << std::endl;
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
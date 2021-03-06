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
\file DumpHeapCommandParser.h

Defines the DumpHeapCommandParser class.
*/

#ifndef __DUMPHEAPCOMMANDPARSER_H__

#define __DUMPHEAPCOMMANDPARSER_H__

#include "MemoryRange.h"

#include <string>
#include <sstream>
#include <vector>

#include "IDebuggerCommandExecutor.h"
#include "ILogger.h"
#include "DumpHeapCommandOutput.h"
#include "MethodTableOutput.h"

/**
\class DumpHeapCommandParser

Implements a parser for dumpheap outputs.
*/
class DumpHeapCommandParser
{
private:
	const std::string _command = "!dumpheap -short -type";
	const std::string _command_mt = "!dumpheap -short -mt";
	const std::string _command_stat = "!dumpheap -stat -type";

	IDebuggerCommandExecutor* _executor;

	std::vector<unsigned long>* Parse(const std::string& lines);
	std::vector<unsigned long>* ParseTables(const std::string& clr_exact_type_name, const std::string& lines);

protected:
	ILogger* _logger;

public:
	DumpHeapCommandParser(IDebuggerCommandExecutor* executor, ILogger* logger)
		: _executor(executor), _logger(logger)
	{

	}

	DumpHeapCommandOutput execute(const std::string& clr_partial_type_name);

	DumpHeapCommandOutput execute_by_mt(unsigned long method_table);

	MethodTableOutput find_method_tables(const std::string& clr_exact_type_name);
};

#endif // #ifndef __DUMPHEAPCOMMANDPARSER_H__
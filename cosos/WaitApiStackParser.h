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
\file WaitApiStackParser.h

Defines the WaitApiStackParser class.
*/

#ifndef __WAITAPISTACKPARSER_H__

#define __WAITAPISTACKPARSER_H__

#include "MemoryRange.h"

#include <string>
#include <sstream>
#include <vector>
#include <map>

/**
\class PartialStackFrame

Represents an ChildEBP, return address and first three arguments of a stack frame.
*/
class PartialStackFrame
{
	// 13 12f2e5b8 7071b7ad 12f2e5f8 8e188ec4 12f2e804 clr!MethodDescCallSite::CallTargetWorker+0x152 (FPO: [Non-Fpo])

public:
	bool _is_valid = true;

	unsigned long child_ebp;
	unsigned long ret_address;
	unsigned long arg1;
	unsigned long arg2;
	unsigned long arg3;
	std::string symbol_name;
	unsigned long symbol_offset;

	bool isValid() { return _is_valid; }
};

/**
\class KernelObjectDescriptor

Represents an address that points to an address array whom elements are kernel objects, possibly handles.
*/
class KernelObjectDescriptor
{
private:
	unsigned long _thread_id;
	unsigned long _value;
	unsigned long _count;

public:
	static const unsigned long ADDRESS_IS_IMMEDIATE = -1;
	static const unsigned long VALUE_NOT_FOUND = -1;

	KernelObjectDescriptor(unsigned long value, unsigned long count)
		: _value(value), _count(count), _thread_id(0)
	{

	}

	KernelObjectDescriptor(unsigned long thread_id, unsigned long value, unsigned long count)
		: _value(value), _count(count), _thread_id(thread_id)
	{

	}

	unsigned long get_thread_id(){ return _thread_id; }
	unsigned long get_value(){ return _value; }
	unsigned long get_count(){ return _count; }

	void set_thread_id(unsigned long thread_id){ _thread_id = thread_id; }
	bool is_value_address(){ return _count != ADDRESS_IS_IMMEDIATE; }
};


/**
\class WaitApiStackParser

Implements a parser for stack trace outputs calling kernel wait APIs.
*/
class WaitApiStackParser
{
private:

	static std::map<std::string, std::pair<unsigned long, unsigned long>> _symbol_object;


public:
	KernelObjectDescriptor ParseObjectDescriptor(const PartialStackFrame& stackFrame);
	PartialStackFrame ParseStackFrame(const std::string& line);
	WaitApiStackParser()
	{
	}

	std::vector<const KernelObjectDescriptor>* Parse(const std::string& lines);
};

#endif // #ifndef __WAITAPISTACKPARSER_H__
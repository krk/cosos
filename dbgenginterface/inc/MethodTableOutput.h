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
\file DumpHeapOutput.h

Defines the DumpHeapCommandOutput class.
*/

#ifndef __METHODTABLEOUTPUT_H__

#define __METHODTABLEOUTPUT_H__

#include <vector>

/**
\class MethodTableOutput

Represents method tables parsed from output of the !dumpheap command.
*/
class MethodTableOutput
{
private:
	std::vector<unsigned long> *_method_tables = nullptr;

public:
	MethodTableOutput()
	{

	}

	MethodTableOutput(std::vector<unsigned long> *method_tables)
		: _method_tables(method_tables)
	{

	}

	std::vector<unsigned long>* get_method_tables() const;

	bool has_method_tables() const { return _method_tables != nullptr&& _method_tables->size() > 0; }
};

#endif // #ifndef __METHODTABLEOUTPUT_H__
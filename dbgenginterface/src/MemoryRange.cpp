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
\file MemoryRange.cpp

Implements MemoryRange class that represents a range in a heap in memory.
*/

#include "MemoryRange.h"

/**
Constructs an instance of the MemoryRange class.

\param address Address of the range.
\param size Size of the range.
\param state State of the range.
\param usage Usage of the range.
*/
MemoryRange::MemoryRange(unsigned long address, unsigned long size, ::State state, ::Usage usage)
	: Address(address), Size(size), State(state), Usage(usage)
{
}

/**
Constructs an instance of the MemoryRange class with undefined state and undefined usage.
*/
MemoryRange::MemoryRange()
	: Address(-1), Size(0), State(State::Undefined), Usage(Usage::Undefined)
{
}
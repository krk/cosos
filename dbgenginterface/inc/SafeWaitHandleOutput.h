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
\file SafeWaitHandleOutput.h

Defines the SafeWaitHandleOutput class.
*/

#ifndef __SAFEWAITHANDLEOUTPUT_H__

#define __SAFEWAITHANDLEOUTPUT_H__

#include <map>
#include <memory>

/**
\class SafeWaitHandleOutput

Represents output of the SafeWaitHandleParser.
*/
class SafeWaitHandleOutput
{
private:
	std::shared_ptr<std::map<unsigned long, unsigned long>> _handle_address = nullptr;

public:
	SafeWaitHandleOutput()
	{

	}

	SafeWaitHandleOutput(std::shared_ptr<std::map<unsigned long, unsigned long>> handle_address)
		: _handle_address(handle_address)
	{

	}
	std::shared_ptr<std::map<unsigned long, unsigned long>> get_handle_addresses();

	bool has_handle_addresses() { return _handle_address != nullptr && _handle_address->size() > 0; }
};

#endif // #ifndef __SAFEWAITHANDLEOUTPUT_H__
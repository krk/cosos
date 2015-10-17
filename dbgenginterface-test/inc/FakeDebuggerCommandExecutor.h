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
\file FakeDebuggerCommandExecutor.h

Defines the FakeDebuggerCommandExecutor class.
*/

#ifndef __FAKEDEBUGGERCOMMANDEXECUTOR_H__

#define __FAKEDEBUGGERCOMMANDEXECUTOR_H__

#include <functional>

#include "IDebuggerCommandExecutor.h"

/**
\class FakeDebuggerCommandExecutor

Represents renderable heap information.
*/
class FakeDebuggerCommandExecutor : public IDebuggerCommandExecutor
{
public:
	typedef std::function<bool(const std::string&, std::string&)> OutputLambda;

private:
	OutputLambda _output_lambda;

public:

	FakeDebuggerCommandExecutor(OutputLambda output_lambda)
		: _output_lambda(output_lambda)
	{

	}

	virtual bool ExecuteCommand(const std::string& command, std::string& output) override;
};

#endif // #ifndef __FAKEDEBUGGERCOMMANDEXECUTOR_H__
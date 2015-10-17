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
\file DbgEngCommandExecutor.cpp

Implements DbgEngCommandExecutor class that can execute commands in DbgEng context.
*/

#include "DbgEngCommandExecutor.h"
#include "StdioOutputCallbacks.h"

/**
Constructs an instance of the DbgEngCommandExecutor class.

\param debug_client IDebugClient instance.
\param debug_control IDebugControl instance.
*/
DbgEngCommandExecutor::DbgEngCommandExecutor(PDEBUG_CLIENT debug_client, PDEBUG_CONTROL debug_control)
	: _debug_client(debug_client), _debug_control(debug_control)
{

}

/**
Executes a command in DbgEng scope.

\param command Command text to execute.
\param output Output of the command, if successful.
*/
bool DbgEngCommandExecutor::ExecuteCommand(const std::string& command, std::string& output)
{
	g_OutputCb.Clear();

	if (_debug_control->Execute(DEBUG_OUTCTL_THIS_CLIENT | //Send output to only outputcallbacks
		DEBUG_OUTCTL_OVERRIDE_MASK |
		DEBUG_OUTCTL_NOT_LOGGED,
		command.c_str(),
		DEBUG_EXECUTE_DEFAULT) != S_OK)
	{
		_debug_control->Release();
		_debug_client->Release();

		g_OutputCb.Clear();

		return false;
	}

	output = std::string(g_OutputCb.GetOutputBuffer());

	g_OutputCb.Clear();

	return true;
}

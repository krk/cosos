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
\file GcViewDescriptor.h

Defines the GcViewDescriptor class.
*/

#ifndef __GCVIEWDESCRIPTOR_H__

#define __GCVIEWDESCRIPTOR_H__

#include "MemoryRange.h"

#include <string>
#include <vector>
#include <memory>
#include <qpixmap.h>

/**
\class GcViewDescriptor

Represents renderable heap information.
*/
class GcViewDescriptor
{
private:
	static const int IMAGE_WIDTH = 2048;
	static const int IMAGE_HEIGHT = 512;

	static unsigned char* createImage(RangeList ranges, RangeList gcRanges);
	static unsigned char* createImage(RangeList ranges, bool isMonochrome = false);
	static void drawImage(unsigned char* image, RangeList ranges, bool isMonochrome = false);

	void updateImages();

	static QRgb getColor(State state, Usage usage);

public:
	std::string _freeblockinfo;
	std::string _gcInfo1;
	std::string _gcInfo2;

	RangeList _ranges = nullptr;
	RangeList _gcRanges = nullptr;

	void saveImages(const char* filename, const char* gcFilename);
	static void saveImages(RangeList ranges, RangeList gcRanges, const char* filename, const char* gcFilename);

	const std::pair<unsigned char*, unsigned char*> GcViewDescriptor::getImageBuffers();

	const QPixmap getNullPixmap();

	GcViewDescriptor()
	{
	}
};

#endif // #ifndef __GCVIEWDESCRIPTOR_H__
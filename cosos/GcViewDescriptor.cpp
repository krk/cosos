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
\file GcViewDescriptor.cpp

Implements GcViewDescriptor class that can render a combined view of native and CLR heap snapshots.
*/

#include "GcViewDescriptor.h"

/**
Gets an empty pixmap.
*/
const QPixmap GcViewDescriptor::getNullPixmap()
{
	auto whiteImage = new QImage(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format::Format_ARGB32);

	whiteImage->fill(Qt::GlobalColor::white);

	auto pixmap = QPixmap::fromImage(*whiteImage);

	return pixmap;
}

/**
Saves png images for native and gc heap ranges.

\param ranges Native ranges.
\param gcRanges CLR GC ranges.
\param filename Native png filename.
\param gcFilename CLR GC png filename.
*/
void GcViewDescriptor::saveImages(RangeList ranges, RangeList gcRanges, const char* filename, const char* gcFilename)
{
	GcViewDescriptor descriptor;

	descriptor._ranges = ranges;
	descriptor._gcRanges = gcRanges;

	descriptor.saveImages(filename, gcFilename);
}

/**
Saves png images for native and gc heap ranges.

\param filename Native png filename.
\param gcFilename CLR GC png filename.
*/
void GcViewDescriptor::saveImages(const char* filename, const char* gcFilename)
{
	auto buffers = getImageBuffers();

	if (filename)
	{
		if (buffers.first == nullptr)
		{
			getNullPixmap().save(filename);
		}
		else
		{
			auto image = QImage(buffers.first, 2048, 512, QImage::Format::Format_RGB32);

			image.save(filename, "png");

			delete buffers.first;
		}
	}

	if (gcFilename)
	{
		if (buffers.second == nullptr)
		{
			getNullPixmap().save(gcFilename);
		}
		else
		{
			auto image = QImage(buffers.second, 2048, 512, QImage::Format::Format_RGB32);

			image.save(gcFilename, "png");

			delete buffers.second;
		}
	}
}

/**
Creates and returns image buffers.
*/
const std::pair<unsigned char*, unsigned char*> GcViewDescriptor::getImageBuffers()
{
	unsigned char* image = nullptr;
	unsigned char* gcImage = nullptr;

	if (_ranges == nullptr && _gcRanges == nullptr)
	{
		return std::make_pair(nullptr, nullptr);
	}
	else if (_ranges == nullptr && _gcRanges != nullptr)
	{
		image = createImage(_gcRanges);

		return std::make_pair(nullptr, image);
	}
	else if (_ranges != nullptr && _gcRanges == nullptr)
	{
		image = createImage(_ranges);

		return std::make_pair(image, nullptr);
	}
	else
	{
		image = createImage(_ranges);

		gcImage = createImage(_ranges, _gcRanges);

		return std::make_pair(image, gcImage);
	}
}

/**
Creates image buffer for native and gc heap ranges.

\param ranges Native ranges.
\param gcRanges CLR GC ranges.
*/
unsigned char* GcViewDescriptor::createImage(RangeList ranges, RangeList gcRanges)
{
	auto buffer = createImage(ranges, true);

	drawImage(buffer, gcRanges);

	return buffer;
}

/**
Creates image buffer for native and gc heap ranges.

\param ranges Native ranges.
\param isMonochrome True if to be drawn as monochrome.
*/
unsigned char* GcViewDescriptor::createImage(RangeList ranges, bool isMonochrome)
{
	auto buffer = new unsigned char[4 * IMAGE_WIDTH * IMAGE_HEIGHT];

	memset(buffer, 0x80, 4 * IMAGE_WIDTH * IMAGE_HEIGHT);

	drawImage(buffer, ranges, isMonochrome);

	return buffer;
}

/**
Draws native and gc heap ranges to an image buffer.

\param buffer Image buffer.
\param ranges Native ranges.
\param isMonochrome True if to be drawn as monochrome.
*/
void GcViewDescriptor::drawImage(unsigned char* buffer, RangeList ranges, bool isMonochrome)
{
	const unsigned int PAGE_SIZE = 4096;

	if (ranges == nullptr || ranges->size() == 0)
	{
		return;
	}

	for (auto mr : *ranges)
	{
		unsigned int x = mr.Address / PAGE_SIZE / IMAGE_HEIGHT;
		unsigned int y = (mr.Address / PAGE_SIZE) % IMAGE_HEIGHT;

		QRgb c;

		if (isMonochrome && mr.Usage != Usage::Free)
		{
			c = getColor(mr.State, Usage::Undefined);
		}
		else
		{
			c = getColor(mr.State, mr.Usage);
		}

		if (mr.Size > PAGE_SIZE)
		{
			unsigned int numPages = mr.Size / PAGE_SIZE;

			for (int pos = 0; pos < numPages; pos++)
			{
				if (y == IMAGE_HEIGHT)
				{
					y = 0;
					x++;
				}

				// TODO check input.
				if (x > IMAGE_WIDTH)
				{
					continue;
				}

				if (y > IMAGE_HEIGHT)
				{
					continue;
				}

				buffer[4 * (y * IMAGE_WIDTH + x)] = qRed(c);
				buffer[4 * (y * IMAGE_WIDTH + x) + 1] = qGreen(c);
				buffer[4 * (y * IMAGE_WIDTH + x) + 2] = qBlue(c);

				y++;
			}
		}
	}
}

/**
Gets the color for the specified state and usage.

\param state State.
\param usage Usage.
*/
QRgb GcViewDescriptor::getColor(State state, Usage usage)
{
	if (state == State::Undefined)
		return QRgb(0x808080);
	if (usage == Usage::Free)
		return QRgb(0xFFFFFF);

	if (usage == Usage::EnvironmentBlock || usage == Usage::PEB || usage == Usage::ProcessParameters || usage == Usage::TEB || usage == Usage::Stack)
	{
		if (state == State::Commit)
			return QRgb(0x800080);
		else
			return QRgb(0xFFC0CB);
	}

	if (usage == Usage::Heap)
	{
		if (state == State::Commit)
			return QRgb(0x0000FF);
		else
			return QRgb(0xADD8E6);
	}

	if (usage == Usage::Image)
	{
		if (state == State::Commit)
			return QRgb(0x8B0000);
		else
			return QRgb(0XFF0000);
	}

	if (usage == Usage::VirtualAlloc)
	{
		if (state == State::Commit)
			return QRgb(0x008000);
		else
			return QRgb(0xADFF2F);
	}

	if (usage == Usage::PageHeap)
	{
		if (state == State::Commit)
			return QRgb(0x000000);
		else
			return QRgb(0x2F4F4F);
	}

	if (usage == Usage::GCHeap)
	{
		if (state == State::Commit)
			return QRgb(0x00FF7F);
		else
			return QRgb(0xADFF2F);
	}

	if (usage == Usage::GCLOHeap)
	{
		if (state == State::Commit)
			return QRgb(0x006400);
		else
			return QRgb(0x32CD32);
	}

	return QRgb(0x808080);
}

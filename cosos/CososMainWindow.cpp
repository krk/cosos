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
\file CososMainWindow.cpp

Implements CososMainWindow class that contains the gcview UI.
*/

#include "CososMainWindow.h"

/**
Constructs an instance of the CososMainWindow class.

\param parent Parent of the window.
*/
CososMainWindow::CososMainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	setImage(ui.qwBlocks, GcViewDescriptor.getNullPixmap());
	setImage(ui.qwHeapBlocks, GcViewDescriptor.getNullPixmap());
}

/**
Destructs an instance of the CososMainWindow class.
*/
CososMainWindow::~CososMainWindow()
{
	if (_lastBuffer1)
	{
		delete _lastBuffer1;
	}

	if (_lastBuffer2)
	{
		delete _lastBuffer2;
	}
}

/**
Updates textual information on the UI.
*/
void CososMainWindow::updateInfos()
{
	ui.label_freeblockinfo->setText(QString(GcViewDescriptor._freeblockinfo.c_str()));
	ui.label_gcinfo_1->setText(QString(GcViewDescriptor._gcInfo1.c_str()));
	ui.label_gcinfo_2->setText(QString(GcViewDescriptor._gcInfo2.c_str()));
}

/**
Updates memory map images on the UI.
*/
void CososMainWindow::updateImages()
{
	// TODO refactor lastBuffer variables.

	if (_lastBuffer1)
	{
		delete _lastBuffer1;
	}

	if (_lastBuffer2)
	{
		delete _lastBuffer2;
	}

	auto buffers = GcViewDescriptor.getImageBuffers();

	if (buffers.first)
	{
		auto image = QImage(buffers.first, 2048, 512, QImage::Format::Format_RGB32);

		auto pixmap = QPixmap::fromImage(image);

		setImage(ui.qwBlocks, pixmap);
	}
	else
	{
		setImage(ui.qwBlocks, GcViewDescriptor.getNullPixmap());
	}

	_lastBuffer1 = buffers.first;

	if (buffers.second)
	{
		auto image = QImage(buffers.second, 2048, 512, QImage::Format::Format_RGB32);

		auto pixmap = QPixmap::fromImage(image);

		setImage(ui.qwHeapBlocks, pixmap);
	}
	else
	{
		setImage(ui.qwHeapBlocks, GcViewDescriptor.getNullPixmap());
	}

	_lastBuffer2 = buffers.second;
}

/**
Sets a pixmap as the background of a label.

\param label The label.
\param pixmap The pixmap.
*/
void CososMainWindow::setImage(QLabel* label, const QPixmap& pixmap)
{
	label->setPixmap(pixmap);
}
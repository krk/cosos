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
\file CososMainWindow.h

Defines the CososMainWindow class.
*/

#ifndef COSOSMAINWINDOW_H
#define COSOSMAINWINDOW_H

#include <qstring.h>
#include <QtWidgets/QMainWindow>
#include <qevent.h>
#include "ui_CososMainWindow.h"
#include "memoryrange.h"
#include "gcviewdescriptor.h"

/**
\class CososMainWindow

Main window of the gcview extension.
*/
class CososMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	CososMainWindow(QWidget *parent = 0);
	~CososMainWindow();

	GcViewDescriptor GcViewDescriptor;

public slots:
	void updateImages();
	void updateInfos();

private:
	Ui::CososMainWindowClass ui;

	void* _lastBuffer1 = nullptr;
	void* _lastBuffer2 = nullptr;

	void setImage(QLabel* label, const QPixmap& pixmap);
	void setImage(QLabel* label, const QImage* image);
	void setImage(QLabel* label, const unsigned char* image);
};

#endif // COSOSMAINWINDOW_H

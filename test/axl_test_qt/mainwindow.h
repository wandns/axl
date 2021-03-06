//..............................................................................
//
//  This file is part of the AXL library.
//
//  AXL is distributed under the MIT license.
//  For details see accompanying license.txt file,
//  the public copy of which is also available at:
//  http://tibbo.com/downloads/archive/axl/license.txt
//
//..............................................................................

#pragma once

#define MAINWINDOW_H

//..............................................................................

class MyWidget: public QWidget
{
public:
	MyWidget(QWidget* parent = NULL);

	virtual
	void
	paintEvent(QPaintEvent* e);
};

//..............................................................................

class MainWindow: public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = NULL);

	virtual QSize sizeHint() const
	{
		return QSize(640, 480);
	}

private:
#if (1)
	MyWidget m_myWidget;
#else
	QTextEdit m_editWidget;
#endif
};

//..............................................................................

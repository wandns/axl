// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_GUI_QTIMAGE_H

#include "axl_gui_Image.h"

namespace axl {
namespace gui {

//.............................................................................

class QtImage: public Image
{
	friend class QtEngine;

public:
	QPixmap m_qtPixmap;

public:
	QtImage ();

	virtual
	bool
	getData (
		void* data,
		int left,
		int top,
		int right,
		int bottom
		);
};

//.............................................................................

} // namespace gui
} // namespace axl


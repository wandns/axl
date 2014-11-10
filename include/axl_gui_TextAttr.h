// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_GUI_TEXTATTR_H

#include "axl_gui_ColorAttr.h"
#include "axl_gui_FontDesc.h"

namespace axl {
namespace gui {

//.............................................................................

struct TextAttr: public ColorAttr
{
	uint_t m_fontFlags;

	TextAttr ()
	{
		m_fontFlags = 0;
	}

	TextAttr (
		uint_t foreColor,
		uint_t backColor = StdColor_Undefined,
		uint_t fontFlags = 0
		)
	{
		setup (foreColor, backColor, fontFlags);
	}

	TextAttr (
		const ColorAttr& colorAttr,
		uint_t fontFlags = 0
		)
	{
		setup (colorAttr, fontFlags);
	}

	TextAttr (const char* string)
	{
		parse (string);
	}

	int 
	cmp (const TextAttr& attr)
	{
		return memcmp (this, &attr, sizeof (TextAttr));
	}

	void
	clear ()
	{
		ColorAttr::clear ();
		m_fontFlags = 0;
	}

	void
	setup (
		uint_t foreColor,
		uint_t backColor,
		uint_t fontFlags
		)
	{
		m_foreColor = foreColor;
		m_backColor = backColor;
		m_fontFlags = fontFlags;
	}

	void
	setup (
		const ColorAttr& colorAttr,
		uint_t fontFlags
		)
	{

		*(ColorAttr*) this = colorAttr;
		m_fontFlags = fontFlags;
	}

	void
	overlay (
		const TextAttr& baseAttr,
		const TextAttr& overlayAttr
		)
	{
		ColorAttr::overlay (baseAttr, overlayAttr);	
		m_fontFlags = overlayFontFlags (baseAttr.m_fontFlags, overlayAttr.m_fontFlags);
	}

	void
	overlay (const TextAttr& overlayAttr)
	{
		overlay (*this, overlayAttr);
	}

	void
	parse (
		const char* string,
		const char** end = NULL
		);

	void
	parseOverlay (
		const TextAttr& baseAttr,
		const char* string,
		const char** end = NULL
		);

	void
	parseOverlay (
		const char* string,
		const char** end = NULL
		)
	{
		parseOverlay (*this, string, end);
	}
};

//.............................................................................

} // namespace gui
} // namespace axl


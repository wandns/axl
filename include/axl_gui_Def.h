// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_GUI_DEF_H

#include "axl_rtl_BitIdx.h"

namespace axl {
namespace gui {

class Engine;

//.............................................................................

enum MouseButtonKind
{
	MouseButtonKind_None   = 0x00,
	MouseButtonKind_Left   = 0x01,
	MouseButtonKind_Right  = 0x02,
	MouseButtonKind_Medium = 0x04,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
MouseButtonKind
getFirstMouseButton (uint_t buttons)
{
	return (MouseButtonKind) rtl::getLoBitIdx32 (buttons);
}

//.............................................................................

enum ModifierKeyKind
{
	ModifierKeyKind_None   = 0x00,
	ModifierKeyKind_Shift  = 0x01,
	ModifierKeyKind_Ctrl   = 0x02,
	ModifierKeyKind_Alt    = 0x04,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
ModifierKeyKind
getFirstModifierKey (uint_t modifierKeys)
{
	return (ModifierKeyKind) rtl::getLoBitIdx32 (modifierKeys);
}

//.............................................................................

enum KeyKind
{
	KeyKind_Backspace   = 0x08,
	KeyKind_Tab         = 0x09,
	KeyKind_Enter       = 0x0d,	
	KeyKind_Esc         = 0x1b,

	KeyKind_Pause       = 0x10000013,	
	KeyKind_Print       = 0x1000002a,	

	KeyKind_CapsLock    = 0x10000014,
	KeyKind_NumLock     = 0x10000090,
	KeyKind_ScrollLock  = 0x10000091,

	KeyKind_Shift       = 0x10000010,
	KeyKind_Ctrl        = 0x10000011,
	KeyKind_Alt         = 0x10000012,

	KeyKind_PageUp      = 0x10000021,
	KeyKind_PageDown    = 0x10000022,
	KeyKind_End         = 0x10000023,
	KeyKind_Home        = 0x10000024,

	KeyKind_Left        = 0x10000025,
	KeyKind_Up          = 0x10000026,
	KeyKind_Right       = 0x10000027,
	KeyKind_Down        = 0x10000028,

	KeyKind_Insert      = 0x1000002d,
	KeyKind_Delete      = 0x1000002e,

	KeyKind_F1          = 0x10000070,
	KeyKind_F2          = 0x10000071,
	KeyKind_F3          = 0x10000072,
	KeyKind_F4          = 0x10000073,
	KeyKind_F5          = 0x10000074,
	KeyKind_F6          = 0x10000075,
	KeyKind_F7          = 0x10000076,
	KeyKind_F8          = 0x10000077,
	KeyKind_F9          = 0x10000078,
	KeyKind_F10         = 0x10000079,
	KeyKind_F11         = 0x1000007a,
	KeyKind_F12         = 0x1000007b,
};

//.............................................................................

struct Point
{
	union
	{
		struct
		{
			int m_x;
			int m_y;
		};

		struct
		{
			uint_t m_x_u;
			uint_t m_y_u;
		};
	};

	Point ()
	{
		m_x = 0;
		m_y = 0;
	}

	Point (
		int x,
		int y
		)
	{
		m_x = x;
		m_y = y;
	}

	Point 
	operator + (const Point& point2)
	{
		return Point (m_x + point2.m_x, m_y + point2.m_y);
	}

	Point 
	operator - (const Point& point2)
	{
		return Point (m_x - point2.m_x, m_y - point2.m_y);
	}

	void
	setup (
		int x,
		int y
		)
	{
		m_x = x;
		m_y = y;
	}

	void
	clipNegative ()
	{
		if (m_x < 0)
			m_x = 0;

		if (m_x < 0)
			m_x = 0;
	}
};

//.............................................................................

struct Size
{
	union
	{
		struct 
		{
			int m_width;
			int m_height;
		};

		struct 
		{
			uint_t m_width_u;
			uint_t m_height_u;
		};
	};

	Size ()
	{
		m_width = 0;
		m_height = 0;
	}

	Size (
		int width,
		int height
		)
	{
		m_width = width;
		m_height = height;
	}

	Size 
	operator + (const Size& size2)
	{
		return Size (m_width + size2.m_width, m_height + size2.m_height);
	}

	Size 
	operator - (const Size& size2)
	{
		return Size (m_width - size2.m_width, m_height - size2.m_height);
	}

	void
	setup (
		int width,
		int height
		)
	{
		m_width = width;
		m_height = height;
	}

	void
	clipNegative ()
	{
		if (m_width < 0)
			m_width = 0;

		if (m_height < 0)
			m_height = 0;
	}
};

//.............................................................................

enum FormFactorKind
{
	FormFactorKind_Landscape,
	FormFactorKind_Portrait,
	FormFactorKind_LineStrip,
	FormFactorKind_ColumnStrip,
	FormFactorKind__Count,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
FormFactorKind
getFormFactor (
	int width,
	int height, 
	int stripThreshold = 32
	)
{
	return 
		height > width * stripThreshold ? FormFactorKind_LineStrip :
		width > height * stripThreshold ? FormFactorKind_ColumnStrip :
		height > width ? FormFactorKind_Portrait : FormFactorKind_Landscape;
}

inline
FormFactorKind
getFormFactor (
	const Size& size, 
	int stripThreshold = 32
	)
{
	return getFormFactor (size.m_width, size.m_height, stripThreshold);
}

//.............................................................................

struct Rect
{
	union 
	{
		struct 
		{
			int m_left;
			int m_top;
			int m_right;
			int m_bottom;
		};

		struct 
		{
			uint_t m_left_u;
			uint_t m_top_u;
			uint_t m_right_u;
			uint_t m_bottom_u;
		};
	};

	Rect ()
	{
		m_left = 0;
		m_top = 0;
		m_right = 0;
		m_bottom = 0;
	}

	Rect (
		int left,
		int top,
		int right,
		int bottom
		)
	{
		m_left = left;
		m_top = top;
		m_right = right;
		m_bottom = bottom;
	}

	Rect (
		int left,
		int top
		)
	{
		m_left = left;
		m_top = top;
		m_right = left;
		m_bottom = top;
	}

	Rect (
		const Point& leftTop,
		const Point& rightBottom
		)
	{
		m_left = leftTop.m_x;
		m_top = leftTop.m_y;
		m_right = rightBottom.m_x;
		m_bottom = rightBottom.m_y;
	}

	Rect (const Point& leftTop)
	{
		m_left = leftTop.m_x;
		m_top = leftTop.m_y;
		m_right = leftTop.m_x;
		m_bottom = leftTop.m_y;
	}

	Rect (
		const Point& leftTop,
		const Size& size
		)
	{
		m_left = leftTop.m_x;
		m_top = leftTop.m_y;
		m_right = leftTop.m_x + size.m_width;
		m_bottom = leftTop.m_y + size.m_height;
	}

	bool
	isEmpty () const
	{
		return m_right == m_left || m_bottom == m_top;
	}

	int
	getWidth () const
	{
		return m_right - m_left;
	}

	int
	getHeight () const
	{
		return m_bottom - m_top;
	}

	Size
	getSize () const
	{
		return Size (getWidth (), getHeight ());
	}

	void
	setup (
		int left,
		int top,
		int right,
		int bottom
		)
	{
		m_left = left;
		m_top = top;
		m_right = right;
		m_bottom = bottom;
	}

	void
	clipNegative ()
	{
		if (m_left < 0)
			m_left = 0;

		if (m_top < 0)
			m_top = 0;

		if (m_right < 0)
			m_right = 0;

		if (m_bottom < 0)
			m_bottom = 0;
	}
};

//.............................................................................

struct CursorPos
{
	union
	{
		struct
		{
			int m_line;
			int m_col;
		};

		struct
		{
			uint_t m_line_u;
			uint_t m_col_u;
		};
	};

	CursorPos ()
	{
		m_line = 0;
		m_col = 0;
	}

	CursorPos (
		int line,
		int col
		)
	{
		setup (line, col);
	}

	bool
	operator == (const CursorPos& pos2) const
	{
		return cmp (pos2) == 0;
	}

	bool
	operator != (const CursorPos& pos2) const
	{
		return cmp (pos2) != 0;
	}

	bool
	operator < (const CursorPos& pos2) const
	{
		return cmp (pos2) < 0;
	}

	bool
	operator <= (const CursorPos& pos2) const
	{
		return cmp (pos2) <= 0;
	}

	bool
	operator > (const CursorPos& pos2) const
	{
		return cmp (pos2) > 0;
	}

	bool
	operator >= (const CursorPos& pos2) const
	{
		return cmp (pos2) >= 0;
	}

	int
	cmp (const CursorPos& pos2) const
	{
		return 
			m_line < pos2.m_line ? -1 : 
			m_line > pos2.m_line ?  1 : 
			m_col < pos2.m_col ? -1 : 
			m_col > pos2.m_col ?  1 : 0;
	}

	int
	cmp_u (const CursorPos& pos2) const
	{
		return 
			m_line_u < pos2.m_line_u ? -1 : 
			m_line_u > pos2.m_line_u ?  1 : 
			m_col_u < pos2.m_col_u ? -1 : 
			m_col_u > pos2.m_col_u ?  1 : 0;
	}

	void
	setup (
		int line,
		int col
		)
	{
		m_line = line;
		m_col = col;
	}

	void
	clipNegative ()
	{
		if (m_line < 0)
			m_line = 0;

		if (m_col < 0)
			m_col = 0;
	}
};

//.............................................................................

class GuiItem
{
protected:
	Engine* m_engine;

public:
	GuiItem ()
	{
		m_engine = NULL;
	}

	Engine* 
	getEngine ()
	{
		return m_engine;
	}
};

//.............................................................................

} // namespace gui
} // namespace axl


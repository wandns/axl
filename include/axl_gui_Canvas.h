// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2013. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_GUI_CANVAS_H

#include "axl_gui_Def.h"
#include "axl_gui_Color.h"
#include "axl_gui_Font.h"
#include "axl_gui_Image.h"
#include "axl_gui_ImageList.h"

namespace axl {
namespace gui {
	
//.............................................................................

struct ICanvas: IGuiItem
{
	// {267AE64D-B609-41F9-8B40-1D883A75CB5A}
	AXL_OBJ_INTERFACE (
		ICanvas,
		0x267ae64d, 0xb609, 0x41f9, 0x8b, 0x40, 0x1d, 0x88, 0x3a, 0x75, 0xcb, 0x5a
		)

protected:
	IFont* m_pFont;
	TColorAttr m_ColorAttr;

public:
	IFont* m_pBaseFont;
	TTextAttr m_BaseTextAttr;
	TTextAttr m_DefTextAttr;
	TPalette m_Palette;

public:
	ICanvas ()
	{
		m_pFont = NULL;
		m_pBaseFont = NULL;
		m_BaseTextAttr.Setup (EStdPalColor_WidgetText, EStdPalColor_WidgetBack, 0);
	}

	IFont*
	GetFont ()
	{
		return m_pFont;
	}

	TColorAttr
	GetColorAttr ()
	{
		return m_ColorAttr;
	}

	virtual
	bool
	DrawRect (
		int Left,
		int Top,
		int Right,
		int Bottom,
		uint_t Color
		) = 0;

	bool
	DrawRect (
		int Left,
		int Top,
		int Right,
		int Bottom
		)
	{
		return DrawRect (Left, Top, Right, Bottom, m_DefTextAttr.m_BackColor);
	}

	bool
	DrawRect (
		const TRect& Rect,
		uint_t Color
		)
	{
		return DrawRect (
			Rect.m_Left, 
			Rect.m_Top, 
			Rect.m_Right, 
			Rect.m_Bottom, 
			Color
			);
	}

	bool
	DrawRect (const TRect& Rect)
	{
		return DrawRect (
			Rect.m_Left, 
			Rect.m_Top, 
			Rect.m_Right, 
			Rect.m_Bottom, 
			m_DefTextAttr.m_BackColor
			);
	}

	virtual
	bool
	DrawText (
		int x,
		int y,
		int Left,
		int Top,
		int Right,
		int Bottom,
		uint_t TextColor,
		uint_t BackColor,
		uint_t FontFlags,
		const char* pText,
		size_t Length = -1
		) = 0;

	bool
	DrawText (
		int x,
		int y,
		int Left,
		int Top,
		int Right,
		int Bottom,
		const char* pText,
		size_t Length = -1
		)
	{
		return DrawText (
			x,
			y,
			Left,
			Top,
			Right,
			Bottom,
			m_DefTextAttr.m_ForeColor,
			m_DefTextAttr.m_BackColor,
			m_DefTextAttr.m_FontFlags,
			pText,
			Length
			);
	}

	bool
	DrawText (
		const TPoint& Point,
		const TRect& Rect,
		const TTextAttr& TextAttr,
		const char* pText,
		size_t Length = -1
		)
	{
		return DrawText (
			Point.m_x,
			Point.m_y,
			Rect.m_Left, 
			Rect.m_Top, 
			Rect.m_Right, 
			Rect.m_Bottom, 
			TextAttr.m_ForeColor,
			TextAttr.m_BackColor,
			TextAttr.m_FontFlags,
			pText, 
			Length
			);
	}

	bool
	DrawText (
		const TPoint& Point,
		const TRect& Rect,
		const char* pText,
		size_t Length = -1
		)
	{
		return DrawText (
			Point.m_x,
			Point.m_y,
			Rect.m_Left, 
			Rect.m_Top, 
			Rect.m_Right, 
			Rect.m_Bottom, 
			m_DefTextAttr.m_ForeColor,
			m_DefTextAttr.m_BackColor,
			m_DefTextAttr.m_FontFlags,
			pText, 
			Length
			);
	}

	virtual
	bool
	DrawImage (
		int x,
		int y,
		IImage* pImage,
		int Left,
		int Top,
		int Right,
		int Bottom
		) = 0;

	bool
	DrawImage (
		const TPoint& Point,
		IImage* pImage,
		const TRect& Rect = TRect ()
		)
	{
		return DrawImage (
			Point.m_x, 
			Point.m_y, 
			pImage, 
			Rect.m_Left, 
			Rect.m_Top, 
			Rect.m_Right, 
			Rect.m_Bottom
			);
	}

	virtual
	bool
	DrawImage (
		int x,
		int y,
		IImageList* pImageList,
		size_t Index
		) = 0;

	bool
	DrawImage (
		const TPoint& Point,
		IImageList* pImageList,
		size_t Index
		)
	{
		return DrawImage (
			Point.m_x, 
			Point.m_y, 
			pImageList, 
			Index
			);
	}

	virtual
	bool
	CopyRect (
		ICanvas* pSrcCanvas,
		int xDst,
		int yDst,
		int xSrc,
		int ySrc,
		int Width,
		int Height
		) = 0;

	bool
	CopyRect (
		ICanvas* pSrcCanvas,
		const TPoint& DstPoint,
		const TPoint& SrcPoint,
		const TSize& Size
		)
	{
		return CopyRect (
			pSrcCanvas, 
			DstPoint.m_x,
			DstPoint.m_y,
			SrcPoint.m_x,
			SrcPoint.m_y,
			Size.m_Width, 
			Size.m_Height
			);
	}
};

//.............................................................................

} // namespace gui
} // namespace axl
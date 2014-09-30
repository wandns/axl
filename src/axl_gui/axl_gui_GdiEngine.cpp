#include "pch.h"
#include "axl_gui_GdiEngine.h"
#include "axl_ref_Factory.h"
#include "axl_err_Error.h"
#include "axl_gui_Widget.h"

namespace axl {
namespace gui {

//.............................................................................

GdiEngine::~GdiEngine ()
{
	if (m_hWndClipboardOwner)
		::DestroyWindow (m_hWndClipboardOwner);
}

ref::Ptr <Font>
GdiEngine::createStdFont (StdFontKind fontKind)
{
	LOGFONTW logFont;
	HFONT hFont;

	switch (fontKind)
	{
	case StdFontKind_Gui:
		return createStockFont (DEFAULT_GUI_FONT);

	case StdFontKind_Monospace:
		buildLogFont (&logFont, L"Courier New", 10);
		logFont.lfPitchAndFamily = FIXED_PITCH;

		hFont = ::CreateFontIndirectW (&logFont);
		ASSERT (hFont);
		
		return createFont (hFont);

	default:
		return ref::PtrKind_Null;
	}
}

ref::Ptr <Font>
GdiEngine::createFont (
	const char* faceName,
	size_t pointSize,
	uint_t flags
	)
{
	LOGFONTW logFont;
	buildLogFont (&logFont, rtl::String_w (faceName), pointSize, flags);

	HFONT hFont = ::CreateFontIndirect (&logFont);
	if (!hFont)
		return err::failWithLastSystemError (ref::PtrKind_Null);

	return createFont (hFont);
}

ref::Ptr <Font>
GdiEngine::createStockFont (int stockFontKind)
{
	HGDIOBJ h = ::GetStockObject (stockFontKind);
	dword_t gdiObjectType = ::GetObjectType (h);
	if (gdiObjectType != OBJ_FONT)
	{
		err::setError (err::StatusKind_InvalidHandle);
		return ref::PtrKind_Null;
	}

	return createFont ((HFONT) h);
}

ref::Ptr <Font>
GdiEngine::createFont (HFONT hFont)
{
	GdiFont* font = AXL_MEM_NEW (GdiFont);

	LOGFONTW logFont;
	::GetObjectW (hFont, sizeof (logFont), &logFont);
	getFontDescFromLogFont (&logFont, &font->m_fontDesc);

	ref::Ptr <GdiFontuple> fontTuple = AXL_REF_NEW (GdiFontuple);
	fontTuple->m_baseFont = font;
	fontTuple->m_fontModArray [font->m_fontDesc.m_flags] = font;

	font->m_h = hFont;
	font->m_tuple = fontTuple;

	return ref::Ptr <Font> (font, fontTuple);
}

Font*
GdiEngine::getFontMod (
	Font* _pBaseFont,
	uint_t flags
	)
{
	ASSERT (_pBaseFont->getEngine () == this);

	GdiFont* baseFont = (GdiFont*) _pBaseFont;
	GdiFontuple* fontTuple = (GdiFontuple*) baseFont->m_tuple;

	LOGFONTW logFont;
	getLogFontFromFontDesc (*fontTuple->m_baseFont->getFontDesc (), &logFont);
	modifyLogFont (&logFont, flags);

	HFONT hFont = ::CreateFontIndirect (&logFont);
	if (!hFont)
		return err::failWithLastSystemError ((Font*) NULL);

	GdiFont* font = AXL_MEM_NEW (GdiFont);
	font->m_fontDesc = baseFont->m_fontDesc;
	font->m_fontDesc.m_flags = flags;
	font->m_h = hFont;

	ASSERT (!(flags & FontFlagKind_Transparent) && flags < countof (fontTuple->m_fontModArray));
	ASSERT (!fontTuple->m_fontModArray [flags]);

	fontTuple->m_fontModArray [flags] = font;
	return font;
}

ref::Ptr <Cursor>
GdiEngine::createStockCursor (LPCTSTR stockCursorRes)
{
	HCURSOR h = ::LoadCursor (NULL, stockCursorRes);
	if (!h)
		return err::failWithLastSystemError (ref::PtrKind_Null);

	ref::Ptr <GdiCursor> cursor = AXL_REF_NEW (ref::Box <GdiCursor>);
	cursor->m_h = h;
	return cursor;
}

ref::Ptr <Cursor>
GdiEngine::createStdCursor (StdCursorKind cursorKind)
{
	static LPCTSTR stockCursorResTable [StdCursorKind__Count] =
	{
		IDC_ARROW,    // EStdCursor_Arrow = 0,
		IDC_WAIT,     // EStdCursor_Wait,
		IDC_IBEAM,    // EStdCursor_IBeam,
		IDC_HAND,     // EStdCursor_Hyperlink,
		IDC_SIZENS,   // EStdCursor_SizeNS,
		IDC_SIZEWE,   // EStdCursor_SizeWE,
		IDC_SIZENWSE, // EStdCursor_SizeNWSE,
		IDC_SIZENESW, // EStdCursor_SizeNESW,
		IDC_SIZEALL,  // EStdCursor_SizeAll,
	};

	ASSERT (cursorKind < StdCursorKind__Count);
	return createStockCursor (stockCursorResTable [cursorKind]);
}

ref::Ptr <Image>
GdiEngine::createImage ()
{
	ref::Ptr <GdiImage> image = AXL_REF_NEW (ref::Box <GdiImage>);
	return image;
}

ref::Ptr <Image>
GdiEngine::createImage (
	int width,
	int height,
	PixelFormatKind pixelFormat,
	const void* data,
	bool isScreenCompatible
	)
{
	uint_t bitCount;

	switch (pixelFormat)
	{
	case PixelFormatKind_Rgba:
		bitCount = 32;
		break;

	case PixelFormatKind_Rgb:
		bitCount = 24;
		break;

	default:
		err::setFormatStringError ("unsupported pixel format '%s'", getPixelFormatString (pixelFormat));
	};

	HBITMAP hBitmap;

	if (!isScreenCompatible)
	{
		hBitmap = ::CreateBitmap (
			width,
			height,
			1,
			bitCount,
			data
			);

		if (!hBitmap)
			return err::failWithLastSystemError (ref::PtrKind_Null);
	}
	else
	{
		BITMAPINFO bitmapInfo = { 0 };
		bitmapInfo.bmiHeader.biSize = sizeof (bitmapInfo.bmiHeader);
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = bitCount;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;
		bitmapInfo.bmiHeader.biWidth = width;
		bitmapInfo.bmiHeader.biHeight = height;

		ScreenDc screenDc;

		hBitmap = ::CreateCompatibleBitmap (
			screenDc,
			width,
			height
			);

		if (!hBitmap)
			return err::failWithLastSystemError (ref::PtrKind_Null);

		bool_t result = ::SetDIBits (
			screenDc,
			hBitmap,
			0,
			height,
			data,
			&bitmapInfo,
			DIB_RGB_COLORS
			);

		if (!result)
			return err::failWithLastSystemError (ref::PtrKind_Null);
	}

	ref::Ptr <GdiImage> image = AXL_REF_NEW (ref::Box <GdiImage>);
	image->m_h = hBitmap;
	return image;
}

ref::Ptr <Canvas>
GdiEngine::createOffscreenCanvas (
	int width,
	int height
	)
{
	ScreenDc screenDc;
	HBITMAP hBitmap = ::CreateCompatibleBitmap (screenDc, width, height);
	if (!hBitmap)
		return err::failWithLastSystemError (ref::PtrKind_Null);

	HDC hdc = ::CreateCompatibleDC (screenDc);

	ref::Ptr <GdiCanvas> dc = AXL_REF_NEW (ref::Box <GdiCanvas>);
	dc->attach (hdc, NULL, GdiCanvas::DestructKind_DeleteDc);
	dc->m_hBitmap = hBitmap;
	dc->m_hPrevBitmap = (HBITMAP) ::SelectObject (hdc, hBitmap);

	return dc;
}

uintptr_t 
GdiEngine::registerClipboardFormat (const rtl::String& formatName)
{
	err::setError (err::StatusKind_NotImplemented);
	return -1;
}

bool
GdiEngine::readClipboard (rtl::String* string)
{
	bool result = openClipboard ();
	if (!result)
		return false;

	HANDLE hData = ::GetClipboardData (CF_TEXT);
	if (!hData)
	{
		::CloseClipboard();

		err::setError (err::StatusKind_InvalidDeviceRequest);
		return false;
	}

	string->clear ();

	size_t size = ::GlobalSize (hData);
	if (!size)
	{
		::CloseClipboard();
		return true;
	}

	void* data = ::GlobalLock (hData);
	ASSERT (data);

	string->copy ((char*) data);

	::GlobalUnlock(data);

	::CloseClipboard ();
	return true;
}

bool
GdiEngine::readClipboard (
	uintptr_t format,
	rtl::Array <char>* data
	)
{
	err::setError (err::StatusKind_NotImplemented);
	return false;
}

bool
GdiEngine::writeClipboard (
	const char* string,
	size_t length
	)
{
	bool result = openClipboard ();
	if (!result)
		return false;

	HGLOBAL hData = ::GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, length + 1);
	void* p = ::GlobalLock (hData);
	memcpy (p, string, length);
	((char*) p) [length] = 0;
	::GlobalUnlock (hData);

	::SetClipboardData (CF_TEXT, hData);
	::CloseClipboard ();
	return true;
}

bool
GdiEngine::writeClipboard (
	uintptr_t format,
	const void* data,
	size_t size
	)
{
	err::setError (err::StatusKind_NotImplemented);
	return false;
}

bool
GdiEngine::openClipboard ()
{
	bool_t result;

	if (!m_hWndClipboardOwner)
	{
		m_hWndClipboardOwner = ::CreateWindowExW (
			0,
			L"STATIC",
			NULL,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			g::getModule ()->getModuleHandle (),
			NULL
			);

		if (!m_hWndClipboardOwner)
			return err::failWithLastSystemError ();
	}

	result = ::OpenClipboard (m_hWndClipboardOwner);
	return err::complete (result);
}

bool
GdiEngine::showCaret (
	Widget* widget,
	const Rect& rect
	)
{
	return true;
}

void
GdiEngine::hideCaret ()
{
}

//.............................................................................

} // namespace gui
} // namespace axl

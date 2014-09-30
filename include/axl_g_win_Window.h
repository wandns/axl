// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_G_WIN_WINDOW_H

#include "axl_exe_Thunk.h"
#include "axl_g_Module.h"
#include "axl_err_Error.h"
#include "axl_rtl_Handle.h"

namespace axl {
namespace g {
namespace win {

//.............................................................................

class Window: public rtl::Handle <HWND>
{
public:
	// CWindowImplT <> overridables

	enum
	{
		classStyle      = CS_DBLCLKS,
		classCursor     = (int) IDC_ARROW,
		classBackground = COLOR_WINDOW,
	};

public:
	// Win32 wrappers

	bool
	create (
		const wchar_t* className, 
		HWND hWndParent,
		const RECT* rect = NULL,
		const wchar_t* windowName = NULL,
		uint_t style = 0,
		uint_t styleEx = 0,
		uint_t ID = 0,
		void* param = NULL
		)
	{
		static RECT _DefaultRect = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };

		if (!rect)
			rect = &_DefaultRect;

		m_h = ::CreateWindowExW (
			styleEx, 
			className,
			windowName, 
			style,
			rect->left,
			rect->top,
			rect->right - rect->left,
			rect->bottom - rect->top,
			hWndParent,
			(HMENU) ID,
			g::getModule ()->getModuleHandle (),
			param
			);

		return err::complete (m_h != NULL);
	}

	bool 
	destroyWindow ()
	{
		bool_t result = ::DestroyWindow (m_h);
		return err::complete (result);
	}

	LONG_PTR
	getClassLongPtr (int index)
	{
		return ::GetClassLongPtr (m_h, index);
	}

	LONG_PTR
	setClassLongPtr (
		int index, 
		LONG_PTR value
		)
	{
		return ::SetClassLongPtr (m_h, index, value);
	}

	LONG_PTR
	getWindowLongPtr (int index)
	{
		return ::GetWindowLongPtr (m_h, index);
	}

	LONG_PTR
	setWindowLongPtr (
		int index, 
		LONG_PTR value
		)
	{
		return ::SetWindowLongPtr (m_h, index, value);
	}

	LRESULT
	sendMessage (
		UINT msg, 
		WPARAM wParam = 0,
		LPARAM lParam = 0
		)
	{
		return ::SendMessage (m_h, msg, wParam, lParam);
	}

	LRESULT
	postMessage (
		UINT msg, 
		WPARAM wParam = 0,
		LPARAM lParam = 0
		)
	{
		return ::PostMessage (m_h, msg, wParam, lParam);
	}

	bool 
	showWindow (int show = SW_SHOW)
	{
		bool_t result = ::ShowWindow (m_h, show);
		return err::complete (result);
	}

	bool 
	setWindowPos (
		HWND hWndInsertAfter,
		int x,
		int y,
		int width,
		int height,
		uint_t flags
		)
	{
		bool_t result = ::SetWindowPos (m_h, hWndInsertAfter, x, y, width, height, flags);
		return err::complete (result);
	}

	HDWP 
	deferWindowPos (
		HDWP hDwp,
		HWND hWndInsertAfter,
		int x,
		int y,
		int width,
		int height,
		uint_t flags
		)
	{
		return ::DeferWindowPos (hDwp, m_h, hWndInsertAfter, x, y, width, height, flags);
	}

	// CWindowImplT <> overridable WindowProc

	LRESULT 
	windowProc (
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam, 
		bool* isHandled_o
		) 
	{
		*isHandled_o = false;
		return 0;
	} 
};

//.............................................................................

// lightweight window implementation class
// keeps object pointer in WNDLONGPTR::0 and not in Thunk (like ATL)
// therefore, creating windows based on existing class (e.g. EDIT-based class) is not possible
// implementation on thunks would interfere with ATL and therefore it would be not possible to use axl window as a 
// superclass for ATL CWindowImpl <>. unlike ATL doesnt require supporting C++ object 
// so it's possible to register a class and then use it with ::CreateWindow or as a superclass for ATL::CWindowImpl
// note that unlike deriving from existing classes subclassing of existing windows IS supported.

extern
AXL_SELECT_ANY
const uint_t g_createWindowTls = ::TlsAlloc ();

template <
	class T, 
	class B = Window
	>
class WindowImpl: public B
{
protected:
	enum WindowStateKind
	{
		WindowStateKind_Idle = 0,
		WindowStateKind_Normal,
		WindowStateKind_Internal,
		WindowStateKind_Attached,
		WindowStateKind_Subclassed
	};

	class RegisterWndClass
	{
	public:
		void 
		operator () (WNDCLASSW** wndClass_o)
		{
			static WNDCLASSW wndClass = { 0 };
			wndClass.style = T::classStyle;
			wndClass.hCursor = ::LoadCursor (NULL, MAKEINTRESOURCE (T::classCursor));
			wndClass.hbrBackground = (HBRUSH) (T::classBackground + 1);
			wndClass.hInstance = g::getModule ()->getModuleHandle ();
			wndClass.lpfnWndProc = &_WindowProc;
			wndClass.cbWndExtra = sizeof (T*);				
			wndClass.lpszClassName = T::getClassName ();

			if (!wndClass.lpszClassName)
			{
				static wchar_t className [64] = { 0 };
				_snwprintf (className, countof (className) - 1, L"axl:%x", &_WindowProc);
				wndClass.lpszClassName = className;
			}; 

			bool_t result = ::RegisterClass (&wndClass);
			if (!result)
			{
				err::setLastSystemError ();
				return;
			}

			*wndClass_o = &wndClass;
		}
	};

protected:
	int m_windowState;

	// subclass

	exe::Thunk* m_subclassWindowThunk;
	WNDPROC m_pfOriginalWindowProc;

public:
	WindowImpl ()
	{		
		m_windowState = WindowStateKind_Idle;
		m_subclassWindowThunk = NULL;
		m_pfOriginalWindowProc = NULL;
	}

	~WindowImpl ()
	{
#ifndef _AXL_DISABLE_PREMATURE_WINDOW_DESTRUCTION_CHECK
		ASSERT (!m_h);
#endif
		detach ();
	}

	// reset wndclass name every time

	static 
	const wchar_t*
	getWndClassName ()
	{ 
		return NULL;
	}

	static 
	bool
	registerWndClass ()
	{
		return getWndClass () != NULL;
	}

	static
	const WNDCLASS*
	getWndClass ()
	{
		static WNDCLASS* wndClass = NULL;
		mt::callOnce (RegisterWndClass (), &wndClass);
		return wndClass;
	}

	void
	attach (HWND hWnd)
	{
		detach ();

		m_h = hWnd;
		m_windowState = WindowStateKind_Attached;
	}

	HWND 
	detach ()
	{
		return detachEx (false);
	}

	bool 
	create (
		HWND hWndParent,
		const RECT* rect = NULL,
		const wchar_t* windowName = NULL,
		uint_t style = 0,
		uint_t styleEx = 0,
		uint_t ID = 0,
		void* param = NULL
		)
	{ 
		detach ();

		::TlsSetValue (g_createWindowTls, static_cast <T*> (this));

		const WNDCLASS* wndClass = getWndClass ();
		if (!wndClass)
			return false;

		bool result = Window::create (
			wndClass->lpszClassName, 
			hWndParent, 
			rect, 
			windowName, 
			style, 
			styleEx, 
			ID, 
			param
			); 

		::TlsSetValue (g_createWindowTls, NULL);

		return result;
	}

	bool 
	subclass (HWND hWnd)
	{
		detach ();

		m_subclassWindowThunk = exe::createThunk (_SubclassWindowProc, static_cast <T*> (this));
		if (!m_subclassWindowThunk)
			return false;

		m_h = hWnd;
		m_windowState = WindowStateKind_Subclassed;
		m_pfOriginalWindowProc = (WNDPROC) ::SetWindowLongPtr (hWnd, GWLP_WNDPROC, (LONG_PTR) m_subclassWindowThunk);	
		return true;
	}
	
	static 
	T* 
	fromHandle (HWND hWnd)
	{
		return (T*) (Window*) ::GetWindowLongPtr (hWnd, 0);
	}

	LRESULT 
	defWindowProc (
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam
		)
	{
		return m_windowState == WindowStateKind_Subclassed ? 
			::CallWindowProc (m_pfOriginalWindowProc, m_h, msg, wParam, lParam) :
			::DefWindowProc (m_h, msg, wParam, lParam);
	}

protected:	
	HWND 
	detachEx (bool doDrop)
	{
		HWND hWnd = m_h;

		WNDPROC pfPrev;

		switch (m_windowState)
		{
		case WindowStateKind_Idle:
			return NULL;

		case WindowStateKind_Attached:
			break;

		case WindowStateKind_Internal:
			ASSERT (doDrop); // should only be called from _WindowProc::WM_NCDESTROY
			break;

		case WindowStateKind_Normal:
			if (!doDrop)
			{
				pfPrev = (WNDPROC) setWindowLongPtr (GWLP_WNDPROC, (LONG_PTR) ::defWindowProc);
				ASSERT (pfPrev == (WNDPROC) &_WindowProc); // check if anybody has subclassed us
				setWindowLongPtr (0, NULL);
			}

			break;

		case WindowStateKind_Subclassed:
			ASSERT (m_subclassWindowThunk);

			if (!doDrop)
			{
				pfPrev = (WNDPROC) setWindowLongPtr (GWLP_WNDPROC, (LONG_PTR) m_pfOriginalWindowProc);
				ASSERT (pfPrev == (WNDPROC) m_subclassWindowThunk); // check if anybody has over-subclassed us
			}

			AXL_MEM_FREE (m_subclassWindowThunk);
			m_subclassWindowThunk = NULL;
			m_pfOriginalWindowProc = NULL;
			break;
		}

		m_h = NULL;
		m_windowState = WindowStateKind_Idle;
		return hWnd;
	}

	LRESULT 
	processMessage (
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam
		)
	{
		bool isHandled = true;
		LRESULT result = static_cast <T*> (this)->windowProc (msg, wParam, lParam, &isHandled);
		
		if (!isHandled)
			result = defWindowProc (msg, wParam, lParam);

		return result;
	}

	// actual WNDPROC procedures

	static 
	LRESULT 
	WINAPI
	_WindowProc (
		HWND hWnd,
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam
		)
	{
		if (msg == WM_NCCREATE)
		{
			T* self = (T*) ::TlsGetValue (g_createWindowTls);
			if (self)
			{
				self->m_windowState = WindowStateKind_Normal;
			}
			else
			{
				self = AXL_MEM_NEW (T);
				self->m_windowState = WindowStateKind_Internal;
			}

			self->m_h = hWnd;
			::SetWindowLongPtr (hWnd, 0, (LONG_PTR) self);

			return self->processMessage (msg, wParam, lParam);
		}

		T* self = (T*) ::GetWindowLongPtr (hWnd, 0);
		if (!self)
			return ::DefWindowProc (hWnd, msg, wParam, lParam);

		ASSERT (self->m_windowState == WindowStateKind_Normal || self->m_windowState == WindowStateKind_Internal);

		LRESULT result = self->processMessage (msg, wParam, lParam);

		if (msg == WM_NCDESTROY)
		{
			bool doDelete = self->m_windowState == WindowStateKind_Internal;

			self->detachEx (true);

			if (doDelete)
				AXL_MEM_DELETE (self);
		}

		return result;
	}

	static 
	LRESULT 
	WINAPI
	_SubclassWindowProc (
		T* self, // HWND gonna be replaced with T* by thunk proc
		UINT msg, 
		WPARAM wParam, 
		LPARAM lParam
		)
	{
		ASSERT (self->m_windowState == WindowStateKind_Subclassed);

		LRESULT result = self->processMessage (msg, wParam, lParam);

		if (msg == WM_NCDESTROY)
			self->detachEx (true);

		return result;
	}
};

//.............................................................................

// convenient macros

#define AXL_G_WINDOW_CLASS(name) \
static \
const wchar_t* \
getClassName () \
{ \
	return name; \
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define AXL_G_WINDOW_MSG_MAP_BEGIN() \
LRESULT \
windowProc ( \
	UINT msg, \
	WPARAM wParam, \
	LPARAM lParam, \
	bool* isHandled_o \
	) \
{ \
	LRESULT result = 0; \
	switch (msg) \
	{

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define AXL_G_WINDOW_MSG_HANDLER(msgCode, handler) \
	case msgCode: \
		result = handler (msg, wParam, lParam, isHandled_o); \
		break; \

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

#define AXL_G_WINDOW_MSG_MAP_END() \
	default: \
		*isHandled_o = false; \
	} \
	return result; \
} 

//.............................................................................

} // namespace win
} // namespace g
} // namespace axl


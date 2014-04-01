// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_RTL_STRING_H

#include "axl_ref_Buf.h"
#include "axl_rtl_BitIdx.h"
#include "axl_rtl_Utf.h"
#include "axl_rtl_CharCodec.h"

namespace axl {
namespace rtl {

//.............................................................................

template <typename T>
class CStringDetailsT
{
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <>
class CStringDetailsT <utf8_t>
{
public:
	typedef utf8_t  C;
	typedef utf16_t C2;
	typedef utf32_t C3;

	typedef CUtf8 CEncoding;
	typedef CStringDetailsT <C2> CDetails2;
	typedef CStringDetailsT <C3> CDetails3;
	
	static
	const C*
	GetCrLf ()
	{
		return "\r\n";
	}

	static
	size_t
	CalcLength (const C* p)
	{
		return strlen (p);
	}

	static
	int
	Cmp (
		const C* p1,
		const C* p2,
		size_t Length
		)
	{
		return memcmp (p1, p2, Length);
	}

	static
	void
	Fill (
		C* p,
		C c,
		size_t Count
		)
	{
		memset (p, c, Count);
	}

	static
	void
	Copy (
		C* pDst,
		const C* pSrc,
		size_t Count
		)
	{
		memcpy (pDst, pSrc, Count);
	}

#if (_AXL_CPP == AXL_CPP_MSC)
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		return _vscprintf (pFormat, va.m_va);
	}

	static
	void
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		_vsnprintf (pBuffer, BufferLength, pFormat, va.m_va);
		if (BufferLength)
			pBuffer [BufferLength - 1] = 0; // not really necessary, just to make it identical to C++11 vsnprintf
	}
#else
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		return vsnprintf (NULL, 0, pFormat, va.m_va);
	}

	static
	size_t
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		return vsnprintf (pBuffer, BufferLength, pFormat, va.m_va);
	}
#endif
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <typename T>
class CStringDetailsImplT
{
public:
	static
	const T*
	GetCrLf ()
	{
		static T CrLf [2] = { '\r', '\n' };
		return CrLf;
	}

	static
	size_t
	CalcLength (const T* p)
	{
		const T* p0 = p;
		while (*p)
			p++;
	
		return p - p0;
	}

	static
	int
	Cmp (
		const T* p1,
		const T* p2,
		size_t Length
		)
	{
		const T* pEnd = p1 + Length;
		for (; p1 < pEnd; p1++, p2++)
			if (*p1 < *p2)
				return -1;
			else if (*p1 > *p2)
				return 1;
	
		return 0;
	}

	static
	void
	Fill (
		T* p,
		T c,
		size_t Count
		)
	{
		const T* pEnd = p + Count;
		for (; p < pEnd; p++)
			*p = (C) c;
	}

	static
	void
	Copy (
		T* pDst,
		const T* pSrc,
		size_t Count
		)
	{
		memcpy (pDst, pSrc, Count * sizeof (T));
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	
template <>
class CStringDetailsT <utf16_t>: public CStringDetailsImplT <utf16_t>
{
public:
	typedef utf16_t C;
	typedef utf8_t  C2;
	typedef utf32_t C3;

	typedef CUtf16 CEncoding;
	typedef CStringDetailsT <C2> CDetails2;
	typedef CStringDetailsT <C3> CDetails3;

#if (_AXL_CPP == AXL_CPP_MSC)
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		return _vscwprintf (pFormat, va.m_va);
	}

	static
	void
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		_vsnwprintf (pBuffer, BufferLength, pFormat, va.m_va);
		if (BufferLength)
			pBuffer [BufferLength - 1] = 0; // not really necessary, just to make it identical to C++11 vsnprintf
	}
#else
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		ASSERT (false);
		return 0;
	}

	static
	void
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		ASSERT (false);
	}
#endif
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <>
class CStringDetailsT <utf32_t>: public CStringDetailsImplT <utf32_t>
{
public:
	typedef utf32_t C;
	typedef utf8_t  C2;
	typedef utf16_t C3;

	typedef CUtf32 CEncoding;
	typedef CStringDetailsT <C2> CDetails2;
	typedef CStringDetailsT <C3> CDetails3;

#if (_AXL_CPP == AXL_CPP_MSC)
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		ASSERT (false);
		return 0;
	}

	static
	void
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		ASSERT (false);
	}
#else
	static
	size_t
	CalcFormatLength_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		return vswprintf (NULL, 0, pFormat, va.m_va);
	}

	static
	void
	Format_va (
		C* pBuffer,
		size_t BufferLength,
		const C* pFormat,
		axl_va_list va
		)
	{
		vswprintf (pBuffer, BufferLength, pFormat, va.m_va);
	}
#endif
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

typedef CStringDetailsT <char>    CStringDetails;
typedef CStringDetailsT <wchar_t> CStringDetails_w;
typedef CStringDetailsT <utf8_t>  CStringDetails_utf8;
typedef CStringDetailsT <utf16_t> CStringDetails_utf16;
typedef CStringDetailsT <utf32_t> CStringDetails_utf32;

//.............................................................................

enum EString
{
	EString_Empty = 0
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

template <typename T>
class CStringT
{
public:
	class CHdr: public ref::CRefCount
	{
	public:
		size_t m_MaxLength;
		size_t m_Length;
	};

	typedef CStringDetailsT <T> CDetails;
	typedef typename CDetails::C C;
	typedef typename CDetails::C2 C2;
	typedef typename CDetails::C3 C3;

	typedef typename CDetails::CEncoding CEncoding;
	typedef typename CDetails::CDetails2 CDetails2;
	typedef typename CDetails::CDetails3 CDetails3;
	typedef CStringT <C2> CString2;
	typedef CStringT <C3> CString3;

protected:
	C* m_p;

public:
	CStringT ()
	{
		m_p = NULL;
	}

	CStringT (EString StringKind)
	{
		ASSERT (StringKind == EString_Empty);
		m_p = NULL;
	}

	CStringT (const CStringT& Src)
	{
		m_p = NULL;
		Copy (Src);
	}

	CStringT (const CString2& Src)
	{
		m_p = NULL;
		Copy (Src);
	}

	CStringT (const CString3& Src)
	{
		m_p = NULL;
		Copy (Src);
	}

	CStringT (
		const C* p,
		size_t Length = -1
		)
	{
		m_p = NULL;
		Copy (p, Length);
	}

	CStringT (
		const C2* p,
		size_t Length = -1
		)
	{
		m_p = NULL;
		Copy (p, Length);
	}

	CStringT (
		const C3* p,
		size_t Length = -1
		)
	{
		m_p = NULL;
		Copy (p, Length);
	}

	CStringT (
		utf32_t x,
		size_t Count = 1
		)
	{
		m_p = NULL;
		Copy (x, Count);
	}

	CStringT (
		ref::EBuf Kind,
		void* p,
		size_t Size
		)
	{
		m_p = NULL;
		SetBuffer (Kind, p, Size);
	}

	~CStringT ()
	{
		Release ();
	}

	operator const C* () const
	{
		return m_p;
	}

	operator C* ()
	{
		return m_p;
	}

	bool
	operator == (const CStringT& String) const
	{
		return Cmp (String) == 0;
	}

	bool
	operator == (const C* p) const
	{
		return Cmp (p) == 0;
	}

	bool
	operator != (const CStringT& String) const
	{
		return Cmp (String) != 0;
	}

	bool
	operator != (const C* p) const
	{
		return Cmp (p) != 0;
	}

	CStringT&
	operator = (EString StringKind)
	{
		ASSERT (StringKind == EString_Empty);
		Clear ();
		return *this;
	}

	CStringT&
	operator = (const CStringT& Src)
	{
		Copy (Src);
		return *this;
	}

	CStringT&
	operator = (const CString2& Src)
	{
		Copy (Src);
		return *this;
	}

	CStringT&
	operator = (const CString3& Src)
	{
		Copy (Src);
		return *this;
	}

	CStringT&
	operator = (const C* p)
	{
		Copy (p, -1);
		return *this;
	}

	CStringT&
	operator = (const C2* p)
	{
		Copy (p, -1);
		return *this;
	}

	CStringT&
	operator = (const C3* p)
	{
		Copy (p, -1);
		return *this;
	}

	CStringT&
	operator = (utf32_t x)
	{
		Copy (x, 1);
		return *this;
	}

	CStringT&
	operator += (const C* p)
	{
		Append (p, -1);
		return *this;
	}

	CStringT&
	operator += (const C2* p)
	{
		Append (p, -1);
		return *this;
	}

	CStringT&
	operator += (const C3* p)
	{
		Append (p, -1);
		return *this;
	}

	CStringT&
	operator += (utf32_t x)
	{
		Append (x, 1);
		return *this;
	}

	CStringT
	operator + (const CStringT& String) const
	{
		CStringT Result = *this;
		Result.Append (String);
		return Result;
	}

	CStringT
	operator + (const C* p) const
	{
		CStringT Result = *this;
		Result.Append (p);
		return Result;
	}

	CStringT
	operator + (const C2* p) const
	{
		CStringT Result = *this;
		Result.Append (p);
		return Result;
	}

	CStringT
	operator + (const C3* p) const
	{
		CStringT Result = *this;
		Result.Append (p);
		return Result;
	}

	CStringT
	operator + (const CString2& String) const
	{
		CStringT Result = *this;
		Result.Append (String);
		return Result;
	}

	CStringT
	operator + (const CString3& String) const
	{
		CStringT Result = *this;
		Result.Append (String);
		return Result;
	}

	const C*
	cc () const
	{
		return m_p;
	}

	C*
	c ()
	{
		return m_p;
	}

	CString2
	cc2 () const
	{
		return CString2 (m_p, GetLength ());
	}

	CString3
	cc3 () const
	{
		return CString3 (m_p, GetLength ());
	}

	size_t
	GetLength () const
	{
		return m_p ? GetHdr ()->m_Length : 0;
	}

	size_t
	GetBufferLength () const
	{
		return m_p ? GetHdr ()->m_MaxLength : 0;
	}

	bool
	IsEmpty () const
	{
		return GetLength () == 0;
	}

	void
	Release ()
	{
		if (!m_p)
			return;

		GetHdr ()->Release ();
		m_p = NULL;
	}

	void
	Clear ()
	{
		SetLength (0, false);
	}

	int
	Cmp (
		const C* p,
		size_t Length = -1
		) const
	{
		size_t ThisLength = GetLength ();

		if (!p)
			return ThisLength ? 1 : 0;

		if (Length == -1)
			Length = CDetails::CalcLength (p);

		int Result = CDetails::Cmp (m_p, p, AXL_MIN (Length, ThisLength));
		return Result ? Result : rtl::CCmpT <size_t> () (ThisLength, Length);
	}

	int
	Cmp (const CStringT& String) const
	{
		return Cmp (String, String.GetLength ());
	}

	size_t
	ForceCopy (const CStringT& Src)
	{
		return Copy (Src, Src.GetLength ());
	}

	size_t
	Copy (const CStringT& Src)
	{
		if (m_p == Src.m_p)
			return GetLength ();

		if (Src.m_p)
		{
			if (Src.GetHdr ()->GetFree () == (mem::FFree*) -1)
				return Copy (Src, Src.GetLength ());

			Src.GetHdr ()->AddRef ();
		}

		if (m_p)
			GetHdr ()->Release ();

		m_p = Src.m_p;
		return GetLength ();
	}

	size_t
	Copy (const CString2& Src)
	{
		return Copy (Src, Src.GetLength ());
	}

	size_t
	Copy (const CString3& Src)
	{
		return Copy (Src, Src.GetLength ());
	}

	size_t
	Copy (
		const C* p,
		size_t Length = -1
		)
	{
		if (p == m_p && Length == -1)
			return GetLength ();

		if (!p)
		{
			Clear ();
			return 0;
		}

		if (Length == -1)
			Length = CDetails::CalcLength (p);

		if (!SetLength (Length, false))
			return -1;

		if (p != m_p)
			CDetails::Copy (m_p, p, Length);

		return Length;
	}

	size_t
	Copy (
		const C2* p,
		size_t Length = -1
		)
	{
		if (!p)
		{
			Clear ();
			return 0;
		}

		if (Length == -1)
			Length = CDetails2::CalcLength (p);

		size_t NewLength = CUtfConvertT <CEncoding, CDetails2::CEncoding>::CalcRequiredLength (p, Length);
		if (NewLength == -1)
			return -1;

		if (!SetLength (NewLength, false))
			return -1;

		CUtfConvertT <CEncoding, CDetails2::CEncoding>::Convert (m_p, NewLength, p, Length);
		return Length;
	}

	size_t
	Copy (
		const C3* p,
		size_t Length = -1
		)
	{
		if (!p)
		{
			Clear ();
			return 0;
		}

		if (Length == -1)
			Length = CDetails3::CalcLength (p);

		size_t NewLength = CUtfConvertT <CEncoding, CDetails3::CEncoding>::CalcRequiredLength (p, Length);
		if (NewLength == -1)
			return -1;

		if (!SetLength (NewLength, false))
			return -1;

		CUtfConvertT <CEncoding, CDetails3::CEncoding>::Convert (m_p, NewLength, p, Length);
		return Length;
	}

	size_t
	Copy (
		utf32_t x,
		size_t Count = 1
		)
	{
		if (!Count)
		{
			Clear ();
			return 0;
		}

		size_t CodePointLength = CEncoding::GetEncodeCodePointLength (x);
		if (CodePointLength == -1)
			return -1;

		ASSERT (CodePointLength <= 4);

		if (!SetLength (Count * CodePointLength, false))
			return -1;

		C Pattern [sizeof (utf32_t) / sizeof (C)];
		CEncoding::EncodeCodePoint (Pattern, x);
		FillWithPattern (m_p, Pattern, CodePointLength, Count);
		return Count;
	}


	size_t
	Append (const CStringT& Src)
	{
		return IsEmpty () ? Copy (Src) : Append (Src, Src.GetLength ());
	}

	size_t
	Append (const CString2& Src)
	{
		return Append (Src, Src.GetLength ());
	}

	size_t
	Append (const CString3& Src)
	{
		return Append (Src, Src.GetLength ());
	}

	size_t
	Append (
		const C* p,
		size_t Length = -1
		)
	{
		size_t OldLength = GetLength ();

		if (!p)
			return OldLength;

		if (Length == -1)
			Length = CDetails::CalcLength (p);

		if (Length == 0)
			return OldLength;

		size_t NewLength = OldLength + Length;
		if (!SetLength (NewLength, true))
			return -1;

		CDetails::Copy (m_p + OldLength, p, Length);
		return NewLength;
	}

	size_t
	Append (
		const C2* p,
		size_t Length = -1
		)
	{
		size_t OldLength = GetLength ();

		if (!p)
			return OldLength;

		if (Length == -1)
			Length = CDetails2::CalcLength (p);

		if (Length == 0)
			return OldLength;

		size_t AppendLength = CUtfConvertT <CEncoding, CDetails2::CEncoding>::CalcRequiredLength (p, Length);
		if (AppendLength == -1)
			return -1;

		size_t NewLength = OldLength + AppendLength;
		if (!SetLength (NewLength, true))
			return -1;

		CUtfConvertT <CEncoding, CDetails2::CEncoding>::Convert (m_p + OldLength, AppendLength, p, Length);
		return NewLength;
	}

	size_t
	Append (
		const C3* p,
		size_t Length = -1
		)
	{
		size_t OldLength = GetLength ();

		if (!p)
			return OldLength;

		if (Length == -1)
			Length = CDetails3::CalcLength (p);

		if (Length == 0)
			return OldLength;

		size_t AppendLength = CUtfConvertT <CEncoding, CDetails3::CEncoding>::CalcRequiredLength (p, Length);
		if (AppendLength == -1)
			return -1;

		size_t NewLength = OldLength + AppendLength;
		if (!SetLength (NewLength, true))
			return -1;

		CUtfConvertT <CEncoding, CDetails3::CEncoding>::Convert (m_p + OldLength, AppendLength, p, Length);
		return NewLength;
	}

	size_t
	Append (
		utf32_t x,
		size_t Count = 1
		)
	{
		size_t OldLength = GetLength ();

		if (Count == 0)
			return OldLength;

		size_t CodePointLength = CEncoding::GetEncodeCodePointLength (x);
		if (CodePointLength == -1)
			return -1;

		ASSERT (CodePointLength <= 4);

		size_t NewLength = OldLength + Count * CodePointLength;
		if (!SetLength (NewLength, true))
			return -1;

		C Pattern [sizeof (utf32_t) / sizeof (C)];
		CEncoding::EncodeCodePoint (Pattern, x);
		FillWithPattern (m_p + OldLength, Pattern, CodePointLength, Count);
		return NewLength;
	}

	size_t
	AppendNewLine ()
	{
#if (_AXL_ENV == AXL_ENV_WIN)
		return Append (CDetails::GetCrLf (), 2);
#else
		return Append ('\n');
#endif
	}

	size_t
	Format_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		size_t Length = CDetails::CalcFormatLength_va (pFormat, va);
		if (!SetLength (Length, false))
			return -1;

		CDetails::Format_va (m_p, Length + 1, pFormat, va);
		return Length;
	}

	size_t
	Format (
		const C* pFormat,
		...
		)
	{
		AXL_VA_DECL (va, pFormat);
		return Format_va (pFormat, va);
	}

	size_t
	AppendFormat_va (
		const C* pFormat,
		axl_va_list va
		)
	{
		size_t AppendLength = CDetails::CalcFormatLength_va (pFormat, va);
		size_t OldLength = GetLength ();
		size_t NewLength = OldLength + AppendLength;
		if (!SetLength (NewLength, true))
			return -1;

		CDetails::Format_va (m_p + OldLength, AppendLength + 1, pFormat, va);
		return NewLength;
	}

	size_t
	AppendFormat (
		const C* pFormat,
		...
		)
	{
		AXL_VA_DECL (va, pFormat);
		return AppendFormat_va (pFormat, va);
	}

	static
	CStringT
	Format_sva (
		const C* pFormat,
		axl_va_list va
		)
	{
		CStringT Result;
		Result.Format_va (pFormat, va);
		return Result;
	}

	static
	CStringT
	Format_s (
		const C* pFormat,
		...
		)
	{
		AXL_VA_DECL (va, pFormat);
		return Format_sva (pFormat, va);
	}

	size_t
	ReduceLength (size_t Delta)
	{
		size_t Length = GetLength ();
		if (Length <= Delta)
		{
			Clear ();
			return 0;
		}

		Length -= Delta;
		SetLength (Length, true);
		return Length;
	}

	C*
	GetBuffer ()
	{
		return EnsureExclusive () ? m_p : NULL;
	}

	C*
	GetBuffer (
		size_t Length,
		bool SaveContents = false
		)
	{
		return SetLength (Length, SaveContents) ? m_p : NULL;
	}

	C*
	Reserve (
		size_t Length,
		bool SaveContents = false
		)
	{
		size_t OldLength = GetLength ();

		if (!SetLength (Length, SaveContents))
			return NULL;

		SetLength (OldLength, SaveContents);
		return m_p;
	}

	size_t
	UpdateLength ()
	{
		if (!m_p)
			return 0;

		CHdr* pHdr = GetHdr ();
		pHdr->m_Length = CDetails::CalcLength (m_p);
		return pHdr->m_Length;
	}

	bool
	EnsureExclusive ()
	{
		return m_p ? SetLength (GetLength (), true) : true;
	}

	void
	SetBuffer (
		ref::EBuf Kind,
		void* p,
		size_t Size
		)
	{
		ASSERT (Size >= sizeof (CHdr) + sizeof (C));

		CHdr* pOldHdr = GetHdr ();

		mem::FFree* pfFree = Kind == ref::EBuf_Static ? NULL : (mem::FFree*) -1;
		ref::CPtrT <CHdr> NewHdr = AXL_REF_NEW_INPLACE (CHdr, p, pfFree);
		NewHdr->m_Length = 0;
		NewHdr->m_MaxLength = (Size - sizeof (CHdr)) / sizeof (C) - 1;

		if (pOldHdr)
			pOldHdr->Release ();

		m_p = (T*) (NewHdr + 1);
		m_p [0] = 0;

		NewHdr.Detach ();
	}

protected:
	CHdr*
	GetHdr () const
	{
		return m_p ? (CHdr*) m_p - 1 : NULL;
	}

	bool
	SetLength (
		size_t Length,
		bool SaveContents
		)
	{
		CHdr* pOldHdr = GetHdr ();

		if (pOldHdr &&
			pOldHdr->m_MaxLength >= Length &&
			pOldHdr->GetRefCount () == 1)
		{
			pOldHdr->m_Length = Length;
			m_p [Length] = 0;
			return true;
		}

		if (Length == 0)
		{
			Release ();
			return true;
		}

		size_t MaxLength = rtl::GetMinPower2Gt (Length) - 1;
		ASSERT (MaxLength >= Length);

		size_t Size = (MaxLength + 1) * sizeof (C);
		ref::CPtrT <CHdr> NewHdr = AXL_REF_NEW_EXTRA (CHdr, Size);
		if (!NewHdr)
			return false;

		NewHdr->m_Length = Length;
		NewHdr->m_MaxLength = MaxLength;

		C* p = (C*) (NewHdr + 1);
		p [0] = p [Length] = 0;

		if (SaveContents && m_p)
		{
			size_t CopyLength = AXL_MIN (Length, pOldHdr->m_Length);
			CDetails::Copy (p, m_p, CopyLength + 1);
		}

		if (pOldHdr)
			pOldHdr->Release ();

		m_p = p;

		NewHdr.Detach ();
		return true;
	}

	static
	void
	FillWithPattern (
		C* p,
		const C* pPattern,
		size_t PatternLength,
		size_t Count
		)
	{
		if (PatternLength == 1)
		{
			CDetails::Fill (p, *pPattern, Count);
			return;
		}

		C* pEnd = p + Count * PatternLength;

		for (; p < pEnd; p += PatternLength)
			CDetails::Copy (p, pPattern, PatternLength);
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

typedef CStringT <char>    CString;
typedef CStringT <wchar_t> CString_w;
typedef CStringT <utf8_t>  CString_utf8;
typedef CStringT <utf16_t> CString_utf16;
typedef CStringT <utf32_t> CString_utf32;

//.............................................................................

inline
CString
FormatString_va (
	const char* pFormat,
	axl_va_list va
	)
{
	return CString::Format_sva (pFormat, va);
}

inline
CString
FormatString (
	const char* pFormat,
	...
	)
{
	AXL_VA_DECL (va, pFormat);
	return CString::Format_sva (pFormat, va);
}

//.............................................................................

} // namespace rtl
} // namespace axl


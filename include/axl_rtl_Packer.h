// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2013. All rights reserved
// Author: Vladimir Gladkov

#pragma once 

#define _AXL_RTL_PACKER_H

#include "axl_rtl_Pack.h"
#include "axl_rtl_Array.h"

namespace axl {
namespace rtl {
	
//.............................................................................

struct IPacker: obj::IRoot
{
	// {6836FF28-78AB-483D-9CFF-6B1CB1ADD961}
	AXL_OBJ_INTERFACE (
		IPacker,
		0x6836ff28, 0x78ab, 0x483d, 0x9c, 0xff, 0x6b, 0x1c, 0xb1, 0xad, 0xd9, 0x61
		)

	virtual
	axl_va_list
	Pack_va (
		void* p,
		size_t* pSize,
		axl_va_list va
		) = 0;
	
	void
	Pack (
		void* p,
		size_t* pSize,
		...
		)
	{
		AXL_VA_DECL (va, pSize);
		Pack_va (p, pSize, va);
	}

	size_t
	Count_va (axl_va_list va)
	{
		size_t Size = 0;
		Pack_va (NULL, &Size, va);
		return Size;
	}

	size_t
	Count (
		int Unused,
		...
		)
	{
		AXL_VA_DECL (va, Unused);
		return Count_va (va);
	}

	ref::CPtrT <mem::TBlock> 
	CreatePackage_va (axl_va_list va)
	{
		size_t Size = 0;
		Pack_va (NULL, &Size, va);

		if (Size == -1)
			return ref::EPtr_Null;

		typedef ref::CBoxT <mem::TBlock> CPackage;
		ref::CPtrT <CPackage> Package = AXL_REF_NEW_EXTRA (CPackage, Size);
		Pack_va (Package + 1, &Size, va);
		Package->m_p = Package + 1;
		Package->m_Size = Size;

		return Package;			
	}

	ref::CPtrT <mem::TBlock> 
	CreatePackage (
		int Unused,
		...
		)
	{
		AXL_VA_DECL (va, Unused);
		return CreatePackage_va (va);
	}
};

//.............................................................................

template <typename TPack>
class IPackerImplT: public IPacker
{
public:
	AXL_OBJ_CLASS_0 (IPackerImplT, IPacker)

public:
	virtual
	axl_va_list
	Pack_va (
		void* p,
		size_t* pSize,
		axl_va_list va
		)
	{
		size_t Size;
		return TPack () (p, &Size, va);
	}

	static
	IPackerImplT*
	GetSingleton ()
	{
		return rtl::GetSimpleSingleton <IPackerImplT> ();
	}
};

//.............................................................................

// run-time sequencing

class CPackerSeq: public IPacker
{
public:
	AXL_OBJ_CLASS_0 (CPackerSeq, IPacker)

protected:
	rtl::CArrayT <IPacker*> m_Sequence;

public:
	virtual
	axl_va_list
	Pack_va (
		void* p,
		size_t* pSize,
		axl_va_list va
		);

	void
	Clear ()
	{
		m_Sequence.Clear ();
	}

	size_t
	Append (IPacker* pPacker)
	{
		m_Sequence.Append (pPacker);
		return m_Sequence.GetCount ();
	}

	template <typename T>
	size_t
	Append ()
	{
		return Append (IPackerImplT <T>::GetSingleton ());
	}

	// often times it is more convenient to use printf-like format string for sequencing

	size_t
	AppendFormat (const char* pFormat);

	size_t
	Format (const char* pFormat)
	{
		Clear ();
		return AppendFormat (pFormat);
	}
};

//.............................................................................

inline
ref::CPtrT <mem::TBlock> 
FormatPackage_va (
	const char* pFormat, 
	axl_va_list va
	)
{
	CPackerSeq Packer;
	Packer.Format (pFormat);
	return Packer.CreatePackage_va (va);
}

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
ref::CPtrT <mem::TBlock> 
FormatPackage (
	const char* pFormat, 
	...
	)
{
	AXL_VA_DECL (va, pFormat);
	return FormatPackage_va (pFormat, va);
}

//.............................................................................

// incremental packer

class CPacker
{
protected:
	axl::rtl::CArrayT <uchar_t> m_Buffer;

public:
	void 
	Clear ()
	{
		m_Buffer.Clear ();
	}

	const uchar_t*
	GetBuffer ()
	{
		return m_Buffer;
	}

	size_t
	GetSize ()
	{
		return m_Buffer.GetCount ();
	}

	size_t
	Append_va (
		IPacker* pPack,
		axl_va_list va
		);

	template <typename TPack>
	size_t
	Append_va (axl_va_list va)
	{
		IPacker* pPack = IPackerImplT <TPack>::GetSingleton ();
		return Append_va (pPack, va);
	}

	size_t
	Append (
		const void* p,
		size_t Size
		);
/*
	template <typename T>
	size_t 
	Append (const T& Data)
	{
		IPacker* pPack = IPackerImplT <CPackT <T> >::GetSingleton ();
		return Pack (&Data, sizeof (Data));
	}
*/
	size_t
	AppendFormat_va (
		const char* pFormat,
		axl_va_list va
		)
	{
		CPackerSeq Packer;
		Packer.Format (pFormat);
		return Append_va (&Packer, va);
	}

	size_t
	AppendFormat (
		const char* pFormat,
		...
		)
	{
		AXL_VA_DECL (va, pFormat);
		return AppendFormat_va (pFormat, va);
	}

	size_t
	Format_va (
		const char* pFormat,
		axl_va_list va
		)
	{
		Clear ();
		return AppendFormat_va (pFormat, va);
	}

	size_t
	Format (
		const char* pFormat,
		...
		)
	{
		AXL_VA_DECL (va, pFormat);
		return Format_va (pFormat, va);
	}
};

//.............................................................................

} // namespace rtl
} // namespace axl
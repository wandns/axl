// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Type.h"
#include "axl_jnc_Namespace.h"

namespace axl {
namespace jnc {

//.............................................................................

class CStructMember: 
	public CModuleItem,
	public CName,
	public rtl::TListLink
{
protected:
	friend class CStructType;

	CType* m_pType;
	size_t m_Offset;

public:
	CStructMember ()
	{
		m_ItemKind = EModuleItem_StructMember;
		m_pType = NULL;
		m_Offset = 0;
	}

	CType*
	GetType ()
	{
		return m_pType;
	}

	size_t
	GetOffset ()
	{
		return m_Offset;
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CStructType: public CNamedType
{
protected:
	friend class CTypeMgr;
	friend class CParser;

	rtl::CArrayT <CStructType*> m_BaseTypeArray;
	rtl::CStdListT <CStructMember> m_MemberList;

public:
	CStructType ()
	{
		m_TypeKind = EType_Struct;
	}

	CStructMember*
	FindMember (
		const tchar_t* pName,
		bool Traverse = false
		);

	CStructMember*
	CreateMember (
		const rtl::CString& Name,
		CType* pType,
		size_t PackFactor = 8
		);

	rtl::CIteratorT <CStructMember>
	GetFirstMember ()
	{
		return m_MemberList.GetHead ();
	}
};

//.............................................................................

} // namespace axl {
} // namespace jnc {

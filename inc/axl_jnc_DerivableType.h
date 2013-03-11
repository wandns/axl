// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Type.h"

namespace axl {
namespace jnc {

class CDerivableType;
class CStructType;
class CClassType;

//.............................................................................

enum EBaseTypeFlag
{
	EBaseTypeFlag_Constructed = 1
};

//.............................................................................

class CBaseType: public rtl::TListLink
{
protected:
	friend class CDerivableType;
	friend class CStructType;
	friend class CClassType;

	CDerivableType* m_pType;

	int m_Flags;
	size_t m_Offset;
	size_t m_LlvmIndex;
	size_t m_VTableIndex;

public:
	CBaseType ();

	int 
	GetFlags ()
	{
		return m_Flags;
	}

	CDerivableType*
	GetType ()
	{
		return m_pType;
	}

	size_t 
	GetOffset ()
	{
		return m_Offset;
	}

	size_t
	GetLlvmIndex ()
	{
		return m_LlvmIndex;
	}

	size_t
	GetVTableIndex ()
	{
		return m_VTableIndex;
	}
};

//.............................................................................

class CBaseTypeCoord
{
protected:
	char m_Buffer [256];

public:
	CDerivableType* m_pType;
	size_t m_Offset;
	rtl::CArrayT <size_t> m_LlvmIndexArray;
	size_t m_VTableIndex;

public:
	CBaseTypeCoord ()
	{
		Init ();
	}

	CBaseTypeCoord (const CBaseTypeCoord& Coord)
	{
		Init ();
		*this = Coord;
	}

protected:
	void
	Init ();
};

//.............................................................................

class CDerivableType: public CNamedType
{
protected:
	rtl::CStringHashTableMapAT <CBaseType*> m_BaseTypeMap;
	rtl::CStdListT <CBaseType> m_BaseTypeList;
	
public:
	rtl::CConstListT <CBaseType>
	GetBaseTypeList ()
	{
		return m_BaseTypeList;
	}

	CBaseType*
	AddBaseType (CDerivableType* pType);

	CBaseType*
	FindBaseType (CType* pType)
	{
		rtl::CStringHashTableMapIteratorAT <CBaseType*> It = m_BaseTypeMap.Find (pType->GetSignature ());
		return It ? It->m_Value : NULL;
	}

	bool
	FindBaseTypeTraverse (
		CType* pType,
		CBaseTypeCoord* pCoord = NULL
		)
	{
		return FindBaseTypeTraverseImpl (pType, pCoord, 0);
	}

	void
	MarkConstructed (CBaseType* pBaseType)
	{
		pBaseType->m_Flags |= EBaseTypeFlag_Constructed;
	}

	void
	ClearConstructedFlag ();

protected:
	bool
	FindBaseTypeTraverseImpl (
		CType* pType,
		CBaseTypeCoord* pCoord,
		size_t Level
		);

	virtual
	CModuleItem*
	FindItemTraverseImpl (
		const tchar_t* pName,
		CBaseTypeCoord* pCoord = NULL,
		int Flags = 0
		)
	{
		return FindItemTraverseImpl (pName, pCoord, Flags, 0);
	}

	CModuleItem*
	FindItemTraverseImpl (
		const tchar_t* pName,
		CBaseTypeCoord* pCoord,
		int Flags,
		size_t BaseTypeLevel
		);
};

//.............................................................................

} // namespace jnc {
} // namespace axl {
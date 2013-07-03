// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2013. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_FunctionTypeOverload.h"
#include "axl_jnc_StructType.h"

namespace axl {
namespace jnc {

class CPropertyPtrType;
class CClassType;

struct TPropertyPtrTypeTuple;

//.............................................................................

enum EPropertyTypeFlag
{
	EPropertyTypeFlag_Const    = 0x010000,
	EPropertyTypeFlag_Bindable = 0x020000,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
EPropertyTypeFlag
GetFirstPropertyTypeFlag (uint_t Flags)
{
	return (EPropertyTypeFlag) (1 << rtl::GetLoBitIdx (Flags));
}

const char* 
GetPropertyTypeFlagString (EPropertyTypeFlag Flag);

rtl::CString
GetPropertyTypeFlagString (uint_t Flags);

inline
const char* 
GetFirstPropertyTypeFlagString (uint_t Flags)
{
	return GetPropertyTypeFlagString (GetFirstPropertyTypeFlag (Flags));
}

//.............................................................................

enum EPropertyPtrType
{
	EPropertyPtrType_Normal = 0,
	EPropertyPtrType_Weak,
	EPropertyPtrType_Thin,
	EPropertyPtrType__Count,
};

const char*
GetPropertyPtrTypeKindString (EPropertyPtrType PtrTypeKind);

//.............................................................................

class CPropertyType: public CType
{
	friend class CTypeMgr;

protected:
	CFunctionType* m_pGetterType;
	CFunctionTypeOverload m_SetterType;
	CFunctionType* m_pBinderType;
	rtl::CString m_TypeModifierString;
	CPropertyType* m_pStdObjectMemberPropertyType;
	CPropertyType* m_pShortType;
	CStructType* m_pVTableStructType;
	TPropertyPtrTypeTuple* m_pPropertyPtrTypeTuple;

public:
	CPropertyType ();

	bool
	IsReadOnly ()
	{
		return m_SetterType.IsEmpty ();
	}

	bool
	IsIndexed ()
	{
		return !m_pGetterType->GetArgArray ().IsEmpty ();
	}

	bool
	IsMemberPropertyType ()
	{
		return m_pGetterType->IsMemberMethodType ();
	}

	CType* 
	GetThisArgType ()
	{
		return m_pGetterType->GetThisArgType ();
	}

	CNamedType*
	GetThisTargetType ()
	{
		return m_pGetterType->GetThisTargetType ();
	}

	CFunctionType*
	GetGetterType ()
	{
		return m_pGetterType;
	}

	CFunctionTypeOverload*
	GetSetterType ()
	{
		return &m_SetterType;
	}

	CFunctionType*
	GetBinderType ()
	{
		return m_pBinderType;
	}

	CType*
	GetReturnType ()
	{
		ASSERT (m_pGetterType);
		return m_pGetterType->GetReturnType ();
	}

	CPropertyType*
	GetMemberPropertyType (CClassType* pType);

	CPropertyType*
	GetStdObjectMemberPropertyType ();

	CPropertyType*
	GetShortType  ();

	CPropertyPtrType* 
	GetPropertyPtrType (
		EType TypeKind,
		EPropertyPtrType PtrTypeKind = EPropertyPtrType_Normal,
		uint_t Flags = 0
		);

	CPropertyPtrType* 
	GetPropertyPtrType (
		EPropertyPtrType PtrTypeKind = EPropertyPtrType_Normal,
		uint_t Flags = 0
		)
	{
		return GetPropertyPtrType (EType_PropertyPtr, PtrTypeKind, Flags);
	}

	CStructType*
	GetVTableStructType ();

	rtl::CString
	GetTypeModifierString ();

	static
	rtl::CString
	CreateSignature (
		CFunctionType* pGetterType,
		const CFunctionTypeOverload& SetterType,
		uint_t Flags
		);

protected:
	virtual 
	void
	PrepareTypeString ();

	virtual 
	void
	PrepareLlvmType ()
	{
		ASSERT (false);
	}
};

//.............................................................................

struct TSimplePropertyTypeTuple: rtl::TListLink
{
	CPropertyType* m_PropertyTypeArray [ECallConv__Count] [2] [2]; // callconv x const x bindable
};

//.............................................................................

} // namespace jnc {
} // namespace axl {
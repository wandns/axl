// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_FunctionType.h"

namespace axl {
namespace jnc {

//.............................................................................

enum EFunctionPtrType
{
	EFunctionPtrType_Normal = 0,
	EFunctionPtrType_Thin,
	EFunctionPtrType_Weak,
	EFunctionPtrType_Unsafe,
	EFunctionPtrType__Count,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

const tchar_t*
GetFunctionPtrTypeKindString (EFunctionPtrType PtrTypeKind);

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

inline
EFunctionPtrType 
GetFunctionPtrTypeKindFromModifiers (int Modifiers)
{
	return 
		(Modifiers & ETypeModifier_Unsafe) ? EFunctionPtrType_Unsafe :
		(Modifiers & ETypeModifier_Weak) ? EFunctionPtrType_Weak : 
		(Modifiers & ETypeModifier_Thin) ? EFunctionPtrType_Thin : EFunctionPtrType_Normal;
}

//.............................................................................

class CFunctionPtrType: public CType
{
protected:
	friend class CTypeMgr;

	EFunctionPtrType m_PtrTypeKind;
	CFunctionType* m_pFunctionType;
	CEventType* m_pEventType;
	rtl::CString m_TypeModifierString;

public:
	CFunctionPtrType ();

	EFunctionPtrType
	GetPtrTypeKind ()
	{
		return m_PtrTypeKind;
	}

	CFunctionType* 
	GetFunctionType ()
	{
		return m_pFunctionType;
	}

	CEventType* 
	GetEventType ();

	CStructType* 
	GetFunctionPtrStructType ();

	CStructType* 
	GetFunctionWeakPtrStructType ();

	rtl::CString
	GetTypeModifierString ();

	static
	rtl::CStringA
	CreateSignature (
		CFunctionType* pFunctionType,
		EType TypeKind,
		EFunctionPtrType PtrTypeKind,
		int Flags
		);

protected:
	virtual 
	void
	PrepareTypeString ();

	virtual 
	void
	PrepareLlvmType ();
};

//.............................................................................

class CFunctionPtrTypeTuple: public rtl::TListLink
{
protected:
	friend class CTypeMgr;

	CStructType* m_pFunctionPtrStructType;
	CStructType* m_pFunctionWeakPtrStructType;
	CFunctionPtrType* m_FunctionPtrArray [2] [EFunctionPtrType__Count] [2]; // ref x kind x nonull

public:
	CFunctionPtrTypeTuple ()
	{
		memset (this, 0, sizeof (CFunctionPtrTypeTuple));
	}
};

//.............................................................................

} // namespace jnc {
} // namespace axl {


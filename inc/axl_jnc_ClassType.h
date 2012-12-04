// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_StructType.h"
#include "axl_jnc_PropertyType.h"
#include "axl_jnc_Function.h"
#include "axl_jnc_Decl.h"

namespace axl {
namespace jnc {

class CFunctionMgr;



//.............................................................................

class CClassBaseType: public rtl::TListLink
{
protected:
	friend class CClassType;

	CClassType* m_pType;
	CStructBaseType* m_pStructBaseType;
	size_t m_VTableIndex;

public:
	CClassBaseType ()
	{
		m_pType = NULL;
		m_pStructBaseType = NULL;
		m_VTableIndex = -1;
	}

	CClassType*
	GetType ()
	{
		return m_pType;
	}

	size_t 
	GetOffset ()
	{
		ASSERT (m_pStructBaseType);
		return m_pStructBaseType->GetOffset ();
	}

	size_t
	GetLlvmIndex ()
	{
		ASSERT (m_pStructBaseType);
		return m_pStructBaseType->GetLlvmIndex ();
	}

	size_t 
	GetVTableIndex ()
	{
		return m_VTableIndex;
	}
};

//............................................................................

enum EClassMember
{
	EClassMember_Undefined,

	EClassMember_Field,
	EClassMember_Method,
	EClassMember_Property,
	EClassMember_Constructor,
	EClassMember_Destructor,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

enum EClassMemberStorage
{
	EClassMemberStorage_Undefined,
	EClassMemberStorage_Static,
	EClassMemberStorage_Dynamic,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CClassMember: 
	public CModuleItem,
	public CName,
	public rtl::TListLink
{
protected:
	friend class CClassType;

	EClassMember m_MemberKind;
	EClassMemberStorage m_Storage;
	EAccess m_Access;

public:
	CClassMember ()
	{
		m_ItemKind = EModuleItem_ClassMember;
		m_MemberKind = EClassMember_Undefined;
		m_Storage = EClassMemberStorage_Undefined;
		m_Access = EAccess_Public;
	}

	EClassMember GetMemberKind ()
	{
		return m_MemberKind;
	}

	EClassMemberStorage GetStorage ()
	{
		return m_Storage;
	}

	EAccess GetAccess ()
	{
		return m_Access;
	}

	CClassType*
	GetParentType ()
	{
		return (CClassType*) (CNamedType*) m_pParentNamespace; // double cast cause CStructType is not defined yet
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CClassFieldMember: public CClassMember
{
protected:
	friend class CClassType;

	CType* m_pType;
	CType* m_pBitFieldBaseType;
	size_t m_BitCount;

	CStructMember* m_pStructMember;

public:
	CClassFieldMember ()
	{
		m_MemberKind = EClassMember_Field;
		m_pType = NULL;
		m_pBitFieldBaseType = NULL;
		m_BitCount = 0;

		m_pStructMember = NULL;
	}

	CType*
	GetType ()
	{
		return m_pType;
	}

	size_t
	GetOffset ()
	{
		ASSERT (m_pStructMember);
		return m_pStructMember->GetOffset ();
	}

	size_t
	GetLlvmIndex ()
	{
		ASSERT (m_pStructMember);
		return m_pStructMember->GetLlvmIndex ();
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CClassMethodMember: 
	public CClassMember,
	public CFunctionOverload
{
protected:
	friend class CClassType;

public:
	CClassMethodMember ()
	{
		m_MemberKind = EClassMember_Method;
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CClassPropertyMember: public CClassMember
{
protected:
	friend class CClassType;

	CProperty* m_pProperty;
	size_t m_VTableIndex;

public:
	CClassPropertyMember ()
	{
		m_MemberKind = EClassMember_Property;
		m_pProperty = NULL;
		m_VTableIndex = -1;
	}

	CProperty*
	GetProperty ()
	{
		return m_pProperty;
	}

	size_t
	GetVTableIndex ()
	{
		return m_VTableIndex;
	}
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CClassType: public CNamedType
{
protected:
	friend class CTypeMgr;
	friend class CParser;

	size_t m_PackFactor;

	rtl::CStringHashTableMapAT <CClassBaseType*> m_BaseTypeMap;
	rtl::CStdListT <CClassBaseType> m_BaseTypeList;

	rtl::CStdListT <CClassFieldMember> m_FieldMemberList;
	rtl::CStdListT <CClassMethodMember> m_MethodMemberList;
	rtl::CStdListT <CClassPropertyMember> m_PropertyMemberList;

	CFunction* m_pInitializer;
	CFunction* m_pFinalizer;
	CFunctionOverload m_Constructor;

	rtl::CArrayT <CFunction*> m_VTable;

	CStructType* m_pPointerStructType;
	CStructType* m_pInterfaceHdrStructType;
	CStructType* m_pInterfaceStructType;
	CStructType* m_pClassStructType;
	CStructType* m_pVTableStructType;

	CValue m_VTablePtrValue;

public:
	CClassType ();

	size_t
	GetPackFactor ()
	{
		return m_PackFactor;
	}

	llvm::StructType* 
	GetLlvmType ();

	CStructType* 
	GetPointerStructType ()
	{
		ASSERT (m_pPointerStructType);
		return m_pPointerStructType;
	}

	CStructType* 
	GetInterfaceHdrStructType ()
	{
		ASSERT (m_pInterfaceHdrStructType);
		return m_pInterfaceHdrStructType;
	}

	CStructType* 
	GetInterfaceStructType ()
	{
		ASSERT (m_pInterfaceStructType);
		return m_pInterfaceStructType;
	}

	CStructType* 
	GetClassStructType ()
	{
		ASSERT (m_pClassStructType);
		return m_pClassStructType;
	}

	CStructType* 
	GetVTableStructType ()
	{
		ASSERT (m_pVTableStructType);
		return m_pVTableStructType;
	}

	size_t 
	GetVTableSize ()
	{
		return m_VTable.GetCount ();
	}

	CFunction** 
	GetVTable ()
	{
		return m_VTable;
	}

	bool
	GetVTablePtrValue (CValue* pValue);

	CFunction* 
	GetInitializer ();

	CFunction* 
	GetFinalizer ()
	{
		return m_pFinalizer;
	}

	CFunctionOverload*
	GetConstructor ()
	{
		return &m_Constructor;
	}

	size_t 
	GetBaseTypeCount ()
	{
		return m_BaseTypeList.GetCount ();
	}

	rtl::CIteratorT <CClassBaseType>
	GetFirstBaseType ()
	{
		return m_BaseTypeList.GetHead ();
	}

	bool
	FindBaseType (
		CClassType* pType,
		size_t* pOffset = NULL,
		rtl::CArrayT <size_t>* pLlvmIndexArray = NULL,
		size_t* pVTableIndex = NULL
		);

	CClassBaseType*
	AddBaseType (CClassType* pType);

	rtl::CIteratorT <CClassFieldMember>
	GetFirstFieldMember ()
	{
		return m_FieldMemberList.GetHead ();
	}

	rtl::CIteratorT <CClassMethodMember>
	GetFirstMethodMember ()
	{
		return m_MethodMemberList.GetHead ();
	}

	rtl::CIteratorT <CClassPropertyMember>
	GetFirstPropertyMember ()
	{
		return m_PropertyMemberList.GetHead ();
	}

	CClassMember*
	FindMember (
		const tchar_t* pName,
		size_t* pBaseTypeOffset,
		rtl::CArrayT <size_t>* pLlvmBaseTypeIndexArray,
		size_t* pBaseTypeVTableIndex
		);

	CClassFieldMember*
	CreateFieldMember (
		const rtl::CString& Name,
		CType* pType,
		size_t BitCount = 0
		);

	CFunction*
	CreateMethodMember (
		const rtl::CString& Name,
		CFunctionType* pType,
		rtl::CStdListT <CFunctionFormalArg>* pArgList = NULL
		);

	CClassPropertyMember*
	CreatePropertyMember (
		const rtl::CString& Name,
		CProperty* pProperty
		);
	
	bool
	CalcLayout ();

protected:
	CFunction*
	FindOverridenMethodMember (
		const rtl::CString& Name,
		CFunctionType* pClosureType,
		size_t* pBaseTypeVTableIndex
		);
};

//.............................................................................

} // namespace axl {
} // namespace jnc {

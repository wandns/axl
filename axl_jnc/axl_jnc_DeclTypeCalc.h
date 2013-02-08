// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Decl.h"

namespace axl {
namespace jnc {

class CArrayType;
class CDataPtrType;
class CClassType;
class CClassPtrType;
class CFunctionType;
class CFunctionPtrType;
class CPropertyType;
class CPropertyPtrType;

//.............................................................................

class CDeclTypeCalc
{
protected:
	CModule* m_pModule;
	rtl::CIteratorT <CDeclSuffix> m_Suffix;
	int m_TypeModifiers;

public:
	CType*
	GetType (
		CType* pType,
		int Modifiers,
		const int* pPointerArray,
		size_t PointerCount,
		rtl::CIteratorT <CDeclSuffix> Suffix,
		int* pDataPtrTypeFlags
		);

protected:
	bool
	CheckUnusedModifiers ();

	CType*
	GetIntegerType (CType* pType);

	CArrayType*
	GetArrayType (CType* pElementType);

	CFunctionType*
	GetFunctionType (CType* pReturnType);

	CPropertyType*
	GetPropertyType (CType* pReturnType);

	CEventType*
	GetEventType (CType* pReturnType);

	CDataPtrType*
	GetDataPtrType (CType* pDataType);

	CClassPtrType*
	GetClassPtrType (CClassType* pClassType);

	CFunctionPtrType*
	GetFunctionPtrType (CFunctionType* pFunctionType);

	CPropertyPtrType*
	GetPropertyPtrType (CPropertyType* pPropertyType);
};

//.............................................................................

} // namespace jnc {
} // namespace axl {


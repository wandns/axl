// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Value.h"
#include "axl_jnc_Function.h"
#include "axl_jnc_StructType.h"

namespace axl {
namespace jnc {

//.............................................................................

class CVTableType: public CNamedType
{
protected:
	CStructType* m_pVTableStructType;	
	rtl::CArrayT <CFunction*> m_VTable;
	CValue m_VTablePtrValue;

public:
	CVTableType ()
	{
		m_pVTableStructType = NULL;
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

	CStructType* 
	GetVTableStructType ();

	bool
	GetVTablePtrValue (CValue* pValue);

	void
	AddFunctionToVTable (CFunction* pFunction);
};

//.............................................................................

} // namespace axl {
} // namespace jnc {

// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Type.h"

namespace axl {
namespace jnc {

//.............................................................................

class CImportType: public CType
{
protected:
	friend class CTypeMgr;

	CQualifiedName m_Name;
	rtl::CString m_QualifiedName;
	CNamespace* m_pAnchorNamespace;
	CType* m_pExternType;	

public:
	CImportType ();

	const CQualifiedName&
	GetName ()
	{
		return m_Name;
	}

	CNamespace* 
	GetAnchorNamespace ()
	{
		return m_pAnchorNamespace;
	}

	const rtl::CString&
	GetQualifiedName ();

	CType* 
	GetExternType ()
	{
		return m_pExternType;
	}

	static 
	rtl::CString
	CreateTypeString (const rtl::CString& Tag)
	{
		return rtl::CString::Format_s (_T("import %s"), Tag);
	}

	rtl::CString
	CreateTypeString ()
	{
		return CreateTypeString (GetQualifiedName ());
	}
};

//.............................................................................

} // namespace jnc {
} // namespace axl {

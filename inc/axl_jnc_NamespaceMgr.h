// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Namespace.h"

namespace axl {
namespace jnc {

//.............................................................................

class CNamespaceMgr
{
protected:
	CGlobalNamespace m_GlobalNamespace;
	rtl::CStdListT <CGlobalNamespace> m_NamespaceList;
	rtl::CStdListT <CScope> m_ScopeList;
	rtl::CArrayT <CNamespace*> m_NamespaceStack;
	CNamespace* m_pCurrentNamespace;

public:
	CNamespaceMgr ()
	{
		m_pCurrentNamespace = &m_GlobalNamespace;
	}

	void
	Clear ();

	CGlobalNamespace*
	GetGlobalNamespace ()
	{
		return &m_GlobalNamespace;
	}
	
	CNamespace* 
	GetCurrentNamespace ()
	{
		return m_pCurrentNamespace;
	}

	rtl::CString
	CreateQualifiedName (const tchar_t* pName)
	{
		return m_pCurrentNamespace->CreateQualifiedName (pName);
	}

	CGlobalNamespace*
	CreateNamespace (const rtl::CString& Name);

	CScope*
	CreateScope ();

	CGlobalNamespace*
	OpenNamespace (
		const CToken::CPos& Pos,
		const rtl::CString& Name
		);

	CGlobalNamespace*
	OpenNamespace (
		const CToken::CPos& Pos,
		const CQualifiedName& Name
		);

	void
	OpenNamespace (CNamespace* pNamespace);

	void
	CloseNamespace (size_t Count = 1);
};

//.............................................................................

} // namespace axl {
} // namespace jnc {
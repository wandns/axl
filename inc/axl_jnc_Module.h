// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_AttributeMgr.h"
#include "axl_jnc_TypeMgr.h"
#include "axl_jnc_NamespaceMgr.h"
#include "axl_jnc_FunctionMgr.h"
#include "axl_jnc_VariableMgr.h"
#include "axl_jnc_ConstMgr.h"
#include "axl_jnc_ControlFlowMgr.h"
#include "axl_jnc_OperatorMgr.h"

namespace axl {
namespace jnc {

//.............................................................................

class CModule
{
protected:
	rtl::CString m_FilePath;
	llvm::Module* m_pLlvmModule;

public:
	CTypeMgr m_TypeMgr;
	CAttributeMgr m_AttributeMgr;
	CNamespaceMgr m_NamespaceMgr;
	CFunctionMgr m_FunctionMgr;
	CVariableMgr m_VariableMgr;
	CConstMgr m_ConstMgr;
	CControlFlowMgr m_ControlFlowMgr;
	COperatorMgr m_OperatorMgr;
	
public:
	CModule ();

	~CModule ()
	{
		Clear ();
	}

	rtl::CString
	GetFilePath ()
	{
		return m_FilePath;
	}

	llvm::Module*
	GetLlvmModule ()
	{
		return m_pLlvmModule;
	}

	void
	Clear ();

	bool
	Create (const rtl::CString& FilePath);
};

//.............................................................................

AXL_SELECT_ANY
DWORD g_ModuleTls = TlsAlloc ();

inline
CModule*
GetCurrentThreadModule ()
{
	return (CModule*) TlsGetValue (g_ModuleTls);
}

inline
CModule*
SetCurrentThreadModule (CModule* pModule)
{
	CModule* pPrevModule = GetCurrentThreadModule ();
	TlsSetValue (g_ModuleTls, pModule);
	return pPrevModule;
}

class CSetCurrentThreadModule
{
protected:
	CModule* m_pPrevModule;

public:
	CSetCurrentThreadModule (CModule* pModule)
	{
		m_pPrevModule = SetCurrentThreadModule (pModule);
	}

	~CSetCurrentThreadModule ()
	{
		SetCurrentThreadModule (m_pPrevModule);
	}
};

//.............................................................................

} // namespace axl {
} // namespace jnc {

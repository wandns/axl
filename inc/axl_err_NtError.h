// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once 

#define _AXL_ERR_NTERRORPROVIDER_H

#include "axl_err_ErrorMgr.h"

namespace axl {
namespace err {

//.............................................................................

#if (_AXL_ENV != AXL_ENV_NT)
typedef long NTSTATUS;
#endif

//.............................................................................
	
// {81443347-3BC9-4d5e-8B23-32D780EDB52B}
AXL_DEFINE_GUID (
	GUID_NtError,
	0x81443347, 0x3bc9, 0x4d5e, 0x8b, 0x23, 0x32, 0xd7, 0x80, 0xed, 0xb5, 0x2b
	);

//.............................................................................
	
class CNtErrorProvider: public IErrorProvider
{
public:
	AXL_OBJ_SIMPLE_CLASS (CNtErrorProvider, IErrorProvider)

public:
	static 
	void
	Register ()
	{
		err::GetErrorMgr ()->RegisterProvider (GUID_NtError, rtl::GetSimpleSingleton <CNtErrorProvider> ());
	}
	
	static
	rtl::CString 
	GetErrorDescription (long Status);

	virtual 
	rtl::CString 
	GetErrorDescription (const TError* pError)
	{
		return GetErrorDescription (pError->m_Code);
	}
};

//.............................................................................

class CNtError: public CError
{
public:
	CNtError ()
	{
	}

	CNtError (NTSTATUS Status)
	{
		Create (Status);
	}

	TError* 
	Create (NTSTATUS Status);
};

//.............................................................................

inline
CError
SetNtError (NTSTATUS Status)
{
	SetError (CNtError (Status));
}

//.............................................................................


} // namespace err
} // namespace axl


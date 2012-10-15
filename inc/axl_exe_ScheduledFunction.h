// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once 

#define _AXL_EXE_SCHEDULEDFUNCTION_H

#include "axl_exe_Function.h"
#include "axl_exe_Scheduler.h"

namespace axl {
namespace exe {

//.............................................................................

template <
	typename TCtxArg,
	typename TParArg
	>
class CScheduledFunctionT: public IFunction
{
public:
	AXL_OBJ_SIMPLE_CLASS (CScheduledFunctionT, IFunction)

protected:
	typedef CFunctionT <TCtxArg, TParArg> CFunction;

	IScheduler* m_pScheduler;
	CFunction m_Function;

public:
	CScheduledFunctionT (IScheduler* pScheduler = NULL)
	{
		m_pScheduler = pScheduler;
	}

	CScheduledFunctionT (
		IScheduler* pScheduler,
		int Convention,
		void* pfn,
		...
		)
	{
		SetupV (pScheduler, Convention, pfn, va_start_e (pfn));
	}

	CScheduledFunctionT (
		IScheduler* pScheduler,
		void* pfn,
		...
		)
	{
		SetupV (pScheduler, pfn, va_start_e (pfn));
	}

	void
	SetupV (
		IScheduler* pScheduler,
		ECallingConvention Convention,
		void* pfn,
		va_list va
		)
	{
		m_pScheduler = pScheduler;
		m_Function.SetupV (Convention, pfn, va);
	}

	void
	SetupV (
		IScheduler* pScheduler,
		void* pfn,
		va_list va
		)
	{
		SetupV (pScheduler, ECallingConvention_Cdecl, pfn, va);
	}

	void
	Setup (
		IScheduler* pScheduler,
		ECallingConvention Convention,
		void* pfn,
		...
		)
	{
		SetupV (pScheduler, Convention, pfn, va_start_e (pfn));
	}

	void
	Setup (
		IScheduler* pScheduler,
		void* pfn,
		...
		)
	{
		SetupV (pScheduler, pfn, va_start_e (pfn));
	}

	virtual 
	IArgPacker* 
	GetArgPacker ()
	{
		return m_Function.GetArgPacker ();
	}

	virtual 
	intptr_t 
	InvokeV (va_list va)
	{
		if (!m_pScheduler)
			return m_Function.InvokeV (va);

		m_pScheduler->ScheduleV (&m_Function, va);
		return 0;
	}
};

//.............................................................................

} // namespace exe
} // namespace axl
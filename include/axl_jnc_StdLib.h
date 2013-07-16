// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2013. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Value.h"

namespace axl {
namespace jnc {

class CMulticastClassType;

//.............................................................................

class CStdLib
{
public:
	static
	void
	Export (
		CModule* pModule,
		llvm::ExecutionEngine* pLlvmExecutionEngine
		);
	
	static
	void
	OnRuntimeError (
		int Error,
		void* pCodeAddr,
		void* pDataAddr
		);

	static
	TInterface* 
	DynamicCastClassPtr (
		TInterface* p,
		CClassType* pType
		);

	static
	TInterface* 
	StrengthenClassPtr (TInterface* p);

	static
	void
	MulticastClear (TMulticast* pMulticast);

	static
	handle_t
	MulticastSet (
		TMulticast* pMulticast,
		TFunctionPtr Ptr
		);

	static
	handle_t
	MulticastSet_t (
		TMulticast* pMulticast,
		void* pf
		);

	static
	handle_t
	MulticastAdd (
		TMulticast* pMulticast,
		TFunctionPtr Ptr
		);

	static
	handle_t
	MulticastAdd_t (
		TMulticast* pMulticast,
		void* pf
		);

	static
	TFunctionPtr
	MulticastRemove (
		TMulticast* pMulticast,
		handle_t Handle
		);

	static
	void*
	MulticastRemove_t (
		TMulticast* pMulticast,
		handle_t Handle
		);

	static
	TFunctionPtr
	MulticastGetSnapshot (TMulticast* pMulticast);

	static
	void*
	HeapAlloc (CType* pType);

	static
	void*
	UHeapAlloc (CType* pType);

	static
	void
	UHeapFree (void* p);

	static
	void
	UHeapFreeClassPtr (TInterface* p);

	static
	void
	GcAddObject (TObject* p);

	static
	void
	GcSafePoint ();

	static
	void
	RunGc ();

	static
	dword_t
	GetCurrentThreadId ();

	static
	int
	CreateThread (TFunctionPtr Ptr);

	static
	void
	Sleep (uint32_t MsCount);

	static
	void*
	GetTls ();

protected:
#if (_AXL_ENV == AXL_ENV_WIN)
	static
	DWORD 
	WINAPI
	ThreadProc (PVOID pContext);
#endif

	static
	void
	ExportMulticastMethods (
		CModule* pModule,
		llvm::ExecutionEngine* pLlvmExecutionEngine,
		CMulticastClassType* pMulticastType
		);
};

//.............................................................................

} // namespace jnc {
} // namespace axl {
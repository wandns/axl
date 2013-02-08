// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_CastOp.h"
#include "axl_jnc_DataPtrType.h"

namespace axl {
namespace jnc {

class CStructBaseTypeCoord;

//.............................................................................

// array -> ptr

class CCast_DataPtrFromArray: public ICastOperator
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtrFromArray, ICastOperator)

public:
	virtual
	ECast
	GetCastKind (
		const CValue& OpValue,
		CType* pType
		);

	virtual
	bool
	ConstCast (
		const CValue& OpValue,
		CType* pType,
		void* pDst
		);

	virtual
	bool
	LlvmCast (
		EAlloc AllocKind,
		const CValue& OpValue,
		CType* pType,
		CValue* pResultValue
		)
	{
		SetCastError (OpValue, pType);
		return false;
	}
};

//.............................................................................

// data ptr -> data ptr

class CCast_DataPtr_Base: public ICastOperator
{
public:
	virtual
	ECast
	GetCastKind (
		const CValue& OpValue,
		CType* pType
		);

protected:
	intptr_t
	GetOffset (
		CDataPtrType* pSrcType,
		CDataPtrType* pDstType,
		CStructBaseTypeCoord* pCoord
		);

	intptr_t
	GetOffsetUnsafePtrValue (
		const CValue& PtrValue, 
		CDataPtrType* pSrcType,
		CDataPtrType* pDstType,
		CValue* pResultValue
		);
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CCast_DataPtr_Normal2Normal: public CCast_DataPtr_Base
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtr_Normal2Normal, ICastOperator)

public:
	virtual
	bool
	ConstCast (
		const CValue& OpValue,
		CType* pType,
		void* pDst
		);

	virtual
	bool
	LlvmCast (
		EAlloc AllocKind,
		const CValue& OpValue,
		CType* pType,
		CValue* pResultValue
		);
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CCast_DataPtr_Thin2Normal: public CCast_DataPtr_Base
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtr_Thin2Normal, ICastOperator)

public:
	virtual
	bool
	ConstCast (
		const CValue& OpValue,
		CType* pType,
		void* pDst
		)
	{
		ASSERT (false); // const cannot be thin
		return false;
	}

	virtual
	bool
	LlvmCast (
		EAlloc AllocKind,
		const CValue& OpValue,
		CType* pType,
		CValue* pResultValue
		);
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CCast_DataPtr_Normal2Unsafe: public CCast_DataPtr_Base
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtr_Normal2Unsafe, ICastOperator)

public:
	virtual
	bool
	ConstCast (
		const CValue& OpValue,
		CType* pType,
		void* pDst
		);

	virtual
	bool
	LlvmCast (
		EAlloc AllocKind,
		const CValue& OpValue,
		CType* pType,
		CValue* pResultValue
		);
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CCast_DataPtr_Unsafe2Unsafe: public CCast_DataPtr_Base
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtr_Unsafe2Unsafe, ICastOperator)

public:
	virtual
	bool
	ConstCast (
		const CValue& OpValue,
		CType* pType,
		void* pDst
		);

	virtual
	bool
	LlvmCast (
		EAlloc AllocKind,
		const CValue& OpValue,
		CType* pType,
		CValue* pResultValue
		);
};

//.............................................................................

// data ptr master cast

class CCast_DataPtr: public CCast_Master
{
public:
	AXL_OBJ_SIMPLE_CLASS (CCast_DataPtr, ICastOperator)

protected:
	CCast_DataPtrFromArray m_FromArray;
	CCast_DataPtr_Normal2Normal m_FromDataPtr_Normal2Normal;
	CCast_DataPtr_Thin2Normal m_FromDataPtr_Thin2Normal;
	CCast_DataPtr_Normal2Unsafe m_FromDataPtr_Normal2Unsafe;
	CCast_DataPtr_Unsafe2Unsafe m_FromDataPtr_Unsafe2Unsafe;

	ICastOperator* m_OperatorTable [EDataPtrType__Count] [2];

public:
	CCast_DataPtr ();

	virtual
	ICastOperator*
	GetCastOperator (
		const CValue& OpValue,
		CType* pType
		);
};

//.............................................................................

} // namespace jnc {
} // namespace axl {
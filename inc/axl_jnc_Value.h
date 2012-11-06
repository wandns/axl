// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_Type.h"

namespace axl {
namespace jnc {

class CVariable;
class CGlobalFunction;
class CGlobalProperty;

//.............................................................................
	
enum EValue
{
	EValue_Void = 0,
	EValue_Null,
	EValue_Type,
	EValue_Const,
	EValue_Variable,
	EValue_VariableAddr,
	EValue_LlvmRegister,
	EValue_GlobalFunction,
	EValue_GlobalProperty,	
	EValue_BoolNot,
	EValue_BoolAnd,
	EValue_BoolOr,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

const tchar_t*
GetValueKindString (EValue ValueKind);

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class CValue
{
protected:
	struct TBufHdr
	{
		size_t m_Size;
	};

	class CGetBufSize 
	{
	public:
		size_t
		operator () (const TBufHdr& Hdr)
		{
			return sizeof (TBufHdr) + Hdr.m_Size;
		}
	};

	typedef ref::CBufT <TBufHdr, CGetBufSize> CConstBuf;

protected:
	EValue m_ValueKind;
	CType* m_pType;	
	CConstBuf m_Const;
	uchar_t m_ConstBuffer [CConstBuf::MinBufSize + 8];

	union
	{
		CVariable* m_pVariable;
		CGlobalFunction* m_pGlobalFunction;
		CGlobalProperty* m_pGlobalProperty;
	};

	mutable llvm::Value* m_pLlvmValue;

public:
	CValue ();

	CValue (const CValue& Value):
		m_Const (ref::EBuf_Field, m_ConstBuffer, sizeof (m_ConstBuffer))
	{
		*this = Value;
	}

	CValue (
		CType* pType,
		const void* p = NULL
		);

	CValue (CVariable* pVariable)
	{
		SetVariable (pVariable);
	}

	CValue (CGlobalFunction* pGlobalFunction)
	{
		SetGlobalFunction (pGlobalFunction);
	}

	CValue (CGlobalProperty* pGlobalProperty)
	{
		SetGlobalProperty (pGlobalProperty);
	}

	EValue 
	GetValueKind () const
	{
		return m_ValueKind;
	}

	const tchar_t*
	GetValueKindString () const
	{
		return jnc::GetValueKindString (m_ValueKind);
	}

	CType* 
	GetType () const
	{
		return m_pType;
	}

	CVariable*
	GetVariable () const
	{
		ASSERT (m_ValueKind == EValue_Variable);
		return m_pVariable;
	}

	CGlobalFunction*
	GetGlobalFunction () const
	{
		ASSERT (m_ValueKind == EValue_GlobalFunction);
		return m_pGlobalFunction;
	}

	CGlobalProperty*
	GetGlobalProperty () const
	{
		ASSERT (m_ValueKind == EValue_GlobalProperty);
		return m_pGlobalProperty;
	}

	void*
	GetConstData () const
	{
		ASSERT (m_ValueKind == EValue_Const);
		return (void*) (m_Const + 1);
	}

	int32_t
	GetInt32 () const
	{
		ASSERT (m_ValueKind == EValue_Const && m_pType->GetSize () >= sizeof (int32_t));
		return *(int32_t*) GetConstData ();
	}

	int64_t
	GetInt64 () const
	{
		ASSERT (m_ValueKind == EValue_Const && m_pType->GetSize () >= sizeof (int64_t));
		return *(int64_t*) GetConstData ();
	}

	float
	GetFloat () const
	{
		ASSERT (m_ValueKind == EValue_Const && m_pType->GetSize () >= sizeof (float));
		return *(float*) GetConstData ();
	}

	double
	GetDouble () const
	{
		ASSERT (m_ValueKind == EValue_Const && m_pType->GetSize () >= sizeof (double));
		return *(double*) GetConstData ();
	}

	llvm::Value*
	GetLlvmValue () const;

	void
	SetVoid ()
	{
		m_ValueKind = EValue_Void;
	}

	void
	SetNull ()
	{
		m_ValueKind = EValue_Null;
	}

	void
	SetType (CType* pType);

	void
	SetVariable (CVariable* pVariable);

	void
	SetGlobalFunction (CGlobalFunction* pFunction);

	void
	SetGlobalProperty (CGlobalProperty* pProperty);

	bool
	CreateConst (
		CType* pType,
		const void* p = NULL
		);

	void
	SetConstBool (bool Bool);

	void
	SetConstInt32 (
		int32_t Integer,
		CType* pType
		)
	{
		CreateConst (pType, &Integer);
	}

	void
	SetConstInt32 (
		int32_t Integer,
		EType TypeKind
		);

	void
	SetConstInt32 (int32_t Integer)
	{
		SetConstInt32 (Integer, GetInt32TypeKind (Integer));
	}

	void
	SetConstUInt32 (
		uint32_t Integer,
		bool ForceUnsigned = false
		)
	{
		SetConstInt32 (Integer, GetUInt32TypeKind (Integer, ForceUnsigned));
	}

	void
	SetConstInt64 (
		int64_t Integer,
		CType* pType
		)
	{
		CreateConst (pType, &Integer);
	}

	void
	SetConstInt64 (
		int64_t Integer,
		EType TypeKind
		);

	void
	SetConstInt64 (int64_t Integer)
	{
		SetConstInt64 (Integer, GetInt64TypeKind (Integer));
	}

	void
	SetConstUInt64 (
		uint64_t Integer,
		bool ForceUnsigned = false
		)
	{
		SetConstInt64 (Integer, GetUInt64TypeKind (Integer, ForceUnsigned));
	}

	void
	SetConstFloat (float Float);

	void
	SetConstDouble (double Double);

	void
	SetLiteralA (
		const char* p,
		size_t Length = -1
		);

	void
	SetLiteralW (
		const wchar_t* p,
		size_t Length = -1
		);

	void
	SetLiteral (
		const tchar_t* p,
		size_t Length = -1
		)
	{
#ifdef _UNICODE
		SetLiteralW (p, Length);
#else
		SetLiteralA (p, Length);
#endif
	}

	void
	SetLlvmRegister (
		llvm::Value* pValue,
		CType* pType
		);

	void
	SetLlvmRegister (
		llvm::Value* pValue,
		EType TypeKind
		);
};

//.............................................................................

struct TFatPointer
{
	void* m_p;
	void* m_pParent;
	CType* m_pType;
	CType* m_pParentType;
};

//.............................................................................

} // namespace axl {
} // namespace jnc {

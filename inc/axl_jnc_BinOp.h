// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2012. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#include "axl_jnc_UnOp.h"

namespace axl {
namespace jnc {

//.............................................................................

enum EBinOp
{
	EBinOp_Undefined = 0,
	
	// arithmetic
	
	EBinOp_Add,
	EBinOp_Sub,
	EBinOp_Mul,
	EBinOp_Div,
	EBinOp_Mod,	
	EBinOp_Shl,
	EBinOp_Shr,	
	EBinOp_BwAnd,
	EBinOp_BwXor,	
	EBinOp_BwOr,

	// comparison

	EBinOp_Eq,
	EBinOp_Ne,
	EBinOp_Lt,
	EBinOp_Le,
	EBinOp_Gt,
	EBinOp_Ge,

	// assignment

	EBinOp_Assign,
	EBinOp_AddAssign,
	EBinOp_SubAssign,
	EBinOp_MulAssign,
	EBinOp_DivAssign,
	EBinOp_ModAssign,
	EBinOp_ShlAssign,
	EBinOp_ShrAssign,
	EBinOp_AndAssign,
	EBinOp_XorAssign,
	EBinOp_OrAssign,
	EBinOp_AtAssign,
	EBinOp_ClassAssign,

	// logic

	EBinOp_LogAnd,
	EBinOp_LogOr,

	// other

	EBinOp_Idx,
	EBinOp_At,

	EBinOp__Count,
};

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

const tchar_t*
GetBinOpKindString (EBinOp OpKind);

//.............................................................................

[uuid ("e205fc7b-e75e-4cc6-88f7-336701c9645a")]
struct IBinaryOperator: obj::IRoot
{	
protected:
	CModule* m_pModule;
	EBinOp m_OpKind;
	int m_OpFlags1;
	int m_OpFlags2;

public:
	IBinaryOperator ();

	CModule*
	GetModule ()
	{
		return m_pModule;
	}

	EBinOp 
	GetOpKind ()
	{
		return m_OpKind;
	}

	int
	GetOpFlags1 ()
	{
		return m_OpFlags1;
	}

	int
	GetOpFlags2 ()
	{
		return m_OpFlags2;
	}

	virtual
	bool
	Operator (
		const CValue& OpValue1,
		const CValue& OpValue2,
		CValue* pResultValue
		) = 0;

	err::CError
	SetOperatorError (		
		CType* pOpType1,
		CType* pOpType2
		)
	{
		return err::SetFormatStringError (
			_T("binary '%s' cannot be applied to '%s' and '%s'"),
			GetBinOpKindString (m_OpKind),
			pOpType1->GetTypeString (),
			pOpType2->GetTypeString ()
			);
	}
};

//.............................................................................

class CBinOp_Idx: public IBinaryOperator
{
public:
	AXL_OBJ_SIMPLE_CLASS (CBinOp_Idx, IBinaryOperator)

public:
	CBinOp_Idx ()
	{
		m_OpKind = EBinOp_Ge;
		m_OpFlags1 = EOpFlag_KeepPropertyRef; // indexing properties is creating a closure
	}

	virtual
	bool
	Operator (
		const CValue& RawOpValue1,
		const CValue& RawOpValue2,
		CValue* pResultValue
		);

protected:
	bool
	ArrayIndexOperator (
		const CValue& RawOpValue1,
		CArrayType* pArrayType,
		const CValue& RawOpValue2,
		CValue* pResultValue
		);

	bool
	PropertyIndexOperator (
		const CValue& RawOpValue1,
		const CValue& RawOpValue2,
		CValue* pResultValue
		);
};

//.............................................................................

} // namespace jnc {
} // namespace axl {

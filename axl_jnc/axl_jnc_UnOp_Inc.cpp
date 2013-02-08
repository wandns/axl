#include "stdafx.h"
#include "axl_jnc_UnOp_Inc.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

bool
CUnOp_PreInc::Operator (
	const CValue& OpValue,
	CValue* pResultValue
	)
{
	CValue OneValue;
	OneValue.SetConstInt32 (1);
	EBinOp BinOpKind = m_OpKind == EUnOp_PreInc ? EBinOp_AddAssign : EBinOp_SubAssign;
		
	bool Result = m_pModule->m_OperatorMgr.BinaryOperator (BinOpKind, OpValue, OneValue);
	if (!Result)
		return false;

	*pResultValue = OpValue;
	return true;
}

//.............................................................................
	
bool
CUnOp_PostInc::Operator (
	const CValue& OpValue,
	CValue* pResultValue
	)
{
	bool Result;

	CValue OldValue;
	Result = m_pModule->m_OperatorMgr.PrepareOperand (OpValue, &OldValue);
	if (!Result)
		return false;

	CValue OneValue;
	OneValue.SetConstInt32 (1);
	EBinOp BinOpKind = m_OpKind == EUnOp_PostInc ? EBinOp_AddAssign : EBinOp_SubAssign;
		
	Result = m_pModule->m_OperatorMgr.BinaryOperator (BinOpKind, OpValue, OneValue);
	if (!Result)
		return false;

	*pResultValue = OldValue;
	return true;
}

//.............................................................................

} // namespace jnc {
} // namespace axl {
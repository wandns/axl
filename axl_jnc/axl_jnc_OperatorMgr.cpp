#include "stdafx.h"
#include "axl_jnc_OperatorMgr.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

COperatorMgr::COperatorMgr ()
{
	m_pModule = GetCurrentThreadModule ();
	ASSERT (m_pModule);
}

void
COperatorMgr::Clear ()
{
	for (size_t i = 0; i < EUnOp__Count; i++)
		m_UnaryOperatorTable [i].Clear ();

	for (size_t i = 0; i < EBinOp__Count; i++)
		m_BinaryOperatorTable [i].Clear ();

	memset (m_BasicCastOperatorTable, 0, sizeof (m_BasicCastOperatorTable));
	m_CastOperatorMap.Clear ();
	m_SuperCastList.Clear ();
}

void
COperatorMgr::AddStdOperators ()
{
	AddStdCastOperators ();
	AddStdUnaryOperators ();
	AddStdBinaryOperators ();
}

void
COperatorMgr::AddStdUnaryOperators ()
{
	AddUnaryOperator (EUnOp_Minus, EType_Int32, &m_UnOp_Minus_i32);
	AddUnaryOperator (EUnOp_Minus, EType_Int64, &m_UnOp_Minus_i64);
	AddUnaryOperator (EUnOp_Minus, EType_Float, &m_UnOp_Minus_f32);
	AddUnaryOperator (EUnOp_Minus, EType_Double, &m_UnOp_Minus_f64);

	AddUnaryOperator (EUnOp_BitwiseNot, EType_Int32, &m_UnOp_BitwiseNot_i32);
	AddUnaryOperator (EUnOp_BitwiseNot, EType_Int64, &m_UnOp_BitwiseNot_i64);
}

void
COperatorMgr::AddStdBinaryOperators ()
{
	AddBinaryOperator (EBinOp_Add, EType_Int32, &m_BinOp_Add_i32);
	AddBinaryOperator (EBinOp_Add, EType_Int64, &m_BinOp_Add_i64);
	AddBinaryOperator (EBinOp_Add, EType_Float, &m_BinOp_Add_f32);
	AddBinaryOperator (EBinOp_Add, EType_Double, &m_BinOp_Add_f64);

	AddBinaryOperator (EBinOp_Sub, EType_Int32, &m_BinOp_Sub_i32);
	AddBinaryOperator (EBinOp_Sub, EType_Int64, &m_BinOp_Sub_i64);
	AddBinaryOperator (EBinOp_Sub, EType_Float, &m_BinOp_Sub_f32);
	AddBinaryOperator (EBinOp_Sub, EType_Double, &m_BinOp_Sub_f64);

	AddBinaryOperator (EBinOp_Mul, EType_Int32, &m_BinOp_Mul_i32);
	AddBinaryOperator (EBinOp_Mul, EType_Int64, &m_BinOp_Mul_i64);
	AddBinaryOperator (EBinOp_Mul, EType_Float, &m_BinOp_Mul_f32);
	AddBinaryOperator (EBinOp_Mul, EType_Double, &m_BinOp_Mul_f64);

	AddBinaryOperator (EBinOp_Div, EType_Int32, &m_BinOp_Div_i32);
	AddBinaryOperator (EBinOp_Div, EType_Int64, &m_BinOp_Div_i64);
	AddBinaryOperator (EBinOp_Div, EType_Float, &m_BinOp_Div_f32);
	AddBinaryOperator (EBinOp_Div, EType_Double, &m_BinOp_Div_f64);
	AddBinaryOperator (EBinOp_Div, EType_Int32_u, &m_BinOp_Div_i32u);
	AddBinaryOperator (EBinOp_Div, EType_Int64_u, &m_BinOp_Div_i64u);

	AddBinaryOperator (EBinOp_Mod, EType_Int32, &m_BinOp_Mod_i32);
	AddBinaryOperator (EBinOp_Mod, EType_Int64, &m_BinOp_Mod_i64);
	AddBinaryOperator (EBinOp_Mod, EType_Int32_u, &m_BinOp_Mod_i32u);
	AddBinaryOperator (EBinOp_Mod, EType_Int64_u, &m_BinOp_Mod_i64u);

	AddBinaryOperator (EBinOp_Shl, EType_Int32, &m_BinOp_Shl_i32);
	AddBinaryOperator (EBinOp_Shl, EType_Int64, &m_BinOp_Shl_i64);

	AddBinaryOperator (EBinOp_Shr, EType_Int32, &m_BinOp_Shr_i32);
	AddBinaryOperator (EBinOp_Shr, EType_Int64, &m_BinOp_Shr_i64);

	AddBinaryOperator (EBinOp_BitwiseAnd, EType_Int32, &m_BinOp_BitwiseAnd_i32);
	AddBinaryOperator (EBinOp_BitwiseAnd, EType_Int64, &m_BinOp_BitwiseAnd_i64);

	AddBinaryOperator (EBinOp_BitwiseOr, EType_Int32, &m_BinOp_BitwiseOr_i32);
	AddBinaryOperator (EBinOp_BitwiseOr, EType_Int64, &m_BinOp_BitwiseOr_i64);

	AddBinaryOperator (EBinOp_BitwiseXor, EType_Int32, &m_BinOp_BitwiseXor_i32);
	AddBinaryOperator (EBinOp_BitwiseXor, EType_Int64, &m_BinOp_BitwiseXor_i64);

	AddBinaryOperator (EBinOp_Eq, EType_Int32, &m_BinOp_Eq_i32);
	AddBinaryOperator (EBinOp_Eq, EType_Int64, &m_BinOp_Eq_i64);
	AddBinaryOperator (EBinOp_Eq, EType_Float, &m_BinOp_Eq_f32);
	AddBinaryOperator (EBinOp_Eq, EType_Double, &m_BinOp_Eq_f64);

	AddBinaryOperator (EBinOp_Ne, EType_Int32, &m_BinOp_Ne_i32);
	AddBinaryOperator (EBinOp_Ne, EType_Int64, &m_BinOp_Ne_i64);
	AddBinaryOperator (EBinOp_Ne, EType_Float, &m_BinOp_Ne_f32);
	AddBinaryOperator (EBinOp_Ne, EType_Double, &m_BinOp_Ne_f64);

	AddBinaryOperator (EBinOp_Lt, EType_Int32, &m_BinOp_Lt_i32);
	AddBinaryOperator (EBinOp_Lt, EType_Int64, &m_BinOp_Lt_i64);
	AddBinaryOperator (EBinOp_Lt, EType_Float, &m_BinOp_Lt_f32);
	AddBinaryOperator (EBinOp_Lt, EType_Double, &m_BinOp_Lt_f64);
	AddBinaryOperator (EBinOp_Lt, EType_Int32_u, &m_BinOp_Lt_i32u);
	AddBinaryOperator (EBinOp_Lt, EType_Int64_u, &m_BinOp_Lt_i64u);

	AddBinaryOperator (EBinOp_Le, EType_Int32, &m_BinOp_Le_i32);
	AddBinaryOperator (EBinOp_Le, EType_Int64, &m_BinOp_Le_i64);
	AddBinaryOperator (EBinOp_Le, EType_Float, &m_BinOp_Le_f32);
	AddBinaryOperator (EBinOp_Le, EType_Double, &m_BinOp_Le_f64);
	AddBinaryOperator (EBinOp_Le, EType_Int32_u, &m_BinOp_Le_i32u);
	AddBinaryOperator (EBinOp_Le, EType_Int64_u, &m_BinOp_Le_i64u);

	AddBinaryOperator (EBinOp_Gt, EType_Int32, &m_BinOp_Gt_i32);
	AddBinaryOperator (EBinOp_Gt, EType_Int64, &m_BinOp_Gt_i64);
	AddBinaryOperator (EBinOp_Gt, EType_Float, &m_BinOp_Gt_f32);
	AddBinaryOperator (EBinOp_Gt, EType_Double, &m_BinOp_Gt_f64);
	AddBinaryOperator (EBinOp_Gt, EType_Int32_u, &m_BinOp_Gt_i32u);
	AddBinaryOperator (EBinOp_Gt, EType_Int64_u, &m_BinOp_Gt_i64u);

	AddBinaryOperator (EBinOp_Ge, EType_Int32, &m_BinOp_Ge_i32);
	AddBinaryOperator (EBinOp_Ge, EType_Int64, &m_BinOp_Ge_i64);
	AddBinaryOperator (EBinOp_Ge, EType_Float, &m_BinOp_Ge_f32);
	AddBinaryOperator (EBinOp_Ge, EType_Double, &m_BinOp_Ge_f64);
	AddBinaryOperator (EBinOp_Ge, EType_Int32_u, &m_BinOp_Ge_i32u);
	AddBinaryOperator (EBinOp_Ge, EType_Int64_u, &m_BinOp_Ge_i64u);
}

void
COperatorMgr::AddStdCastOperators ()
{
	// integer copies

	AddCastOperator (EType_Int8, EType_Int8, &m_Cast_cpy);
	AddCastOperator (EType_Int8, EType_Int8_u, &m_Cast_cpy);
	AddCastOperator (EType_Int8_u, EType_Int8, &m_Cast_cpy);
	AddCastOperator (EType_Int8_u, EType_Int8_u, &m_Cast_cpy);
	AddCastOperator (EType_Int16, EType_Int16, &m_Cast_cpy);
	AddCastOperator (EType_Int16, EType_Int16_u, &m_Cast_cpy);
	AddCastOperator (EType_Int16_u, EType_Int16, &m_Cast_cpy);
	AddCastOperator (EType_Int16_u, EType_Int16_u, &m_Cast_cpy);
	AddCastOperator (EType_Int32, EType_Int32, &m_Cast_cpy);
	AddCastOperator (EType_Int32, EType_Int32_u, &m_Cast_cpy);
	AddCastOperator (EType_Int32_u, EType_Int32, &m_Cast_cpy);
	AddCastOperator (EType_Int32_u, EType_Int32_u, &m_Cast_cpy);
	AddCastOperator (EType_Int64, EType_Int64, &m_Cast_cpy);
	AddCastOperator (EType_Int64, EType_Int64_u, &m_Cast_cpy);
	AddCastOperator (EType_Int64_u, EType_Int64, &m_Cast_cpy);
	AddCastOperator (EType_Int64_u, EType_Int64_u, &m_Cast_cpy);

	AddCastOperator (EType_Int16_be, EType_Int16_be, &m_Cast_cpy);
	AddCastOperator (EType_Int16_be, EType_Int16_beu, &m_Cast_cpy);
	AddCastOperator (EType_Int16_beu, EType_Int16_be, &m_Cast_cpy);
	AddCastOperator (EType_Int16_beu, EType_Int16_beu, &m_Cast_cpy);
	AddCastOperator (EType_Int32_be, EType_Int32_be, &m_Cast_cpy);
	AddCastOperator (EType_Int32_be, EType_Int32_beu, &m_Cast_cpy);
	AddCastOperator (EType_Int32_beu, EType_Int32_be, &m_Cast_cpy);
	AddCastOperator (EType_Int32_beu, EType_Int32_beu, &m_Cast_cpy);
	AddCastOperator (EType_Int64_be, EType_Int64_be, &m_Cast_cpy);
	AddCastOperator (EType_Int64_be, EType_Int64_beu, &m_Cast_cpy);
	AddCastOperator (EType_Int64_beu, EType_Int64_be, &m_Cast_cpy);
	AddCastOperator (EType_Int64_beu, EType_Int64_beu, &m_Cast_cpy);

	// endianness swaps

	AddCastOperator (EType_Int16, EType_Int16_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int16, EType_Int16_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_u, EType_Int16_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_u, EType_Int16_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_be, EType_Int16, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_be, EType_Int16_u, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_beu, EType_Int16, &m_Cast_int_swp);
	AddCastOperator (EType_Int16_beu, EType_Int16_u, &m_Cast_int_swp);

	AddCastOperator (EType_Int32, EType_Int32_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int32, EType_Int32_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_u, EType_Int32_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_u, EType_Int32_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_be, EType_Int32, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_be, EType_Int32_u, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_beu, EType_Int32, &m_Cast_int_swp);
	AddCastOperator (EType_Int32_beu, EType_Int32_u, &m_Cast_int_swp);

	AddCastOperator (EType_Int64, EType_Int64_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int64, EType_Int64_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_u, EType_Int64_be, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_u, EType_Int64_beu, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_be, EType_Int64, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_be, EType_Int64_u, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_beu, EType_Int64, &m_Cast_int_swp);
	AddCastOperator (EType_Int64_beu, EType_Int64_u, &m_Cast_int_swp);

	// integer truncations

	AddCastOperator (EType_Int16, EType_Int8, &m_Cast_int_trunc);
	AddCastOperator (EType_Int16, EType_Int8_u, &m_Cast_int_trunc);
	AddCastOperator (EType_Int32, EType_Int8, &m_Cast_int_trunc);
	AddCastOperator (EType_Int32, EType_Int8_u, &m_Cast_int_trunc);
	AddCastOperator (EType_Int32, EType_Int16, &m_Cast_int_trunc);
	AddCastOperator (EType_Int32, EType_Int16_u, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int8, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int8_u, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int16, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int16_u, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int32, &m_Cast_int_trunc);
	AddCastOperator (EType_Int64, EType_Int32_u, &m_Cast_int_trunc);

	// integer extensions

	AddCastOperator (EType_Int8, EType_Int16, &m_Cast_int_ext);
	AddCastOperator (EType_Int8, EType_Int32, &m_Cast_int_ext);
	AddCastOperator (EType_Int8, EType_Int64, &m_Cast_int_ext);
	AddCastOperator (EType_Int8_u, EType_Int16, &m_Cast_int_ext_u);
	AddCastOperator (EType_Int8_u, EType_Int32, &m_Cast_int_ext_u);
	AddCastOperator (EType_Int8_u, EType_Int64, &m_Cast_int_ext_u);
	AddCastOperator (EType_Int16, EType_Int32, &m_Cast_int_ext);
	AddCastOperator (EType_Int16, EType_Int64, &m_Cast_int_ext);
	AddCastOperator (EType_Int16_u, EType_Int32, &m_Cast_int_ext_u);
	AddCastOperator (EType_Int16_u, EType_Int64, &m_Cast_int_ext_u);
	AddCastOperator (EType_Int32, EType_Int64, &m_Cast_int_ext);
	AddCastOperator (EType_Int32_u, EType_Int64, &m_Cast_int_ext_u);

	// floating extensions & truncations

	AddCastOperator (EType_Float, EType_Float, &m_Cast_cpy);
	AddCastOperator (EType_Double, EType_Double, &m_Cast_cpy);
	AddCastOperator (EType_Float, EType_Double, &m_Cast_f32_f64);
	AddCastOperator (EType_Double, EType_Float, &m_Cast_f64_f32);

	// integer to floating point

	AddCastOperator (EType_Int32, EType_Float, &m_Cast_i32_f32);
	AddCastOperator (EType_Int32_u, EType_Float, &m_Cast_i32u_f32);
	AddCastOperator (EType_Int32, EType_Double, &m_Cast_i32_f64);
	AddCastOperator (EType_Int32_u, EType_Double, &m_Cast_i32u_f64);
	AddCastOperator (EType_Int64, EType_Float, &m_Cast_i64_f32);
	AddCastOperator (EType_Int64_u, EType_Float, &m_Cast_i64u_f32);
	AddCastOperator (EType_Int64, EType_Double, &m_Cast_i64_f64);
	AddCastOperator (EType_Int64_u, EType_Double, &m_Cast_i64u_f64);

	// floating point to integer

	AddCastOperator (EType_Float, EType_Int32, &m_Cast_f32_i32);
	AddCastOperator (EType_Double, EType_Int32, &m_Cast_f64_i32);
	AddCastOperator (EType_Float, EType_Int64, &m_Cast_f32_i64);
	AddCastOperator (EType_Double, EType_Int64, &m_Cast_f64_i64);

	// build super moves for basic types (floyd-warshall)
	
	for (size_t k = 0; k < EType__BasicTypeCount; k++)
	for (size_t i = 0; i < EType__BasicTypeCount; i++)
	for (size_t j = 0; j < EType__BasicTypeCount; j++)
	{
		ICastOperator* pOperatorIK = m_BasicCastOperatorTable [i] [k];
		ICastOperator* pOperatorKJ = m_BasicCastOperatorTable [k] [j];
		if (!pOperatorIK || !pOperatorKJ)
			continue;

		size_t SuperPrice = pOperatorIK->GetPrice () + pOperatorKJ->GetPrice ();

		ICastOperator* pOperatorIJ = m_BasicCastOperatorTable [i] [j];
		if (pOperatorIJ && pOperatorIJ->GetPrice () <= SuperPrice)
			continue;

		CSuperCast* pSuperCast = AXL_MEM_NEW (CSuperCast);
		pSuperCast->m_pIntermediateType = m_pModule->m_TypeMgr.GetBasicType ((EType) k);
		pSuperCast->m_pFirst = pOperatorIK;
		pSuperCast->m_pSecond = pOperatorKJ;
		pSuperCast->m_Price = SuperPrice;
		m_SuperCastList.InsertTail (pSuperCast);

		pOperatorIJ = AddCastOperator ((EType) i, (EType) j, pSuperCast);
	}

	// integer to bool point (after building super moves!)

	for (size_t i = EType_Int8; i < EType_Int64_u; i++)
	{
		AddCastOperator ((EType) i, EType_Bool, &m_Cast_num_bool);	
		AddCastOperator (EType_Bool, (EType) i, &m_Cast_bool_int);	
	}

	for (size_t i = EType_Int16_be; i < EType_Int64_beu; i++)
	{
		AddCastOperator ((EType) i, EType_Bool, &m_Cast_num_bool);	
	}

	AddCastOperator (EType_Float, EType_Bool, &m_Cast_num_bool);	
	AddCastOperator (EType_Double, EType_Bool, &m_Cast_num_bool);	
}

IUnaryOperator*
COperatorMgr::GetUnaryOperator (
	EUnOp OpKind,
	CType* pOpType,
	TUnaryOperatorTypeInfo* pTypeInfo
	)
{
	ASSERT (OpKind > 0 && OpKind < EUnOp__Count);
	IUnaryOperator* pOperator = m_UnaryOperatorTable [OpKind].GetOperator (pOpType, pTypeInfo);
	if (pOperator)
		return pOperator;

	switch (OpKind)
	{
	case EUnOp_Addr:
		pOperator = &m_UnOp_addr;
		break;

	case EUnOp_Indir:
		pOperator = &m_UnOp_indir;
		break;

	case EUnOp_PreInc:
		pOperator = &m_UnOp_preinc;
		break;

	case EUnOp_PreDec:
		pOperator = &m_UnOp_predec;
		break;

	case EUnOp_PostInc:
		pOperator = &m_UnOp_postinc;
		break;

	case EUnOp_PostDec:
		pOperator = &m_UnOp_postdec;
		break;
	}

	if (!pOperator)
		return NULL;

	bool Result = pOperator->GetTypeInfo (pOpType, pTypeInfo);
	return Result ? pOperator : NULL;
}

IUnaryOperator*
COperatorMgr::AddUnaryOperator (
	EUnOp OpKind,
	EType OpTypeKind,
	IUnaryOperator* pOperator
	)
{
	CType* pOpType = m_pModule->m_TypeMgr.GetBasicType (OpTypeKind);
	return AddUnaryOperator (OpKind, pOpType, pOperator);
}

bool
COperatorMgr::UnaryOperator (
	EUnOp OpKind,
	const CValue& RawOpValue,
	CValue* pResultValue
	)
{
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;

	CType* pOpType = OpValue.GetType ();

	TUnaryOperatorTypeInfo TypeInfo;
	IUnaryOperator* pOperator = GetUnaryOperator (OpKind, pOpType, &TypeInfo);	
	if (!pOperator)
	{
		err::SetFormatStringError (
			_T("unary '%s' cannot be applied to '%s'"),
			GetUnOpString (OpKind),
			pOpType->GetTypeString ()
			);
		return false;
	}

	CValue CastOpValue;
	return 
		CastOperator (OpValue, TypeInfo.m_pOpType, &CastOpValue) &&
		pOperator->Operator (CastOpValue, pResultValue);
}

bool
COperatorMgr::UnaryOperator (
	EUnOp OpKind,
	CValue* pValue
	)
{
	CValue ResultValue;

	bool Result = UnaryOperator (OpKind, *pValue, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

IBinaryOperator*
COperatorMgr::AddBinaryOperator (
	EBinOp OpKind,
	EType OpTypeKind1,
	EType OpTypeKind2,
	IBinaryOperator* pOperator
	)
{
	CType* pOpType1 = m_pModule->m_TypeMgr.GetBasicType (OpTypeKind1);
	CType* pOpType2 = m_pModule->m_TypeMgr.GetBasicType (OpTypeKind2);
	return AddBinaryOperator (OpKind, pOpType1, pOpType2, pOperator);
}

bool
COperatorMgr::BinaryOperator (
	EBinOp OpKind,
	const CValue& RawOpValue1,
	const CValue& RawOpValue2,
	CValue* pResultValue
	)
{
	CValue OpValue1;
	CValue OpValue2;

	bool Result = 
		PrepareOperand (RawOpValue1, &OpValue1) &&
		PrepareOperand (RawOpValue2, &OpValue2);
	
	if (!Result)
		return false;
	
	CType* pOpType1 = OpValue1.GetType ();
	CType* pOpType2 = OpValue2.GetType ();

	// temporary implementation of pointer arithmetics -- just for testing

	if (pOpType1->IsReferenceType () && ((CPointerType*) pOpType1)->GetBaseType ()->IsPointerType ())
	{
		Result = LoadReferenceOperator (&OpValue1);
		if (!Result)
			return false;

		pOpType1 = OpValue1.GetType ();

		if (pOpType2->IsReferenceType ())
		{
			Result = LoadReferenceOperator (&OpValue2);
			if (!Result)
				return false;

			pOpType2 = OpValue2.GetType ();
		}

		if (pOpType2->IsIntegerType ())
		{
			if (pOpType1->GetTypeKind () == EType_Pointer)
			{
				CValue Size;
				Size.SetConstSizeT (((CPointerType*) pOpType1)->GetBaseType ()->GetSize ());

				if (OpKind == EBinOp_Sub)
					UnaryOperator (EUnOp_Minus, &Size);

				Result = BinaryOperator (EBinOp_Mul, &OpValue2, Size);
				if (!Result)
					return false;

				llvm::Value* pPtr = m_pModule->m_LlvmBuilder.CreateExtractValue (OpValue1.GetLlvmValue (), 0, "sp_ptr");
				pPtr = m_pModule->m_LlvmBuilder.CreateGEP (pPtr, OpValue2.GetLlvmValue (), "sp_ptr_inc");
				llvm::Value* pSafePtr = m_pModule->m_LlvmBuilder.CreateInsertValue (OpValue1.GetLlvmValue (), pPtr, 0, "sp_ptr");
				pResultValue->SetLlvmRegister (pSafePtr, pOpType1);
			}
			else 
			{
				err::SetFormatStringError ("not yet");
				return false;
			}
			
			return true;
		}
	}

	TBinaryOperatorTypeInfo TypeInfo;
	IBinaryOperator* pOperator = GetBinaryOperator (OpKind, pOpType1, pOpType2, &TypeInfo);	
	if (!pOperator)
	{
		err::SetFormatStringError (
			_T("binary '%s' cannot be applied to '%s' and '%s'"),
			GetBinOpString (OpKind),
			pOpType1->GetTypeString (),
			pOpType2->GetTypeString ()
			);
		return false;
	}
	
	CValue CastOpValue1;
	CValue CastOpValue2;

	return
		CastOperator (OpValue1, TypeInfo.m_pOpType1, &CastOpValue1) &&
		CastOperator (OpValue2, TypeInfo.m_pOpType2, &CastOpValue2) &&
		pOperator->Operator (CastOpValue1, CastOpValue2, pResultValue);
}

bool
COperatorMgr::BinaryOperator (
	EBinOp OpKind,
	CValue* pValue,
	const CValue& OpValue2
	)
{
	CValue ResultValue;

	bool Result = BinaryOperator (OpKind, *pValue, OpValue2, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

ICastOperator*
COperatorMgr::GetCastOperator (
	CType* pSrcType,
	CType* pDstType
	)
{
	if (pSrcType->Cmp (pDstType) == 0)
		return &m_Cast_cpy;

	rtl::CStringA Signature = pSrcType->GetSignature () + pDstType->GetSignature ();
	rtl::CHashTableMapIteratorT <const tchar_t*, ICastOperator*> It = m_CastOperatorMap.Find (Signature);
	if (It)
		return It->m_Value;

	return NULL;
}

ICastOperator*
COperatorMgr::AddCastOperator (
	CType* pSrcType,
	CType* pDstType,
	ICastOperator* pOperator
	)
{
	rtl::CStringA Signature = pSrcType->GetSignature () + pDstType->GetSignature ();
	rtl::CHashTableMapIteratorT <const tchar_t*, ICastOperator*> It = m_CastOperatorMap.Goto (Signature);

	ICastOperator* pPrevOperator = It->m_Value;
	if (!pPrevOperator)
		m_CastSignatureCache.InsertTail (Signature);

	EType SrcTypeKind = pSrcType->GetTypeKind ();
	EType DstTypeKind = pDstType->GetTypeKind ();

	if (SrcTypeKind < EType__BasicTypeCount && DstTypeKind < EType__BasicTypeCount)
		m_BasicCastOperatorTable [SrcTypeKind] [DstTypeKind] = pOperator;

	It->m_Value = pOperator;
	return pPrevOperator;
}

ICastOperator*
COperatorMgr::AddCastOperator (
	EType SrcTypeKind,
	EType DstTypeKind,
	ICastOperator* pOperator
	)
{
	CType* pSrcType = m_pModule->m_TypeMgr.GetBasicType (SrcTypeKind);
	CType* pDstType = m_pModule->m_TypeMgr.GetBasicType (DstTypeKind);
	return AddCastOperator (pSrcType, pDstType, pOperator);
}

ECast
COperatorMgr::GetCastKind (
	CType* pSrcType,
	CType* pDstType
	)
{
	pSrcType = PrepareOperandType (pSrcType);
	pDstType = PrepareOperandType (pDstType);

	if (pSrcType->Cmp (pDstType) == 0)
		return ECast_Implicit;

	if (pSrcType->IsReferenceType ())
	{
		pSrcType = ((CPointerType*) pSrcType)->GetBaseType ();

		if (pSrcType->Cmp (pDstType) == 0)
			return ECast_Implicit;
	}
	
	if (pSrcType->GetTypeKind () == EType_Array && pDstType->IsPointerType ())
		return ECast_Implicit;

	ICastOperator* pOperator = GetCastOperator (pSrcType, pDstType);
	return pOperator ? pOperator->GetCastKind (pSrcType, pDstType) : ECast_None;
}

bool
COperatorMgr::CastOperator (
	const CValue& RawOpValue,
	CType* pType,
	CValue* pResultValue
	)
{
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;
	
	CType* pOpType = OpValue.GetType ();
	if (pOpType->Cmp (pType) == 0)
	{
		*pResultValue = OpValue;
		return true;
	}

	if (pOpType->IsReferenceType ())
	{
		CType* pTargetType = ((CPointerType*) pOpType)->GetBaseType ();
		if (pTargetType->GetTypeKind () == EType_Array && pType->IsPointerType ())
			return CastArrayReferenceToPointerOperator (
				OpValue, 
				(CArrayType*) pTargetType,
				(CPointerType*) pType,
				pResultValue
				);

		Result = LoadReferenceOperator (&OpValue);
		if (!Result)
			return false;

		CType* pOpType = OpValue.GetType ();
		if (pOpType->Cmp (pType) == 0)
		{
			*pResultValue = OpValue;
			return true;
		}
	}

	if (pType->IsPointerType ())
	{
		if (OpValue.GetValueKind () == EValue_Null) 
			return pResultValue->CreateConst (
				pType, 
				NULL
				);
		else if (pOpType->IsPointerType ())
			return CastPointerOperator (
				OpValue, 
				(CPointerType*) pOpType,
				(CPointerType*) pType,
				pResultValue
				);
		else if (pOpType->GetTypeKind () == EType_Array)
			return CastArrayToPointerOperator (
				OpValue, 
				(CArrayType*) pOpType,
				(CPointerType*) pType,
				pResultValue
				);
	}

	ICastOperator* pOperator = GetCastOperator (pOpType, pType);	
	if (!pOperator)
	{
		err::SetFormatStringError (
			_T("cannot convert from '%s' to '%s'"),
			pOpType->GetTypeString (),
			pType->GetTypeString ()
			);
		return false;
	}

	return pOperator->Cast (OpValue, pType, pResultValue);
}

bool
COperatorMgr::CastOperator (
	CValue* pValue,
	CType* pType
	)
{
	CValue ResultValue;

	bool Result = CastOperator (*pValue, pType, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

bool
COperatorMgr::CastOperator (
	const CValue& OpValue,
	EType TypeKind,
	CValue* pResultValue
	)
{
	CType* pType = m_pModule->m_TypeMgr.GetBasicType (TypeKind);
	return CastOperator (OpValue, pType, pResultValue);
}

bool
COperatorMgr::CastOperator (
	CValue* pValue,
	EType TypeKind
	)
{
	CType* pType = m_pModule->m_TypeMgr.GetBasicType (TypeKind);
	return CastOperator (pValue, pType);
}

bool
COperatorMgr::CastPointerOperator (
	const CValue& OpValue,
	CPointerType* pSrcType,
	CPointerType* pDstType,
	CValue* pResultValue
	)
{
	err::SetFormatStringError (_T("conversion of pointers is not implemented yet"));
	return true;
}

bool
COperatorMgr::CastArrayReferenceToPointerOperator (
	const CValue& OpValue,
	CArrayType* pArrayType,
	CPointerType* pPointerType,
	CValue* pResultValue
	)
{
/*
	CValue Zero;
	Zero.SetConstInt32 (0, EType_Int32);
	
	llvm::Value* pLlvmValue = Value.GetLlvmValue ();
	llvm::Value* pLlvmZero = Zero.GetLlvmValue ();

	llvm::Value* LlvmIndexArray [] =
	{
		pLlvmZero,
		pLlvmZero,
	};

	llvm::Value* pLlvmGep = m_pModule->m_LlvmBuilder.CreateGEP (
		pLlvmValue, 
		llvm::ArrayRef <llvm::Value*> (LlvmIndexArray, 2)
		);
	pResultValue->SetLlvmRegister (pLlvmGep, pType);
	return true;
*/
	err::SetFormatStringError (_T("conversion of array reference to pointer is not yet implemented"));
	return false;
}

bool
COperatorMgr::CastArrayToPointerOperator (
	const CValue& OpValue,
	CArrayType* pArrayType,
	CPointerType* pPointerType,
	CValue* pResultValue
	)
{
	if (OpValue.GetValueKind () == EValue_Const && pPointerType->GetTypeKind () == EType_Pointer_u)
	{
		const CValue& SavedSrcValue = m_pModule->m_ConstMgr.SaveValue (OpValue);		
		void* p = SavedSrcValue.GetConstData ();
		return pResultValue->CreateConst (pPointerType, &p);
	}

	err::SetFormatStringError (_T("conversion of array to pointer is not yet implemented"));
	return false;
}

bool
COperatorMgr::MoveOperator (
	const CValue& RawOpValue,
	const CValue& DstValue
	)
{
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;

	CType* pDstType = DstValue.GetType ();
	if (pDstType->GetTypeKind () == EType_Property)
		return SetPropertyOperator (OpValue, DstValue.GetProperty ());

	if (!pDstType->IsReferenceType ())
	{
		err::SetFormatStringError (_T("left operand must be l-value"));
		return false;
	}

	return StoreReferenceOperator (OpValue, DstValue);
}

bool
COperatorMgr::MoveOperator (
	const CValue& SrcValue,
	const CValue& DstValue,
	EBinOp OpKind
	)
{
	if (OpKind == EBinOp_None)
		return MoveOperator (SrcValue, DstValue);

	CValue RValue;
	return 
		BinaryOperator (OpKind, DstValue, SrcValue, &RValue) &&
		MoveOperator (RValue, DstValue);
}

bool
COperatorMgr::ConditionalOperator (
	CValue* pOpValue,
	const CValue& TrueValue,
	const CValue& FalseValue
	)
{
	return true;
}

bool
COperatorMgr::MemberOperator (
	const CValue& RawOpValue,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;

	CType* pType = OpValue.GetType ();

	if (pType->IsReferenceType ())
	{
		pType = ((CPointerType*) pType)->GetBaseType ();

		if (pType->IsPointerType ())
		{
			pType = ((CPointerType*) pType)->GetBaseType ();

			Result = UnaryOperator (EUnOp_Indir, &OpValue);
			if (!Result)
				return false;
		}
	}

	EType TypeKind = pType->GetTypeKind ();

	switch (TypeKind)
	{
	case EType_Struct:
	case EType_Union:
		return StructMemberOperator (OpValue, (CStructType*) pType, pName, pResultValue);

	case EType_Interface:
	case EType_Class:
		return ClassMemberOperator (OpValue, (CClassType*) pType, pName, pResultValue);

	default:
		err::SetFormatStringError (_T("member operator cannot be applied to '%s'"), pType->GetTypeString ());
		return false;
	}
}

bool
COperatorMgr::MemberOperator (
	CValue* pValue,
	const tchar_t* pName
	)
{
	CValue ResultValue;

	bool Result = MemberOperator (*pValue, pName, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

bool
COperatorMgr::PointerToMemberOperator (
	const CValue& RawOpValue,
	const tchar_t* pName,
	CValue* pResultValue
	)
{	
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;

	TUnaryOperatorTypeInfo TypeInfo;
	IUnaryOperator* pOperator = GetUnaryOperator (EUnOp_Ptr, OpValue.GetType (), &TypeInfo);
	if (pOperator)
	{
		CValue CastOpValue;
		Result = 
			CastOperator (OpValue, TypeInfo.m_pOpType, &CastOpValue) &&
			pOperator->Operator (CastOpValue, &OpValue) && 
			PrepareOperand (&OpValue);

		if (!Result)
			return false;
	}

	return MemberOperator (OpValue, pName, pResultValue);
}

bool
COperatorMgr::PointerToMemberOperator (
	CValue* pValue,
	const tchar_t* pName
	)
{
	CValue ResultValue;

	bool Result = PointerToMemberOperator (*pValue, pName, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

bool
COperatorMgr::StructMemberOperator (
	const CValue& OpValue,
	CStructType* pStructType,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CStructMember* pMember = pStructType->FindMember (pName);
	if (!pMember)
	{
		err::SetFormatStringError (_T("'%s' is not a member of '%s'"), pName, pStructType->GetTypeString ());
		return false;
	}
	
	EValue OpValueKind = OpValue.GetValueKind ();
	if (OpValueKind == EValue_Const)
	{
		pResultValue->CreateConst (
			pMember->GetType (), 
			(char*) OpValue.GetConstData () + pMember->GetOffset ()
			);

		return true;
	}

	CType* pOpType = OpValue.GetType ();

	if (!pOpType->IsReferenceType ())
	{
		llvm::Value* pLlvmMember = m_pModule->m_LlvmBuilder.CreateExtractValue (OpValue.GetLlvmValue (), pMember->GetLlvmIndex (), "member");
		pResultValue->SetLlvmRegister (pLlvmMember, pMember->GetType ());
		return true;
	}

	if (pOpType->GetTypeKind () == EType_Reference_u)
	{
		llvm::Value* pLlvmGep = CreateLlvmGep (OpValue.GetLlvmValue (), 0, pMember->GetLlvmIndex ());
		CType* pResultType = pMember->GetType ()->GetPointerType (EType_Reference_u);
		pResultValue->SetLlvmRegister (pLlvmGep, pResultType);
		return true;
	}

	if (OpValue.GetValueKind () == EValue_Variable)
	{
		ASSERT (pOpType->GetTypeKind () == EType_Reference);
		llvm::Value* pLlvmGep = CreateLlvmGep (OpValue.GetLlvmValue (), 0, pMember->GetLlvmIndex ());
		pResultValue->SetVariable (OpValue.GetVariable (), pLlvmGep, pMember->GetType ());
		return true;
	}
	else
	{
		llvm::Value* pLlvmFatPtr = OpValue.GetLlvmValue ();		
		llvm::Value* pLlvmPtr = m_pModule->m_LlvmBuilder.CreateExtractValue (pLlvmFatPtr, 0, "sf_ptr");
		pLlvmPtr = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pStructType->GetPointerType (EType_Pointer_u)->GetLlvmType (), "sf_ptr_cast");

		llvm::Value* pLlvmGep = CreateLlvmGep (pLlvmPtr, 0, pMember->GetLlvmIndex ());
		pLlvmFatPtr = ModifyLlvmSafePointer (pLlvmFatPtr, pLlvmGep);
		
		CType* pResultType = pMember->GetType ()->GetPointerType (EType_Reference);
		pResultValue->SetLlvmRegister (pLlvmFatPtr, pResultType);
		return true;
	}
}

bool
COperatorMgr::ClassMemberOperator (
	const CValue& OpValue,
	CClassType* pClassType,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CModuleItem* pClassMember = pClassType->FindMember (pName);
	if (!pClassMember)
	{
		err::SetFormatStringError (_T("'%s' is not a member of '%s'"), pName, pClassType->GetTypeString ());
		return false;
	}

	err::SetFormatStringError (_T("class member operator is not implemented yet"));
	return false;
}

CType*
GetVarArgType (
	CType* pType,
	bool IsUnsafeVarArg
	)
{
	EType TypeKind = pType->GetTypeKind ();

	switch (TypeKind)
	{
	case EType_Reference:
	case EType_Reference_u:
		return ((CPointerType*) pType)->GetBaseType ();

	default:
		return pType;
	}
}

bool
COperatorMgr::CallOperator (
	const CValue& RawOpValue,
	rtl::CBoxListT <CValue>* pArgList,
	CValue* pResultValue
	)
{
	CValue OpValue;
	bool Result = PrepareOperand (RawOpValue, &OpValue);
	if (!Result)
		return false;

	CFunction* pFunction;

	EValue ValueKind = OpValue.GetValueKind ();
	switch (ValueKind)
	{
	case EValue_Function:
		pFunction = OpValue.GetFunction ();
		break;

	case EValue_FunctionOverload:
		// TODO: find overload based on arg list
		pFunction = OpValue.GetFunctionOverload ()->GetFunction ();
		break;

	default:
		err::SetFormatStringError (_T("cannot call %s"), OpValue.GetValueKindString ());
		return false;
	}

	CFunctionType* pFunctionType = pFunction->GetType ();
	llvm::Function* pLlvmFunction = pFunction->GetLlvmFunction ();
	
	size_t FormalArgCount = pFunctionType->GetArgCount ();
	size_t ActualArgCount = pArgList->GetCount ();

	bool IsVarArg = (pFunctionType->GetFlags () & EFunctionTypeFlag_IsVarArg) != 0;
	bool IsUnsafeVarArg = (pFunctionType->GetFlags () & EFunctionTypeFlag_IsUnsafeVarArg) != 0;

	if (IsVarArg && ActualArgCount < FormalArgCount ||
		!IsVarArg && ActualArgCount != FormalArgCount)
	{
		err::SetFormatStringError (_T("function '%s' takes %d arguments; %d passed"), pFunction->GetTag (), FormalArgCount, ActualArgCount);
		return false;
	}

	char LlvmArgBuffer [256];
	rtl::CArrayT <llvm::Value*> LlvmArgArray (ref::EBuf_Stack, LlvmArgBuffer, sizeof (LlvmArgBuffer));
	
	rtl::CBoxIteratorT <CValue> Arg = pArgList->GetHead ();
	for (size_t i = 0; i < FormalArgCount; Arg++, i++)
	{
		CType* pFormalArgType = pFunctionType->GetArgType (i);
		
		CValue ArgCast;
		Result = CastOperator (*Arg, pFormalArgType, &ArgCast);
		if (!Result)
			return false;

		LlvmArgArray.Append (ArgCast.GetLlvmValue ());
	}

	// vararg

	for (; Arg; Arg++)
	{
		CType* pFormalArgType = GetVarArgType (Arg->GetType (), IsUnsafeVarArg);

		CValue ArgCast;
		Result = CastOperator (*Arg, pFormalArgType, &ArgCast);
		if (!Result)
			return false;

		LlvmArgArray.Append (ArgCast.GetLlvmValue ());
	}

	CType* pReturnType = pFunction->GetType ()->GetReturnType ();
	if (pReturnType->GetTypeKind () != EType_Void)
	{
		llvm::Instruction* pLlvmCall = m_pModule->m_LlvmBuilder.CreateCall (
			pLlvmFunction,
			llvm::ArrayRef <llvm::Value*> (LlvmArgArray, LlvmArgArray.GetCount ()),
			"call"
			);
		
		pResultValue->SetLlvmRegister (pLlvmCall, pReturnType);
	}
	else
	{
		m_pModule->m_LlvmBuilder.CreateCall (
			pLlvmFunction,
			llvm::ArrayRef <llvm::Value*> (LlvmArgArray, LlvmArgArray.GetCount ())
			);
		
		pResultValue->SetVoid ();
	}

	return true;
}

bool
COperatorMgr::CallOperator (
	CValue* pValue,
	rtl::CBoxListT <CValue>* pArgList
	)
{
	CValue ResultValue;

	bool Result = CallOperator (*pValue, pArgList, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

CType* 
COperatorMgr::PrepareOperandType (CType* pOpType) 
{
	CType* pType = pOpType;
	
	for (;;)
	{
		CType* pPrevType = pType;

		EType TypeKind = pType->GetTypeKind ();
		switch (TypeKind)
		{			
		case EType_Qualifier:
			pType = ((CQualifierType*) pType)->GetBaseType ();
			break;

		case EType_Reference:
		case EType_Reference_u:
		case EType_Reference_d:
			CType* pBaseType;

			pBaseType = ((CPointerType*) pType)->GetBaseType ();
			if (pBaseType->IsReferenceType ()) // double reference
				pType = pBaseType;

			break;

		case EType_Property:
			pType = ((CPropertyType*) pType)->GetGetterType ()->GetReturnType ();
			break;
		}

		if (pType == pPrevType)
			return pType;
	}
}

bool 
COperatorMgr::PrepareOperand (
	const CValue& OpValue,
	CValue* pOpValue
	) 
{
	bool Result;

	CValue Value = OpValue;

	for (;;)
	{
		CType* pType = Value.GetType ();

		EType TypeKind = pType->GetTypeKind ();
		switch (TypeKind)
		{			
		case EType_Qualifier:
			Value.OverrideType (((CQualifierType*) pType)->GetBaseType ());
			break;

		case EType_Reference:
		case EType_Reference_u:
		case EType_Reference_d:
			CType* pBaseType;

			pBaseType = ((CPointerType*) pType)->GetBaseType ();
			if (pBaseType->IsReferenceType ()) // double reference
			{
				Result = LoadReferenceOperator (&Value);
				if (!Result)
					return false;
			}

			break;

		case EType_Property:
			Result = GetPropertyOperator (Value.GetProperty (), &Value);
			if (!Result)
				return false;

			break;
		}

		if (Value.GetType () == pType)
		{
			*pOpValue = Value;
			return true;
		}
	}
}

bool 
COperatorMgr::PrepareOperand (CValue* pOpValue)
{
	CValue OpValue;

	bool Result = PrepareOperand (*pOpValue, &OpValue);
	if (!Result)
		return false;

	*pOpValue = OpValue;
	return true;
}

bool
COperatorMgr::LoadReferenceOperator (
	const CValue& OpValue,
	CValue* pResultValue
	)
{
	CPointerType* pType = (CPointerType*) OpValue.GetType ();
	ASSERT (pType->IsReferenceType ());

	EType TypeKind = pType->GetTypeKind ();
	if (TypeKind == EType_Reference_d)
	{
		err::SetFormatStringError (_T("loading from a dynamic reference is not supported yet"));
		return false;
	}

	CType* pTargetType = pType->GetBaseType ();

	bool IsVolatile = false;
	if (pTargetType->GetTypeKind () == EType_Qualifier)
	{
		IsVolatile = (pTargetType->GetFlags () & ETypeQualifier_Volatile) != 0;
		pTargetType = ((CDerivedType*) pTargetType)->GetBaseType ();
	}

	llvm::Value* pLlvmValue = OpValue.GetLlvmValue ();

	if (TypeKind == EType_Reference_u || OpValue.GetValueKind () == EValue_Variable)
	{
		llvm::Value* pLlvmLoad = m_pModule->m_LlvmBuilder.CreateLoad (pLlvmValue, IsVolatile, "loa");
		pResultValue->SetLlvmRegister (pLlvmLoad, pTargetType);
		return true;
	}

	CheckLlvmSafePointer (pLlvmValue, pTargetType->GetSize (), ESafePointerAccess_Read);

	llvm::Value* pLlvmPtr = m_pModule->m_LlvmBuilder.CreateExtractValue (pLlvmValue, 0, "sf_ptr");

	llvm::Type* pLlvmPtrType = pTargetType->GetPointerType (EType_Pointer_u)->GetLlvmType ();
	pLlvmPtr = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pLlvmPtrType, "sf_ptr_cast");
	llvm::Value* pLlvmLoad = m_pModule->m_LlvmBuilder.CreateLoad (pLlvmPtr, IsVolatile, "loa");
	
	pResultValue->SetLlvmRegister (pLlvmLoad, pTargetType);
	return true;
}

bool
COperatorMgr::LoadReferenceOperator (CValue* pValue)
{
	CValue ResultValue;

	bool Result = LoadReferenceOperator (*pValue, &ResultValue);
	if (!Result)
		return false;

	*pValue = ResultValue;
	return true;
}

bool
COperatorMgr::StoreReferenceOperator (
	const CValue& SrcValue,
	const CValue& RawDstValue
	)
{
	bool Result;

	CValue DstValue = RawDstValue;

	CPointerType* pDstType = (CPointerType*) DstValue.GetType ();
	ASSERT (pDstType->IsReferenceType ());

	CType* pTargetType = pDstType->GetBaseType ();
	while (pTargetType->IsReferenceType ()) // peel double references
	{
		Result = LoadReferenceOperator (&DstValue);
		if (!Result)
			return false;

		pDstType = (CPointerType*) pTargetType;
		pTargetType = pDstType->GetBaseType ();
	}

	bool IsVolatile = false;
	if (pTargetType->GetTypeKind () == EType_Qualifier)
	{
		IsVolatile = (pTargetType->GetFlags () & ETypeQualifier_Volatile) != 0;
		pTargetType = ((CDerivedType*) pTargetType)->GetBaseType ();
	}
	
	CValue CastValue;
	Result = CastOperator (SrcValue, pTargetType, &CastValue);
	if (!Result)
		return false;

	llvm::Value* pLlvmSrcValue = CastValue.GetLlvmValue ();
	llvm::Value* pLlvmDstValue = DstValue.GetLlvmValue ();

	if (pTargetType->IsFatPointerType () && CastValue.GetValueKind () == EValue_Variable)
	{
		CVariable* pVariable = CastValue.GetVariable ();
		pLlvmSrcValue = CreateLlvmSafePointer (
			pLlvmSrcValue, 
			pVariable->GetLlvmValue (), 
			pVariable->GetType ()
			);
	}

	EType DstTypeKind = pDstType->GetTypeKind ();	
	if (DstTypeKind == EType_Reference_u || DstValue.GetValueKind () == EValue_Variable)
	{
		m_pModule->m_LlvmBuilder.CreateStore (pLlvmSrcValue, pLlvmDstValue, IsVolatile);
		return true;
	}

	CheckLlvmSafePointer (pLlvmDstValue, pTargetType->GetSize (), ESafePointerAccess_Write);

	llvm::Value* pLlvmPtr = m_pModule->m_LlvmBuilder.CreateExtractValue (pLlvmDstValue, 0, "sf_ptr");
	llvm::Type* pLlvmPtrType = pTargetType->GetPointerType (EType_Pointer_u)->GetLlvmType ();

	pLlvmPtr = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pLlvmPtrType, "sf_ptr_cast");
	m_pModule->m_LlvmBuilder.CreateStore (pLlvmSrcValue, pLlvmPtr, IsVolatile);
	return true;
}

bool
COperatorMgr::GetPropertyOperator (
	CProperty* pProperty,
	CValue* pResultValue
	)
{
	err::SetFormatStringError (_T("getting property is not implemented yet"));
	return false;
}

bool
COperatorMgr::SetPropertyOperator (
	const CValue& SrcValue,
	CProperty* pProperty
	)
{
	CFunctionOverload* pSetter = pProperty->GetSetter ();
	if (!pSetter)
	{
		err::SetFormatStringError (_T("cannot move to a read-only property"));
		return false;
	}

	err::SetFormatStringError (_T("setting properies not implemented yet"));
	return false;
}

llvm::Value*
COperatorMgr::CreateLlvmGep (
	llvm::Value* pLlvmPtr,
	llvm::Value* pLlvmIndex0,
	llvm::Value* pLlvmIndex1
	)
{
	if (!pLlvmIndex0)
	{
		CValue Index0;
		Index0.SetConstInt32 (0, EType_Int32);
		pLlvmIndex0 = Index0.GetLlvmValue ();
	}
		
	if (!pLlvmIndex1)
		return m_pModule->m_LlvmBuilder.CreateGEP (pLlvmPtr, pLlvmIndex0, "gep");

	llvm::Value* LlvmIndexArray [] =
	{
		pLlvmIndex0,
		pLlvmIndex1,
	};

	return m_pModule->m_LlvmBuilder.CreateGEP (
		pLlvmPtr, 
		llvm::ArrayRef <llvm::Value*> (LlvmIndexArray, 2), 
		"gep"
		);
}

llvm::Value*
COperatorMgr::CreateLlvmGep (
	llvm::Value* pLlvmPtr,
	intptr_t Index0,
	intptr_t Index1
	)
{
	CValue IndexValue0;
	IndexValue0.SetConstInt32 (Index0, EType_Int32);

	CValue IndexValue1;
	IndexValue1.SetConstInt32 (Index1, EType_Int32);

	return CreateLlvmGep (pLlvmPtr, IndexValue0.GetLlvmValue (), IndexValue1.GetLlvmValue ());
}

llvm::Value*
COperatorMgr::CreateLlvmSafePointer (
	llvm::Value* pLlvmPtr,
	llvm::Value* pLlvmParentPtr,
	CType* pParentType
	)
{
	// specialize CallOperator () for effiency

	CType* pCharPointerType = m_pModule->m_TypeMgr.GetPointerType (EType_Pointer_u, EType_Int8);

	CValue ParentSize;
	ParentSize.SetConstSizeT (pParentType->GetSize ());

	llvm::Value* LlvmArgArray [3];
	LlvmArgArray [0] = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pCharPointerType->GetLlvmType (), "sp_ptr_cast");
	LlvmArgArray [1] = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmParentPtr, pCharPointerType->GetLlvmType (), "sp_beg_cast");
	LlvmArgArray [2] = ParentSize.GetLlvmValue ();

	CFunction* pCreateSafePointer = m_pModule->m_FunctionMgr.GetCreateSafePtr ();

	return m_pModule->m_LlvmBuilder.CreateCall (
		pCreateSafePointer->GetLlvmFunction (),
		llvm::ArrayRef <llvm::Value*> (LlvmArgArray, countof (LlvmArgArray)), 
		"sptr"
		);
}

llvm::Value*
COperatorMgr::CreateLlvmDynamicPointer (
	llvm::Value* pLlvmPtr,
	CType* pType
	)
{
	CType* pCharPointerType = m_pModule->m_TypeMgr.GetPointerType (EType_Pointer_u, EType_Int8);
	
	pLlvmPtr = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pCharPointerType->GetLlvmType (), "dp_ptr_cast");

	llvm::Value* pLlvmTypePtr = CValue (pCharPointerType, &pType).GetLlvmValue ();

	llvm::Value* pLlvmFatPtr = m_pModule->m_TypeMgr.GetDoublePointerStructType ()->GetLlvmUndefValue ();

	pLlvmFatPtr = m_pModule->m_LlvmBuilder.CreateInsertValue (pLlvmFatPtr, pLlvmPtr, 0, "dp_m_ptr");
	pLlvmFatPtr = m_pModule->m_LlvmBuilder.CreateInsertValue (pLlvmFatPtr, pLlvmTypePtr, 1, "dp_m_type");

	return pLlvmFatPtr;
}

llvm::Value*
COperatorMgr::ModifyLlvmSafePointer (
	llvm::Value* pLlvmSafePtr,
	llvm::Value* pLlvmPtr
	)
{
	CType* pCharPointerType = m_pModule->m_TypeMgr.GetPointerType (EType_Pointer_u, EType_Int8);
	
	pLlvmPtr = m_pModule->m_LlvmBuilder.CreateBitCast (pLlvmPtr, pCharPointerType->GetLlvmType (), "sp_ptr_cast");
	pLlvmSafePtr = m_pModule->m_LlvmBuilder.CreateInsertValue (pLlvmSafePtr, pLlvmPtr, 0, "sp_ptr");

	return pLlvmSafePtr;
}

void
COperatorMgr::CheckLlvmSafePointer (
	llvm::Value* pLlvmSafePtr,
	size_t Size,
	int Access
	)
{
	CValue SizeValue;
	SizeValue.SetConstSizeT (Size);

	CValue AccessValue;
	AccessValue.SetConstInt32 (Access, EType_Int);

	CFunction* pCheckSafePointer = m_pModule->m_FunctionMgr.GetCheckSafePtr ();

	m_pModule->m_LlvmBuilder.CreateCall3 (
		pCheckSafePointer->GetLlvmFunction (),
		pLlvmSafePtr,
		SizeValue.GetLlvmValue (),
		AccessValue.GetLlvmValue ()
		);
}

//.............................................................................

} // namespace axl {
} // namespace jnc {

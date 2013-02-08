#include "stdafx.h"
#include "axl_jnc_OperatorMgr.h"
#include "axl_jnc_Module.h"

namespace axl {
namespace jnc {

//.............................................................................

bool
COperatorMgr::GetFieldMember (
	const CValue& ThisValue,
	CStructMember* pMember,
	CValue* pResultValue
	)
{
	bool Result;

	CStructType* pStructType = pMember->GetParentStructType ();
	CModuleItem* pFieldParent = pStructType->GetFieldParent ();
	ASSERT (pFieldParent);

	if (pStructType->GetStorageKind () != EStorage_Static && ThisValue.IsEmpty ())
	{
		err::SetFormatStringError (_T("function '%s' has no 'this' pointer"), m_pModule->m_FunctionMgr.GetCurrentFunction ()->m_Tag);
		return false;
	}

	if (pFieldParent->GetItemKind () == EModuleItem_Property)
	{
		CProperty* pProperty = (CProperty*) pFieldParent;

		if (pStructType->GetStorageKind () == EStorage_Static)
			return StructMemberOperator (
				pProperty->GetStaticDataVariable (), 
				pProperty->GetStaticFieldStructType (),
				pMember->GetName (),
				pResultValue
				);

		CClassType* pParentClassType = pProperty->GetParentClassType ();
		ASSERT (pParentClassType);

		CClassBaseTypeCoord Coord;

		CValue FieldValue;
		Result = ClassFieldMemberOperator (
			ThisValue, 
			pParentClassType,
			pProperty->GetDataFieldMember (),
			&Coord,
			&FieldValue
			);

		if (!Result)
			return false;

		return StructMemberOperator (FieldValue, pProperty->GetFieldStructType (), pMember->GetName (), pResultValue);
	}
	else
	{
		ASSERT (pFieldParent->GetItemKind () == EModuleItem_Type && ((CType*) pFieldParent)->GetTypeKind () == EType_Class);
		CClassType* pClassType = (CClassType*) pFieldParent;

		if (pStructType->GetStorageKind () == EStorage_Static)
			return StructMemberOperator (
				pClassType->GetStaticDataVariable (), 
				pClassType->GetStaticFieldStructType (),
				pMember->GetName (),
				pResultValue
				);

		return MemberOperator (ThisValue, pMember->GetName (), pResultValue);	
	}
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
	if (pType->GetTypeKind () == EType_DataRef)
		pType = ((CDataPtrType*) pType)->GetTargetType ();

	if (pType->GetTypeKind () == EType_DataPtr)
	{
		pType = ((CDataPtrType*) pType)->GetTargetType ();

		Result = UnaryOperator (EUnOp_Indir, &OpValue);
		if (!Result)
			return false;
	}

	EType TypeKind = pType->GetTypeKind ();
	switch (TypeKind)
	{
	case EType_Struct:
		return StructMemberOperator (OpValue, (CStructType*) pType, pName, pResultValue);

	case EType_Union:
		return UnionMemberOperator (OpValue, (CUnionType*) pType, pName, pResultValue);

	case EType_ClassPtr:
		return 
			PrepareOperand (&OpValue) &&
			ClassMemberOperator (OpValue, (CClassType*) pType, pName, pResultValue);

	default:
		err::SetFormatStringError (_T("member operator cannot be applied to '%s'"), pType->GetTypeString ());
		return false;
	}
}

bool
COperatorMgr::WeakMemberOperator (
	const CValue& RawOpValue,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	err::SetFormatStringError (_T("weak member operator is not implemented yet"));
	return false;
}

/*

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

*/

bool
COperatorMgr::StructMemberOperator (
	const CValue& OpValue,
	CStructType* pStructType,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CStructBaseTypeCoord Coord;
	CStructMember* pMember = pStructType->FindMember (pName, &Coord);
	if (!pMember)
	{
		err::SetFormatStringError (_T("'%s' is not a member of '%s'"), pName, pStructType->GetTypeString ());
		return false;
	}

	Coord.m_LlvmIndexArray.Append (pMember->GetLlvmIndex ());
	Coord.m_Offset += pMember->GetOffset ();
	
	EValue OpValueKind = OpValue.GetValueKind ();
	if (OpValueKind == EValue_Const)
	{
		pResultValue->CreateConst (
			(char*) OpValue.GetConstData () + Coord.m_Offset,
			pMember->GetType () 
			);

		return true;
	}

	if (OpValue.GetType ()->GetTypeKind () != EType_DataRef)
	{
		m_pModule->m_LlvmBuilder.CreateExtractValue (
			OpValue, 
			Coord.m_LlvmIndexArray, 
			Coord.m_LlvmIndexArray.GetCount (),
			pMember->GetType (),
			pResultValue
			);

		return true;
	}

	CDataPtrType* pOpType = (CDataPtrType*) OpValue.GetType ();

	Coord.m_LlvmIndexArray.Insert (0, 0);

	if (pOpType->GetPtrTypeKind () == EDataPtrType_Unsafe)
	{
		CType* pResultType = pMember->GetType ()->GetDataPtrType (EType_DataRef, EDataPtrType_Unsafe);
		m_pModule->m_LlvmBuilder.CreateGep (
			OpValue, 
			Coord.m_LlvmIndexArray, 
			Coord.m_LlvmIndexArray.GetCount (), 
			pResultType, 
			pResultValue
			);

		return true;
	}
	
	ASSERT (pOpType->GetTypeKind () == EType_DataRef);

	if (OpValue.GetValueKind () == EValue_Variable)
	{
		CValue PtrValue;
		m_pModule->m_LlvmBuilder.CreateGep (
			OpValue, 
			Coord.m_LlvmIndexArray, 
			Coord.m_LlvmIndexArray.GetCount (), 
			NULL, 
			&PtrValue
			);
		
		pResultValue->SetVariable (
			OpValue.GetVariable (), 
			PtrValue.GetLlvmValue (), 
			pMember->GetType (),
			true,
			(OpValue.GetFlags () & EValueFlag_VariableOffset) != 0
			);
	}
	else
	{
		CValue PtrValue;
		CValue ValidatorValue;

		CType* pResultType = pMember->GetType ()->GetDataPtrType (EType_DataRef);

		m_pModule->m_LlvmBuilder.CreateExtractValue (OpValue, 0, NULL, &PtrValue);
		m_pModule->m_LlvmBuilder.CreateExtractValue (OpValue, 1, NULL, &ValidatorValue);
		m_pModule->m_LlvmBuilder.CreateGep (
			PtrValue, 
			Coord.m_LlvmIndexArray, 
			Coord.m_LlvmIndexArray.GetCount (), 
			NULL, 
			&PtrValue
			);

		CValue DataPtrValue = pResultType->GetUndefValue ();
		m_pModule->m_LlvmBuilder.CreateInsertValue (DataPtrValue, PtrValue, 0, NULL, &DataPtrValue);
		m_pModule->m_LlvmBuilder.CreateInsertValue (DataPtrValue, ValidatorValue, 1, pResultType, pResultValue);
	}

	return true;
}

bool
COperatorMgr::UnionMemberOperator (
	const CValue& OpValue,
	CUnionType* pUnionType,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CUnionMember* pMember = pUnionType->FindMember (pName);
	if (!pMember)
	{
		err::SetFormatStringError (_T("'%s' is not a member of '%s'"), pName, pUnionType->GetTypeString ());
		return false;
	}
	
	EValue OpValueKind = OpValue.GetValueKind ();
	if (OpValueKind == EValue_Const)
	{
		pResultValue->CreateConst (OpValue.GetConstData (), pMember->GetType ());
		return true;
	}

	if (OpValue.GetType ()->GetTypeKind () != EType_DataRef)
	{
		err::SetFormatStringError (_T("union member operator on registers is not implemented yet"));
		return false;
	}

	CDataPtrType* pOpType = (CDataPtrType*) OpValue.GetType ();

	if (pOpType->GetPtrTypeKind () == EDataPtrType_Unsafe)
	{
		CType* pResultType = pMember->GetType ()->GetDataPtrType (EType_DataRef, EDataPtrType_Unsafe);
		m_pModule->m_LlvmBuilder.CreateBitCast (OpValue, pResultType, pResultValue);
		return true;
	}
	
	if (OpValue.GetValueKind () == EValue_Variable)
	{
		CType* pCastType = pMember->GetType ()->GetDataPtrType (EType_DataRef, EDataPtrType_Unsafe);
		CValue CastValue;
		m_pModule->m_LlvmBuilder.CreateBitCast (OpValue, pCastType, &CastValue);
		pResultValue->SetVariable (
			OpValue.GetVariable (), 
			CastValue.GetLlvmValue (), 
			pMember->GetType (),
			true,
			(OpValue.GetFlags () & EValueFlag_VariableOffset) != 0
			);
	}
	else
	{
		CType* pResultType = pMember->GetType ()->GetDataPtrType (EType_DataRef);
		pResultValue->OverrideType (OpValue, pResultType);
	}

	return true;
}

bool
COperatorMgr::ClassMemberOperator (
	const CValue& OpValue,
	CClassType* pClassType,
	const tchar_t* pName,
	CValue* pResultValue
	)
{
	CClassBaseTypeCoord Coord;
	CModuleItem* pMember = pClassType->FindMember (pName, &Coord);	
	if (!pMember)
	{
		err::SetFormatStringError (_T("'%s' is not a member of '%s'"), pName, pClassType->GetTypeString ());
		return false;
	}

	CModuleItemDecl* pDecl = pMember->GetItemDecl ();
	if (pDecl->GetAccessKind () != EAccess_Public)
	{
		err::SetFormatStringError (_T("'%s' is not accessible"), pName);
		return false;
	}

	CheckClassPtrNull (OpValue);

	EModuleItem MemberKind = pMember->GetItemKind ();
	switch (MemberKind)
	{
	case EModuleItem_StructMember:
		return ClassFieldMemberOperator (
			OpValue, 
			pClassType, 
			(CStructMember*) pMember, 
			&Coord,
			pResultValue
			);
		
	case EModuleItem_Function:
		pResultValue->SetFunction ((CFunction*) pMember);
		if (pDecl->GetStorageKind () != EStorage_Static)
		{
			CClosure* pClosure = pResultValue->CreateClosure ();
			pClosure->GetArgList ()->InsertHead (OpValue);
		}

		break;

	case EModuleItem_Property:
		pResultValue->SetProperty ((CProperty*) pMember);
		if (pDecl->GetStorageKind () != EStorage_Static)
		{
			CClosure* pClosure = pResultValue->CreateClosure ();
			pClosure->GetArgList ()->InsertHead (OpValue);
		}
		break;

	default:
		err::SetFormatStringError (_T("invalid interface member kind"));
		return false;
	}

	return true;
}

bool
COperatorMgr::ClassFieldMemberOperator (
	const CValue& OpValue,
	CClassType* pClassType,
	CStructMember* pMember,
	CClassBaseTypeCoord* pCoord,
	CValue* pResultValue
	)
{
	CValue ObjPtrValue;
	
	size_t ObjPtrIndexArray [] = 
	{
		0, // iface* 
		0, // iface.hdr*
		1, // TObject**
	};

	m_pModule->m_LlvmBuilder.CreateGep (OpValue, ObjPtrIndexArray, countof (ObjPtrIndexArray), NULL, &ObjPtrValue);  // TObject**
	m_pModule->m_LlvmBuilder.CreateLoad (ObjPtrValue, NULL, &ObjPtrValue); // TObject* 

	CValue ScopeLevelValue;
	m_pModule->m_LlvmBuilder.CreateGep2 (ObjPtrValue, 1, NULL, &ScopeLevelValue);  // size_t* m_pScopeLevel
	m_pModule->m_LlvmBuilder.CreateLoad (ScopeLevelValue, NULL, &ScopeLevelValue); // size_t m_ScopeLevel
	
	pCoord->m_FieldCoord.m_LlvmIndexArray.Insert (0, 0);
	pCoord->m_FieldCoord.m_LlvmIndexArray.Insert (1, pClassType->GetFieldMember ()->GetLlvmIndex ());
	pCoord->m_FieldCoord.m_LlvmIndexArray.Append (pMember->GetLlvmIndex ());

	size_t* p = pCoord->m_FieldCoord.m_LlvmIndexArray;
	size_t Count = pCoord->m_FieldCoord.m_LlvmIndexArray.GetCount ();

	CValue PtrValue;
	m_pModule->m_LlvmBuilder.CreateGep (
		OpValue, 
		pCoord->m_FieldCoord.m_LlvmIndexArray, 
		pCoord->m_FieldCoord.m_LlvmIndexArray.GetCount (), 
		NULL,
		&PtrValue
		);

	CDataPtrType* pResultType = pMember->GetType ()->GetDataPtrType (EType_DataRef);

	m_pModule->m_LlvmBuilder.CreateDataPtr (
		PtrValue, 
		PtrValue, 
		pMember->GetType ()->GetSize (),
		ScopeLevelValue,
		pResultType, 
		pResultValue
		);

	return true;
}

bool
COperatorMgr::GetClassFieldMemberValue (
	const CValue& ObjValue,
	CStructMember* pMember,
	CValue* pValue
	)
{
	ASSERT (ObjValue.GetType ()->GetTypeKind () == EType_ClassPtr);

	CClassType* pClassType = ((CClassPtrType*) ObjValue.GetType ())->GetTargetType ();

	CValue FieldValue;
	CClassBaseTypeCoord ClosureCoord;

	return 
		ClassFieldMemberOperator (ObjValue, pClassType, pMember, &ClosureCoord, &FieldValue) && 
		LoadDataRef (FieldValue, pValue);
}

bool
COperatorMgr::SetClassFieldMemberValue (
	const CValue& ObjValue,
	CStructMember* pMember,
	const CValue& Value
	)
{
	ASSERT (ObjValue.GetType ()->GetTypeKind () == EType_ClassPtr);

	CClassType* pClassType = ((CClassPtrType*) ObjValue.GetType ())->GetTargetType ();

	CValue FieldValue;
	CClassBaseTypeCoord ClosureCoord;

	return 
		ClassFieldMemberOperator (ObjValue, pClassType, pMember, &ClosureCoord, &FieldValue) && 
		BinaryOperator (EBinOp_Assign, FieldValue, Value);
}

bool
COperatorMgr::GetClassVTable (
	const CValue& OpValue,
	CClassType* pClassType,
	CValue* pResultValue
	)
{
	size_t IndexArray [] = 
	{
		0, // class.iface*
		0, // class.iface.hdr*
		0, // class.vtbl**
	};

	CValue PtrValue;
	m_pModule->m_LlvmBuilder.CreateGep (
		OpValue, 
		IndexArray, 
		countof (IndexArray),
		NULL, 
		&PtrValue
		);

	// class.vtbl*

	CDataPtrType* pResultType = pClassType->GetVTableStructType ()->GetDataPtrType (EDataPtrType_Unsafe);
	m_pModule->m_LlvmBuilder.CreateLoad (PtrValue, pResultType, pResultValue);
	return true;
}

bool
COperatorMgr::GetVirtualMethodMember (
	CFunction* pFunction,
	CClosure* pClosure,
	CValue* pResultValue
	)
{
	ASSERT (pFunction->GetStorageKind () == EStorage_Virtual && pClosure);
	
	CValue Value = *pClosure->GetArgList ()->GetHead ();
	if (Value.GetType ()->GetTypeKind () != EType_ClassPtr)
	{
		err::SetFormatStringError (_T("virtual method call requires an object pointer"));
		return false;
	}

	CClassType* pClassType = (CClassType*) Value.GetType ();
	CClassType* pVTableType = pFunction->GetVirtualOriginClassType ();
	size_t VTableIndex = pFunction->GetClassVTableIndex ();
	
	CClassBaseTypeCoord Coord;
	pClassType->FindBaseType (pVTableType, &Coord);
	VTableIndex += Coord.m_VTableIndex;
	
	// class.vtbl*

	CValue PtrValue;
	GetClassVTable (Value, pClassType, &PtrValue);

	// pfn*

	m_pModule->m_LlvmBuilder.CreateGep2 (
		PtrValue, 
		VTableIndex,
		NULL, 
		&PtrValue
		);

	// pfn

	m_pModule->m_LlvmBuilder.CreateLoad (
		PtrValue, 
		NULL,
		&PtrValue
		);

	pResultValue->SetLlvmValue (
		PtrValue.GetLlvmValue (), 
		pFunction->GetType ()->GetFunctionPtrType (EFunctionPtrType_Thin)
		);

	return true;
}

bool
COperatorMgr::GetVirtualPropertyMember (
	CProperty* pProperty,
	CClosure* pClosure,
	CValue* pResultValue
	)
{
	ASSERT (pProperty->GetStorageKind () == EStorage_Virtual && pClosure);
	
	CValue Value = *pClosure->GetArgList ()->GetHead ();
	if (Value.GetType ()->GetTypeKind () != EType_ClassPtr)
	{
		err::SetFormatStringError (_T("virtual property requires an object pointer"));
		return false;
	}

	CClassType* pClassType = (CClassType*) Value.GetType ();
	size_t VTableIndex = pProperty->GetParentClassVTableIndex ();

	CClassBaseTypeCoord Coord;
	pClassType->FindBaseType (pProperty->GetParentClassType (), &Coord);
	VTableIndex += Coord.m_VTableIndex;

	// class.vtbl*

	CValue PtrValue;
	GetClassVTable (Value, pClassType, &PtrValue);

	// property.vtbl*

	m_pModule->m_LlvmBuilder.CreateGep2 (
		PtrValue, 
		VTableIndex,
		NULL,
		&PtrValue
		);

	m_pModule->m_LlvmBuilder.CreateBitCast (
		PtrValue, 
		pProperty->GetType ()->GetVTableStructType ()->GetDataPtrType (EDataPtrType_Unsafe),
		&PtrValue
		);

	pResultValue->OverrideType (PtrValue, pProperty->GetType ()->GetPropertyPtrType (EPropertyPtrType_Thin));
	return true;
}

//.............................................................................

} // namespace jnc {
} // namespace axl {
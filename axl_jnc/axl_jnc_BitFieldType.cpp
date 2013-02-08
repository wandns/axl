#include "stdafx.h"
#include "axl_jnc_BitFieldType.h"

namespace axl {
namespace jnc {

//.............................................................................

CBitFieldType::CBitFieldType ()
{
	m_TypeKind = EType_BitField;
	m_Flags = ETypeFlag_Pod;
	m_pBaseType = NULL;
	m_BitOffset = 0;
	m_BitCount = 0;
}

void
CBitFieldType::PrepareTypeString ()
{
	m_TypeString.Format (
		"%s:%d:%d",
		m_pBaseType->GetTypeString (),
		m_BitOffset,
		m_BitOffset + m_BitCount
		); 
}

//.............................................................................

} // namespace jnc {
} // namespace axl {

#include "pch.h"
#include "axl_err_ErrorMgr.h"
#include "axl_err_Errno.h"
#include "axl_dbg_Trace.h"

#if (_AXL_ENV == AXL_ENV_WIN)
#	include "axl_err_WinError.h"
#	include "axl_err_NtError.h"
#endif

namespace axl {
namespace err {

//.............................................................................

ErrorMgr::ErrorMgr ()
{
	m_tlsSlot = mt::getTlsMgr ()->createSlot ();

	registerProvider (GUID_StdError, rtl::getSimpleSingleton <StdErrorProvider> ());
	registerProvider (GUID_Errno, rtl::getSimpleSingleton <ErrnoProvider> ());

#if (_AXL_ENV == AXL_ENV_WIN)
	registerProvider (GUID_WinError, rtl::getSimpleSingleton <WinErrorProvider> ());
	registerProvider (GUID_NtError, rtl::getSimpleSingleton <NtErrorProvider> ());
#endif
}

void
ErrorMgr::registerProvider (
	const rtl::Guid& guid,
	ErrorProvider* provider
	)
{
	mt::ScopeLock scopeLock (&m_lock);
	m_providerMap.visit (guid)->m_value = provider;
}

ErrorProvider* 
ErrorMgr::findProvider (const rtl::Guid& guid)
{
	mt::ScopeLock scopeLock (&m_lock);
	rtl::HashTableMapIterator <rtl::Guid, ErrorProvider*> it = m_providerMap.find (guid);
	return it ? it->m_value : NULL;
}

ErrorModeKind
ErrorMgr::setErrorMode (ErrorModeKind mode)
{
	ThreadEntry* entry = getThreadEntry ();
	ErrorModeKind oldMode = entry->m_mode;
	entry->m_mode = mode;
	return oldMode;
}

Error
ErrorMgr::getError ()
{
	ThreadEntry* entry = findThreadEntry ();
	if (entry && entry->m_error)
		return entry->m_error;

	return g_noError;
}

void
ErrorMgr::setError (const Error& error)
{	
	ThreadEntry* entry = getThreadEntry ();
	entry->m_error = error;
	ThreadEntry* entry2 = findThreadEntry ();
	
	switch (entry->m_mode)
	{
	case ErrorModeKind_NoThrow:
		break;

	case ErrorModeKind_CppException:
		throw error;

	case ErrorModeKind_SehException:
	case ErrorModeKind_SetJmpLongJmp:
		TRACE ("*** unsupported error mode\n");

	default:
		ASSERT (false);
	}
}

ErrorMgr::ThreadEntry*
ErrorMgr::getThreadEntry ()
{
	ThreadEntry* entry = findThreadEntry ();
	if (entry)
		return entry;

	ref::Ptr <ThreadEntry> newEntry = AXL_REF_NEW (ref::Box <ThreadEntry>);
	mt::getTlsMgr ()->setSlotValue (m_tlsSlot, newEntry);
	return newEntry;
}

//.............................................................................

} // namespace err
} // namespace axl

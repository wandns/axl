#include "pch.h"
#include "axl_exe_WorkerWindow.h"
#include "axl_exe_Function.h"

namespace axl {
namespace exe {

//.............................................................................

WorkerWindow::ScheduleResultKind
WorkerWindow::scheduleV (
	exe::IFunction* function, 
	axl_va_list va
	)
{
	// check if we are already in worker thread

	if (m_threadId == ::GetCurrentThreadId ())
	{
		function->invokeV (va);
		return ScheduleResultKind_Invoke;
	}

	m_lock.lock ();

	m_invokeList.addV (function, va);

	if (m_invokeList.getCount () == 1) // post msg only once
		postMessage (WM_PROCESS_INVOKE_LIST);

	m_lock.unlock ();

	return ScheduleResultKind_Pending;
}

LRESULT
WorkerWindow::onProcessInvokeList (
	UINT msg,
	WPARAM wParam,
	LPARAM lParam,
	bool* isHandled
	)
{
	exe::InvokeList invokeList;

	m_lock.lock ();
	invokeList.takeOver (&m_invokeList);
	m_lock.unlock ();

	invokeList.process ();

	return 0;
}

//.............................................................................

} // namespace exe
} // namespace axl


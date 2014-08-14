#include "pch.h"
#include "axl_io_win_Serial.h"
#include "axl_rtl_String.h"

namespace axl {
namespace io {
namespace win {

//.............................................................................

bool 
CSerial::Open (
	const char* pName,
	uint_t Flags
	)
{
	Close ();

	if (strncmp (pName, "\\\\.\\", 4) == 0)
		pName += 4;
	
	char Buffer [256];
	rtl::CString_w DeviceName (ref::EBuf_Stack, Buffer, sizeof (Buffer));
	DeviceName.Format (L"\\\\.\\%S", pName);

	m_h = ::CreateFileW (
		DeviceName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, NULL, 
		OPEN_EXISTING, 
		Flags, 
		NULL
		);

	return err::Complete (m_h != INVALID_HANDLE_VALUE);
}

dword_t
CSerial::GetStatusLines ()
{
	ASSERT (IsOpen ());

	dword_t Lines;
		
	bool_t Result = ::GetCommModemStatus (m_h, &Lines);
	if (!Result)
		return err::FailWithLastSystemError (-1);

	return Lines;
}

dword_t
CSerial::GetWaitMask ()
{
	dword_t Mask;

	bool_t Result = ::GetCommMask (m_h, &Mask);
	if (!Result)
		return err::FailWithLastSystemError (-1);

	return Mask;
}

size_t
CSerial::Read (
	void* p, 
	size_t Size 
	)
{
	dword_t ActualSize;
	bool_t Result = Read (p, (dword_t) Size, &ActualSize, NULL);
	return Result ? ActualSize : -1;
}

size_t
CSerial::Write (
	const void* p, 
	size_t Size
	)
{
	dword_t ActualSize;
	bool_t Result = Write (p, (dword_t) Size, &ActualSize, NULL);
	return Result ? ActualSize : -1;
}

//.............................................................................

} // namespace win
} // namespace io
} // namespace axl


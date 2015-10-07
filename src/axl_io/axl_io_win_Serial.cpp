#include "pch.h"
#include "axl_io_win_Serial.h"
#include "axl_sl_String.h"

namespace axl {
namespace io {
namespace win {

//.............................................................................

bool 
Serial::open (
	const char* name,
	uint_t flags
	)
{
	close ();

	if (strncmp (name, "\\\\.\\", 4) == 0)
		name += 4;
	
	char buffer [256];
	sl::String_w deviceName (ref::BufKind_Stack, buffer, sizeof (buffer));
	deviceName.format (L"\\\\.\\%S", name);

	m_h = ::CreateFileW (
		deviceName, 
		GENERIC_READ | GENERIC_WRITE, 
		0, NULL, 
		OPEN_EXISTING, 
		flags, 
		NULL
		);

	return err::complete (m_h != INVALID_HANDLE_VALUE);
}

dword_t
Serial::getStatusLines ()
{
	ASSERT (isOpen ());

	dword_t lines;
		
	bool_t result = ::GetCommModemStatus (m_h, &lines);
	if (!result)
		return err::failWithLastSystemError (-1);

	return lines;
}

dword_t
Serial::getWaitMask ()
{
	dword_t mask;

	bool_t result = ::GetCommMask (m_h, &mask);
	if (!result)
		return err::failWithLastSystemError (-1);

	return mask;
}

size_t
Serial::read (
	void* p, 
	size_t size 
	)
{
	dword_t actualSize;
	bool_t result = read (p, (dword_t) size, &actualSize, NULL);
	return result ? (size_t) actualSize : -1;
}

size_t
Serial::write (
	const void* p, 
	size_t size
	)
{
	dword_t actualSize;
	bool_t result = write (p, (dword_t) size, &actualSize, NULL);
	return result ? (size_t) actualSize : -1;
}

//.............................................................................

} // namespace win
} // namespace io
} // namespace axl


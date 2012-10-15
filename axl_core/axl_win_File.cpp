#include "stdafx.h"
#include "axl_win_File.h"

namespace axl {
namespace win {

//.............................................................................

uint64_t
CFile::GetSize ()
{
	ASSERT (IsOpen ());

	ULARGE_INTEGER Size;
	
	Size.LowPart = ::GetFileSize (m_h, &Size.HighPart);
	if (Size.LowPart == INVALID_FILE_SIZE)
	{
		err::SetLastSystemError ();
		return -1;
	}

	return Size.QuadPart;
}

bool
CFile::SetSize (uint64_t Size)
{
	ASSERT (IsOpen ());

	uint64_t Position = GetPosition ();

	bool_t Result = SetPosition (Size);
	if (!Result)
		return false;

	Result = ::SetEndOfFile (m_h);
	if (!Result)
		return err::FailWithLastSystemError ();

	SetPosition (Position);
	return true;
}

uint64_t
CFile::GetPosition ()
{
	ASSERT (IsOpen ());

	LARGE_INTEGER Offset = { 0 };

	Offset.LowPart = ::SetFilePointer (m_h, 0, &Offset.HighPart, FILE_CURRENT);
	if (Offset.LowPart == INVALID_FILE_SIZE)
	{
		err::SetLastSystemError ();
		return -1;
	}

	return Offset.QuadPart;
}

bool
CFile::SetPosition (uint64_t _Offset)
{
	ASSERT (IsOpen ());

	LARGE_INTEGER Offset;
	Offset.QuadPart = _Offset;

	ulong_t Result = ::SetFilePointer (m_h, Offset.LowPart, &Offset.HighPart, FILE_BEGIN);
	return err::Complete (Result != INVALID_SET_FILE_POINTER);
}

//.............................................................................

} // namespace win
} // namespace axl

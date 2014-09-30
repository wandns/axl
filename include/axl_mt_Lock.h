// This file is part of AXL (R) Library
// Tibbo Technology Inc (C) 2004-2014. All rights reserved
// Author: Vladimir Gladkov

#pragma once

#define _AXL_MT_LOCK_H

#include "axl_g_Def.h"

#if (_AXL_ENV == AXL_ENV_WIN)
#	include "axl_mt_win_CriticalSection.h"
#elif (_AXL_ENV == AXL_ENV_POSIX)
#	include "axl_mt_psx_Mutex.h"
#endif

namespace axl {
namespace mt {

//.............................................................................

#if (_AXL_ENV == AXL_ENV_WIN)

class Lock
{
public:
	win::CriticalSection m_criticalSection;

public:
	void 
	lock ()
	{
		m_criticalSection.enter ();
	}

	void 
	unlock ()
	{
		m_criticalSection.leave ();
	}
};

#elif (_AXL_ENV == AXL_ENV_POSIX)

class Lock
{
public:
	psx::Mutex m_mutex;

public:
	void 
	lock ()
	{
		m_mutex.lock ();
	}

	void 
	unlock ()
	{
		m_mutex.unlock ();
	}
};

#endif

//. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

class ScopeLock 
{
protected:
	Lock* m_lock;

public:
	ScopeLock (Lock* lock)
	{
		m_lock = lock;		
		lock->lock ();
	}

	~ScopeLock ()
	{
		m_lock->unlock ();
	}
};

//.............................................................................

} // namespace mt
} // namespace axl



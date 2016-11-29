//..............................................................................
//
//  This file is part of the AXL library.
//
//  AXL is distributed under the MIT license.
//  For details see accompanying license.txt file,
//  the public copy of which is also available at:
//  http://tibbo.com/downloads/archive/axl/license.txt
//
//..............................................................................

#include "pch.h"

namespace test_Scheduler {

/*

//..............................................................................


class Test
{
public:
	sl::String m_s;
	sl::String m_s2;
	sl::String m_s3;

	Test ()
	{
		m_s = "hui!!!";
		m_s2 = "hui!!!";
		m_s3 = "hui!!!";
		printf ("CTest::CTest\n");
	}

	~Test ()
	{
		printf ("CTest::~CTest\n");
	}
};

//..............................................................................

class Test2
{
public:
	Test2 ()
	{
		printf ("CTest2::CTest2\n");
	}

	~Test2 ()
	{
		printf ("CTest2::~CTest2\n");
	}
};

//..............................................................................

void
test (
	int x,
	int y,
	int z
	)
{
	printf ("Test: x = %d, y = %d, z = %d\n", x, y, z);
}

class MyScheduler: public exe::IScheduler
{
public:
	virtual
	ScheduleResultKind
	schedule_va (
		const exe::Function& function,
		axl_va_list va
		)
	{
		function->invoke_va (va);
		return ScheduleResultKind_Invoke;
	}
};

struct IInterfaceA
{
	int m_x;

	virtual void foo () {}
};

struct IInterfaceB: IInterfaceA
{
	int m_y;
};

struct IInterfaceC: IInterfaceA
{
	int m_z;
};

void
run ()
{
	Test* p;

	p = sl::getSingleton <Test> ();
	p = sl::getSingleton <Test> ();

	Test2* p2;

	p2 = sl::getSingleton <Test2> ();
	p2 = sl::getSingleton <Test2> ();

	ref::Ptr<Test> x = AXL_REF_NEW (ref::Box <Test>);
	ref::WeakPtr<Test> y = x;
	y.detach ();

	MyScheduler scheduler;

	exe::Function <
		exe::ArgSeq_2 <
			int,
			int
			>,
		exe::ArgSeq_3 <
			int,
			int,
			int
			>
		> function (test, -1, -2);

	function.invoke (0, 100, 200, 300);
}

*/

//..............................................................................

} // namespace test_Scheduler
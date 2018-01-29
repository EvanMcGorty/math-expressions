#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


struct leak_checker
{
	~leak_checker()
	{
#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
#endif
	}
};


leak_checker mem_leak_finder;

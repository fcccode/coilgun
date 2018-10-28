#include "stdafx.h"
#include "caller.h"
extern "C" {
	uintptr_t asmCall(void  * addr, void * args, int argc);
}

caller::caller()
{

}


caller::~caller()
{
}



uintptr_t caller::preparedCall(void * addr, void * args, int argc)
{
	return asmCall(addr, args, argc);
}

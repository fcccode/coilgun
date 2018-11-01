#include "stdafx.h"
#include "caller.h"


extern "C" {
	uintptr_t asmCall(void  * addr, void * args, int argc);
	uintptr_t callShellcode(void *shellCodeAddr);
}

caller::caller()
{

}


caller::~caller()
{
}
uintptr_t caller::shellcodeCall(void * addr) {
	
		return callShellcode(addr);
	
}


uintptr_t caller::preparedCall(void * addr, void * args, int argc)
{
	uintptr_t retVal = 0;
	retVal = asmCall(addr, args, argc);
	printf("[!] GetLastError code  = %d\n", GetLastError());
	return retVal;
}

void caller::segFaultOccured(int signum)
{
	printf("[~] Segfault...\n");
}

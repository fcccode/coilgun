#pragma once

class caller
{
public:
	caller();
	~caller();
	static uintptr_t shellcodeCall(void * addr);
	static uintptr_t preparedCall(void * addr, void * args, int argc);
	static int quickCall(void* addr, void *args[]);
};


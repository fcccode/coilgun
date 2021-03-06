// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//important definitions


#define TYPE_NAME_NOT_FOUND -1
#define TYPE_ERROR -2
#define TYPE_OK 0

#define LIBRARY_FOUND 0
#define LIBRARY_NOT_FOUND -1

#define LIBRARY_NOT_LOADED -1
#define FUNCTION_NOT_FOUND -2
#define RETURN_TYPE_NOT_FOUND -3

#define FORMAT_HEX 1000
#define FORMAT_STRING 1001
#define FORMAT_INT 1002
#define FORMAT_FLOAD 1002
#define FORMAT_DOUBLE 1003
#define FORMAT_WSTRING 1004

#define TYPE_FIELD_SIZE 10
#define TYPE_FIELD_NAME 11
#define TYPE_FIELD_TYPE 12
#define TYPE_FIELD_VAL	13
#define TYPE_FIELD_ADDR 14
#define TYPE_FIELD_ARGC 15
#define TYPE_FIELD_RET  16
#define TYPE_FIELD_FMT	17
#define TYPE_FIELD_FIELD 18

#define PROCESSING_ERR -1
#define PROCESSING_OK 0

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <algorithm>
#include <Windows.h>


// TODO: reference additional headers your program requires here

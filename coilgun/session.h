#pragma once
#include "stdafx.h"
#include "session.h"
#include <sstream>
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <codecvt>
#include <vector>
#include "caller.h"



struct TYPE {
	std::string name;
	unsigned int size;
	int outputFormat;
	void * typeStruct; //NULL if not struct
};

struct VARIABLE {
	void * varAddr;
	unsigned int size;
	std::string name;
	TYPE *type;
};

struct STRUCTURE {
	std::vector<TYPE*> *fields;
};

struct FUNCTION_DATA {
	void * funcAddr;
	unsigned int numOfArgs;
	TYPE *ReturnType;
};




class session
{
public:
	session();
	~session();

	uint8_t unxdigit(int c);

	long long int read_n_le(uint8_t * src, int n);
	
	int addType(std::string typeName,int size, int outputFormat);
	int addFunc(std::string dllName, std::string funcName, int NumOfArgs, std::string returnType);
	int defineStruct(std::string structName, std::vector<std::string> fieldTypes);
	
	int loadLibrary(std::string dllName);
	
	void editType(int field, std::string newVal, std::string typeName);
	void editVar(int field,std::string newVal,std::string varName);
	void editFunc(int field, std::string newVal, std::string funcName);

	int createVariable(std::string type, std::string Name, std::vector<std::string> values);

	int processStructData(STRUCTURE * structToFill, void * dstAddr, std::vector<std::string> values);

	int processStructData(STRUCTURE * structToFill, void * dstAddr);
	int processData(std::string data, int size, void* dstAddr);
	

	int deleteVariable(std::string varName);

	
	
	void printVariables();
	void printVariableValue(std::string varName);
	void printFuncData(std::string funcName);
	void printTypeData(std::string typeName);
	void printWithFormat(int size, void * varAddr, int OutputFormat);
	void printLoadedLibs();
	void printTypes();
	void printFuctions();

	uintptr_t getVarByName(std::string varName);

	uintptr_t getTypeByName(std::string TypeName);

	bool checkIfNumber(char testChar);

	void callWrapper(std::string funcName, std::vector<std::string> args);


	void execShellcode(void * shellCodeAddr, int size, bool noExec);
	


	std::vector<VARIABLE *> VARIABLE_LIST;
	std::vector<TYPE *> TYPES; //typeName + typeSize
	std::map<std::string, HMODULE> LIBRARIES; //dllName + dllHandle
	std::map<std::string, FUNCTION_DATA> FUNCTIONS_MAP; //funcName + funcAddr + arg count



	

private:
	
};


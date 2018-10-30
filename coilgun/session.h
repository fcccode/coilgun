#pragma once
#include <string>
#include <map>
#include <Windows.h>
#include <locale>
#include <codecvt>
#include <vector>




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
	TYPE type;
};

struct STRUCTURE {
	std::vector<TYPE> fields;
};

struct FUNCTION_DATA {
	void * funcAddr;
	unsigned int numOfArgs;
	TYPE ReturnType;
};




class session
{
public:
	session();
	~session();

	uint8_t unxdigit(int c);
	
	int addType(std::string typeName,int size, int outputFormat);
	int addFunc(std::string dllName, std::string funcName, int NumOfArgs, std::string returnType);
	int defineStruct(std::string structName, int numOfFields);
	
	int loadLibrary(std::string dllName);
	
	void editType(int field, std::string newVal, std::string typeName);
	void editVar(int field,std::string newVal,std::string varName);
	void editFunc(int field, std::string newVal, std::string funcName);

	int processStructData(STRUCTURE * structToFill, void * dstAddr);
	int processData(std::string data, int size, void* dstAddr);
	
	int createVariable(std::string type, std::string Name, uintptr_t data);

	int deleteVariable(std::string varName);

	
	
	void printVariables();
	void printVariableValue(std::string varName);
	void printWithFormat(int size, void * varAddr, int OutputFormat);
	void printLoadedLibs();
	void printTypes();
	void printFuctions();

	void callWrapper(std::string funcName);

	void execShellcode(void * shellCodeAddr, int size);

	
	void rawRead(uintptr_t addr, int size);

	std::vector<VARIABLE> VARIABLE_LIST;
	std::vector<TYPE> TYPES; //typeName + typeSize
	std::map<std::string, HMODULE> LIBRARIES; //dllName + dllHandle
	std::map<std::string, FUNCTION_DATA> FUNCTIONS_MAP; //funcName + funcAddr + arg count



	

private:
	
};


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
	unsigned int size;
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
	
	int addType(std::string typeName,int size, int outputFormat);
	int addFunc(std::string dllName, std::string funcName, int NumOfArgs, std::string returnType);
	int defineStruct(std::string structName, int numOfFields);
	
	int loadLibrary(std::string dllName);
	
	void editType(int field, std::string newVal, std::string typeName);
	void editVar(int field,std::string newVal,std::string varName);

	int processStructData(STRUCTURE * structToFill, int size, void * dstAddr);
	int processData(std::string data, int size, void* dstAddr);
	
	int createVariable(std::string type, std::string Name, uintptr_t data);
	int deleteVariable(std::string varName);
	int editVariable(std::string varName);
	
	void printVariables();
	void printVariableValue(std::string varName);
	void printLoadedLibs();
	void printTypes();
	void printFuctions();

	void callWrapper(std::string funcName);

	std::vector<VARIABLE> VARIABLE_LIST;
	std::vector<TYPE> TYPES; //typeName + typeSize
	std::map<std::string, HMODULE> LIBRARIES; //dllName + dllHandle
	std::map<std::string, FUNCTION_DATA> FUNCTIONS_MAP; //funcName + funcAddr + arg count



	

private:
	
};


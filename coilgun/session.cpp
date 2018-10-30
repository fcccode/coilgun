#include "stdafx.h"
#include "session.h"
#include <sstream>
#include <iostream>
#include <string>
#include "caller.h"

session::session()
{
	//initialize known types, not all tho
	TYPE DEFAULT;
	DEFAULT.name = "DEFAULT";
	DEFAULT.size = 8;
	DEFAULT.outputFormat = FORMAT_HEX;
	DEFAULT.typeStruct = nullptr;
	this->TYPES.push_back(DEFAULT);
}

session::~session()
{
}

uint8_t session::unxdigit(int c) // thanks braindead
{
	if (((unsigned)c - 48U) <= 9)
		return c - 48;
	return (c | 32) + 10 - 'a';
}

int session::addType(std::string typeName, int typeSize, int outputFormat)
{
	bool found = false;
	for (int i = 0 ; i < this->TYPES.size(); i++) {
		if (this->TYPES.at(i).name.compare(typeName) == 0) {
			found = true;
			break;
		}
	}
	if (!found) {
		if (typeSize <= 0) {
			return TYPE_ERROR;
		}
		TYPE tmp = { typeName,typeSize, outputFormat ,nullptr };
		this->TYPES.push_back(tmp);
		return TYPE_OK;
	}
	else {
		printf("[-] Type with that name already exists\n");
	}
	
	
}

int session::addFunc(std::string dllName, std::string funcName, int NumOfArgs, std::string returnType = "NULL") {
	bool found = false;
	int foundindex = 0;
	if (this->LIBRARIES.find(dllName) == this->LIBRARIES.end()) {
		return LIBRARY_NOT_LOADED;
	}
	void* funcAddr = GetProcAddress(this->LIBRARIES.find(dllName)->second, &funcName[0]);
	if (funcAddr == nullptr) {
		return FUNCTION_NOT_FOUND;
	}

	if (returnType.compare("NULL") != 0) {
		for (int i = 0; i < this->TYPES.size(); i++) {
			if (this->TYPES.at(i).name.compare(returnType) == 0) {
				found = true;
				foundindex = i;
				break;
			}
			if (!found) {
				return RETURN_TYPE_NOT_FOUND;
			}

			FUNCTION_DATA function = { funcAddr,NumOfArgs,this->TYPES.at(foundindex) };
			this->FUNCTIONS_MAP.insert(std::make_pair(funcName, function));
		}
	}
	else {
		FUNCTION_DATA function = { funcAddr,NumOfArgs,this->TYPES.at(0) /*default entry*/ };
		this->FUNCTIONS_MAP.insert(std::make_pair(funcName, function));
	}
}

int session::defineStruct(std::string structName, int numOfFields) {
	std::string fieldType;
	STRUCTURE *tmpStruct = new STRUCTURE;
	bool found = false;
	int foundindex = 0;
	int size = 0;
	for (int i = 0; i < numOfFields; i++) {
		printf("[?] Field[%d] type: ", i);
		std::getline(std::cin >> std::noskipws, fieldType);
		for (int i=0; i < this->TYPES.size(); i++) {
			if (this->TYPES.at(i).name.compare(fieldType) == 0) {
				found = true;
				foundindex = i;
				size += this->TYPES.at(i).size;
				break;
			}
		}
		if(!found){
			printf("[-] Unknown type \"%s\"\n", fieldType);
			return TYPE_NAME_NOT_FOUND;
			
		}
		else {
			tmpStruct->fields.push_back(this->TYPES.at(foundindex));
			found = false;
		}
	}
	TYPE tmpType;
	tmpType.name = structName;
	tmpType.size = size;
	tmpType.typeStruct = (void *)tmpStruct;
	tmpType.outputFormat = FORMAT_HEX; // to do, parse input
	this->TYPES.push_back(tmpType); // structure is considered as a type. As usual
	return 0;
}

int session::loadLibrary(std::string dllName) {
	HMODULE tmpHndl;
	//sanity check
	if (dllName.back() == '\\') {
		return LIBRARY_NOT_FOUND;
	}
	if (LoadLibraryA(dllName.c_str()) == NULL) {
		return LIBRARY_NOT_FOUND;
	}
	// if it was a path, let's truncate it
	while (dllName.find('\\') != std::string::npos) {
		dllName = dllName.substr(dllName.find('\\')+1);
	}
	tmpHndl = GetModuleHandleA(dllName.c_str());
	if (tmpHndl == nullptr){
		return LIBRARY_NOT_FOUND;
	}
	this->LIBRARIES.insert(std::make_pair(dllName, tmpHndl));
	return LIBRARY_FOUND;
}

void session::editType(int field, std::string newVal, std::string typeName)
{
	bool found = false;
	int foundindex = 0;
	for (int i = 0; i < this->TYPES.size(); i++) {
		if (this->TYPES.at(i).name.compare(typeName) == 0) {
			found = true;
			foundindex = i;
		}
	}
	if (!found) {
		printf("[-] Type with such name wasn't found\n");
		return;
	}
	switch (field)
	{
	case TYPE_FIELD_NAME:
		this->TYPES.at(foundindex).name = newVal;
		break;
	case TYPE_FIELD_SIZE:
		this->TYPES.at(foundindex).size = atoi(newVal.c_str());
		break;
	}
}

void session::editVar(int field, std::string newVal, std::string varName)
{
	bool found = false;
	int foundindex = 0;
	for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
		if (this->VARIABLE_LIST.at(i).name.compare(varName) == 0) {
			found = true;
			foundindex = i;
		}
	}
	if (!found) {
		printf("[-] Variable with such name wasn't found\n");
		return;
	}
	switch (field)
	{
	case TYPE_FIELD_NAME:
		this->VARIABLE_LIST.at(foundindex).name = newVal;
		break;
	case TYPE_FIELD_TYPE:
		found = false;
		for (int i = 0; i < this->TYPES.size(); i++) {
			if (this->TYPES.at(i).name.compare(newVal) == 0) {
				this->VARIABLE_LIST.at(foundindex).type = this->TYPES.at(i);
				found = true;
			}

		}
		if (!found) {
			printf("[-] Type name not found\n");
		}
		break;
	case TYPE_FIELD_VAL:
		if (processData(newVal,this->VARIABLE_LIST.at(foundindex).type.size,this->VARIABLE_LIST.at(foundindex).varAddr) != PROCESSING_OK) {
			printf("[-] Error occured while processing input\n");
		}
		break;
	case TYPE_FIELD_ADDR:
		if (processData(newVal, sizeof(uintptr_t), &(this->VARIABLE_LIST.at(foundindex).varAddr)) != PROCESSING_OK) {
			printf("[-] Error occured while processing input\n");
		}
	}
	

}

void session::editFunc(int field, std::string newVal, std::string funcName) {
	if (this->FUNCTIONS_MAP.find(funcName) != this->FUNCTIONS_MAP.end()) {
		FUNCTION_DATA &curFunc = (this->FUNCTIONS_MAP.find(funcName)->second);
		if (field == TYPE_FIELD_ARGC) {
			curFunc.numOfArgs = atoi(newVal.c_str());
		}
		else {
			bool found = false;
			for (int i = 0; i < this->TYPES.size(); i++) {
				if (this->TYPES.at(i).name.compare(newVal) == 0) {
					curFunc.ReturnType = this->TYPES.at(i);
					found = true;
					break;
				}
			}
			if (!found) {
				printf("[-] Unknown type\n");
			}
		}
	}
	else {
		printf("[-] Function wasn't found\n");
	}
}

int session::createVariable(std::string type, std::string Name, uintptr_t data) {
	//ask for input here, needs structs processing
	VARIABLE newVar;
	bool found = false;
	int foundindex = 0;
	int size = 0;
	for (int i = 0; i < this->TYPES.size(); i++) {
		if (this->TYPES.at(i).name.compare(type) == 0) {
			foundindex = i;
			found = true;
			break;
		}
	}
	if (!found) {
		return TYPE_NAME_NOT_FOUND;
	}
	if (data == 0) {
		data = (uintptr_t)std::string("").data();
	}
	size = this->TYPES.at(foundindex).size;
	void * varAddr = malloc(size);
	if (varAddr == nullptr) {
		printf("[-] Error during allocation\n");
		return -1;
	}
	memset(varAddr, 0, size); // zero dat mem
	if (this->TYPES.at(foundindex).typeStruct != nullptr) {//check if struct
		//call struct process
		if (processStructData((STRUCTURE*)(this->TYPES.at(foundindex).typeStruct), varAddr) == PROCESSING_OK) {
			newVar.name = Name;
			newVar.size = size;
			newVar.type = this->TYPES.at(foundindex);
			newVar.varAddr = varAddr;
			this->VARIABLE_LIST.push_back(newVar);
		}
	}
	else {
		//call basic var process
		if (processData(std::string((char *)data), size, varAddr) == PROCESSING_OK) {
			newVar.name = Name;
			newVar.size = size;
			newVar.type = this->TYPES.at(foundindex);
			newVar.varAddr = varAddr;
			this->VARIABLE_LIST.push_back(newVar);
		}
	}

	//check if pointer
	return 0;
}

int session::processStructData(STRUCTURE * structToFill, void* dstAddr) {
	std::string input;

	void * structPointer;
	structPointer = dstAddr;
	for (int i = 0; i < structToFill->fields.size(); i++) {
		printf("field[%d] : ", i);
		std::getline(std::cin >> std::noskipws, input); // eat newline
		if (processData(input, structToFill->fields.at(i).size, structPointer) != PROCESSING_OK) {
			return PROCESSING_ERR;
		}
		structPointer = (void*)( (uintptr_t)(structPointer) +structToFill->fields.at(i).size); // might be broken, need to debug
	}
	return 0;
}

int session::processData(std::string data, int size, void* dstAddr) {
	if (data.empty()) {
		return PROCESSING_OK;
	}
	switch (data[0])
	{
	case '0':
		//process as hex
		if (data.size() > 2) {
			if (data[1] == 'x') {
				//hex confirmed
				data = data.substr(2);
				if (data.size() % 2 == 1) {
					//preceed string with zero
					data = "0" + data;
				}
				
				int counter = 0;
				uint8_t *dstPointer = ((uint8_t*)dstAddr);
				for (int i = data.size()-1; i > 0; i-=2)
				{
					
					*(dstPointer+counter) = unxdigit(data[i-1]) * 16 + unxdigit(data[i]);
					counter++;
				}
				
				
				
			}
		}
		else {
			memset(dstAddr, 0, size);
		}
		break;
	case '"':
	case '\'':
		//process as string
		if (size > data.size() - 1) {// - 2 quotes + 1 null byte
			//string is smaller than the size of alloced var
			memcpy(dstAddr, (data.c_str() + 1)/*copy after first quote */, data.size() - 2/* -2 quotes*/);
		}
		else {//string is equal or bigger than size
			memcpy(dstAddr, (data.c_str() + 1), size-1 /* dream diary reference */);
		}
		break;
	case '*':
		//process as pointer to variable
		// search for variable;
		if (data.size() < 2) {
			printf("[-] Please provide pointed-to variable name\n");
			return PROCESSING_ERR;
		}
		for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
			if (this->VARIABLE_LIST.at(i).name.compare(&data.c_str()[1]) == 0) {
				memcpy(dstAddr, &(this->VARIABLE_LIST.at(i).varAddr), size);
				return PROCESSING_OK; //exit that loop
			}
		}
		printf("[-] Variable to point-to wasn't found\n");
		return PROCESSING_ERR;
		break;
	case ' ':
		//process as empty
		// do nothing
		break;
	default:
		//process as integer
		long long int tmpStorage = 0;
		sscanf_s(data.c_str(), "%d", &tmpStorage);
		memcpy(dstAddr, &tmpStorage, size);
		break;
	}
	return PROCESSING_OK;
}

int session::deleteVariable(std::string varName) {
	VARIABLE varToDelete;
	bool found = false;
	int foundindex = 0;
	for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
		if (this->VARIABLE_LIST.at(i).name.compare(varName) == 0) {
			varToDelete = this->VARIABLE_LIST.at(i);
			found = true;
			foundindex = i;
			break;
		}
	}
	if (!found) {
		printf("[-] Variable with that name wasn't found\n");
	}
	this->VARIABLE_LIST.erase(this->VARIABLE_LIST.begin() + foundindex);
	free(varToDelete.varAddr);
	return 0;
}



void session::printVariables()
{
	printf("[*] Currently allocated variables\n");
	VARIABLE curElement;
	for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
		curElement = this->VARIABLE_LIST.at(i);
		printf("[%d] %s %s\n", i, curElement.type.name.c_str(), curElement.name.c_str());
	}
}

void session::printVariableValue(std::string varName)
{
	VARIABLE curElement;
	bool found = false;
	int foundindex = 0;
	if (this->VARIABLE_LIST.size() == 0) {
		printf("[-] No variables were allocated\n");
		return;
	}
	for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
		if (this->VARIABLE_LIST.at(i).name.compare(varName) == 0) {
			found = true;
			foundindex = i;
			break;
		}
	}
	if (!found) {
		printf("[-] Variable with that name wasn't found\n");
	}
	curElement = this->VARIABLE_LIST.at(foundindex);
	if (curElement.type.typeStruct != nullptr) {
		STRUCTURE *tmpStruct = (STRUCTURE *)curElement.type.typeStruct;
		void * pointerAddr = curElement.varAddr;
		printf("[+] %s\n", varName.c_str());
		for (int i = 0; i < tmpStruct->fields.size(); i++) {
			printf("[+][& 0x%p][%d] = ", pointerAddr,i);
			printWithFormat(tmpStruct->fields.at(i).size, pointerAddr, tmpStruct->fields.at(i).outputFormat);
			pointerAddr = (void*)(((uintptr_t)pointerAddr) + tmpStruct->fields.at(i).size); // no support for nested structs yet
		}
	}
	else {
		printf("[+][& 0x%p] %s = ",curElement.varAddr, varName.c_str());
		printWithFormat(curElement.size, curElement.varAddr, curElement.type.outputFormat);
	}
	
	
}
void session::printFuncData(std::string funcName) {
	if (this->FUNCTIONS_MAP.find(funcName) != this->FUNCTIONS_MAP.end()) {
		FUNCTION_DATA tmpFunc = this->FUNCTIONS_MAP.find(funcName)->second;
		printf("[+] Address 0x%p \n", (uintptr_t)tmpFunc.funcAddr);
		printf("[+] Required argc %d \n", tmpFunc.numOfArgs);
		printf("[+] Return type %s \n", tmpFunc.ReturnType.name.c_str());
	}
	else {
		printf("[-] Function wasn't found\n");
	}
}
void session::printTypeData(std::string typeName) {
	bool found = false;
	for (int i = 0; i < this->TYPES.size(); i++) {
		if (this->TYPES.at(i).name.compare(typeName) == 0) {
			found = true;
			TYPE tmpType = this->TYPES.at(i);
			printf("[+] Size %d\n",tmpType.size);
			printf("[+] Output format ");
			switch (tmpType.outputFormat)
			{
			case FORMAT_HEX:
				printf("hex\n");
				break;
			case FORMAT_STRING:
				printf("str\n");
				break;
			case FORMAT_INT:
				printf("int\n");
				break;
			}
			if (tmpType.typeStruct != nullptr) {
				printf("[+] Is structure\n");
				printf("[*] Structure fields:\n");
				STRUCTURE *tmpStruct = (STRUCTURE*)tmpType.typeStruct;
				for (int j = 0; j < tmpStruct->fields.size(); j++) {
					printf("[%d] %s\n", j, tmpStruct->fields.at(i).name.c_str());
				}
			}

		}
	}
	if (!found) {
		printf("[-] Type wasn't found");
	}
}


void session::printWithFormat(int size, void * varAddr, int OutputFormat) {
	switch (OutputFormat)
	{
	case FORMAT_HEX:
		printf("0x");
		for (signed int i = size-1; i >= 0; i--)
		{
			printf("%02x", (unsigned)((uint8_t*)varAddr)[i]);
		}
		printf("\n");
		break;

	case FORMAT_STRING:
		//output string, needs additional checks to preserve instance from segfaults
		printf("%s\n",  varAddr);
		break;

	case FORMAT_INT:
		//output integer
		printf("%d\n",  *((uintptr_t *)(varAddr)));
		break;
	}
}
void session::printLoadedLibs()
{
	if (this->LIBRARIES.size() == 0) {
		printf("[-] No libraries were loaded\n");
		return;
	}
	for (const auto &curVal : this->LIBRARIES)
	{
		printf("[*] %s\n", (curVal.first).c_str());
	}
}

void session::printTypes()
{
	TYPE curElement;
	if (this->TYPES.size() == 0) {
		printf("[-] No types were defined\n");
		return;
	}

	for (int i = 0; i < this->TYPES.size(); i++) {
		curElement = this->TYPES.at(i);
		printf("[*] %s %d\n", curElement.name.c_str(), curElement.size);
	}
}

void session::printFuctions()
{
	if (this->FUNCTIONS_MAP.size() == 0) {
		printf("[-] No functions were resolved\n");
		return;
	}
	for (const auto &curVal : this->FUNCTIONS_MAP)
	{
		printf("[*] %s\n", curVal.first.c_str());
	}
}

void session::callWrapper(std::string funcName)
{
	bool isTokenized = false;
	bool found = false;
	bool passByRef = false;
	FUNCTION_DATA callFunc;
	std::string inputString;
	std::istringstream strStream;
	std::vector<std::string> args;
	std::vector<void *> parsedArgs;
	std::string arg;
	uintptr_t ret_val = 0;

	if (this->FUNCTIONS_MAP.find(funcName) != this->FUNCTIONS_MAP.end()) {
		callFunc = this->FUNCTIONS_MAP.find(funcName)->second;
		if (callFunc.numOfArgs != 0) {
			printf("[?] Arguments for function: ");
			std::getline(std::cin, inputString);
			strStream.str(inputString);
			while (std::getline(strStream,arg,' ')) {
				args.push_back(arg);
				isTokenized = true;
			}
			if (!isTokenized) {
				args.push_back(inputString);
			}
			if (args.size() < callFunc.numOfArgs || args.size() > callFunc.numOfArgs) {
				printf("[-] Wrong amount of arguments for function. %d required, %d provided", callFunc.numOfArgs, args.size());
			}
			isTokenized = false;
			//parse args
			for (int i = 0; i < args.size(); i++) {
				if (args.at(i).compare("0") == 0) {
					// put 0
					parsedArgs.push_back(nullptr);
				}
				else {
					if (args.at(i)[0] == '&') {
						passByRef = true;
						args.at(i) = args.at(i).substr(1);
					}
					for (int j = 0; j < this->VARIABLE_LIST.size(); j++) {
						if (this->VARIABLE_LIST.at(j).name.compare(args.at(i)) == 0) {
							if (passByRef) {
								parsedArgs.push_back(&(this->VARIABLE_LIST.at(j).varAddr));
							}
							else {
								parsedArgs.push_back(this->VARIABLE_LIST.at(j).varAddr);
							}
							
							found = true;
						}
					}
					if (!found) {
						printf("[-] Unknown variable name\n");
						return;
					}
					found = false;
				}
			}
			ret_val = caller::preparedCall(callFunc.funcAddr, parsedArgs.data(), parsedArgs.size());
		}
		else {
			ret_val = caller::preparedCall(callFunc.funcAddr, nullptr, 0);
		}
		if (callFunc.ReturnType.name.compare("DEFAULT") != 0) {
			// push variable with return value and print it
			std::string varName = "return_" + funcName;
			for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
				if (this->VARIABLE_LIST.at(i).name.compare(varName) == 0) {
					this->VARIABLE_LIST.erase(this->VARIABLE_LIST.begin() + i);
					break;
				}
			}
			createVariable(callFunc.ReturnType.name, varName,(uintptr_t )((std::to_string((long long int)ret_val)).c_str()));
		}
		printf("[+] Output value %d\n", ret_val); //later custom type

	}
}

void session::execShellcode(void *shellCodeAddr, int size, bool noExec) {
	std::vector<void*> args;
	uintptr_t ProtFlag = 0x40; // PAGE EXECUTE REDWRITE
	uintptr_t MemFlag = 0x1000 | 0x2000;
	uintptr_t dstAddr = 0x0;
	uintptr_t newSize = (uintptr_t)size; //needs to be expanded
	DWORD junk = 0;
	//load kernel32
	printf("[*] Attempting to load kernel32\n");
	loadLibrary("kernel32.dll");
	//resolve VirtualAlloc
	printf("[*] Resolving VirtualAlloc\n");
	addFunc("kernel32.dll", "VirtualAlloc", size, "NULL");
	//prepare args
	args.push_back(&dstAddr);  
	args.push_back(&newSize);
	args.push_back(&MemFlag);
	args.push_back(&ProtFlag);
	printf("[*] Calling VirtualAlloc\n");
	uintptr_t newLoc  = caller::preparedCall(this->FUNCTIONS_MAP.find("VirtualAlloc")->second.funcAddr, args.data(), 4);
	if ((void *)newLoc == nullptr) {
		printf("[-] Fail during memory allocation for shellcode\n");
		return;
	}
	printf("[+] New executable shellcode location 0x%p\n", newLoc);
	uintptr_t locPointer = newLoc;
	for (int i = size-1; i >= 0; i--)//copy reversed
	{
		*(uint8_t*)newLoc++ = ((uint8_t *)shellCodeAddr)[i];
	}
	if (noExec) {
		return;
	}
	printf("[+] Executing shellcode\n");
	caller::shellcodeCall((void *)locPointer);
}
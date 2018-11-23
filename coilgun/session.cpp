#include "stdafx.h"
#include "session.h"

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
	TYPE *tmpType;
	if (typeName.empty()) {
		printf("[-] Name can't be empty\n");
		return TYPE_ERROR;
	}
	else if (checkIfNumber(typeName[0])) {
		printf("[-] Name can't start with digit\n");
		return TYPE_ERROR;
	}

	if (getTypeByName(typeName) == 0){
		if (typeSize <= 0) {
			return TYPE_ERROR;
		}
		TYPE tmp = { typeName,typeSize, outputFormat , nullptr };
		this->TYPES.push_back(tmp);
		return TYPE_OK;
	}
	else {
		printf("[-] Type with that name already exists\n");
	}
	
	
}

int session::addFunc(std::string dllName, std::string funcName, int NumOfArgs, std::string returnType) {
	
	TYPE *tmpType;
	if (this->LIBRARIES.find(dllName) == this->LIBRARIES.end()) {
		return LIBRARY_NOT_LOADED;
	}
	void* funcAddr = GetProcAddress(this->LIBRARIES.find(dllName)->second, &funcName[0]);
	if (funcAddr == nullptr) {
		return FUNCTION_NOT_FOUND;
	}

	if (returnType.compare("NULL") != 0) {
		if(getTypeByName(returnType)!=0){
			tmpType = (TYPE*)getTypeByName(returnType);
			FUNCTION_DATA function = { funcAddr,NumOfArgs,tmpType };
			this->FUNCTIONS_MAP.insert(std::make_pair(funcName, function));
		}
		else{
			return RETURN_TYPE_NOT_FOUND;
		}
	}
	else {
		FUNCTION_DATA function = { funcAddr,NumOfArgs,&(this->TYPES.at(0)) /*default entry*/ };
		this->FUNCTIONS_MAP.insert(std::make_pair(funcName, function));
	}
}

int session::defineStruct(std::string structName, std::vector<std::string> fieldTypes) {
	std::string fieldType;
	STRUCTURE *tmpStruct = new STRUCTURE;
	TYPE* tmpType;
	bool found = false;
	int size = 0;
	if (structName.empty()) {
		printf("[-] Name can't be empty\n");
		return TYPE_ERROR;
	}
	else if (checkIfNumber(structName[0])) {
		printf("[-] Name can't start with digit\n");
		return TYPE_ERROR;
	}
	if (fieldTypes.size() == 0) {
		printf("[-] Struct can't be empty\n");
		return TYPE_NAME_NOT_FOUND;
	}
	for (int i = 0; i < fieldTypes.size(); i++) {
		found = false;
		if (getTypeByName(fieldTypes.at(i)) != 0) {
			found = true;
			tmpType = (TYPE *)getTypeByName(fieldTypes.at(i));
			tmpStruct->fields.push_back(*tmpType);
			size += tmpType->size;
		}
		if (!found) {
			printf("[-] Unknown type \"%s\"\n", fieldType.c_str());
			return TYPE_NAME_NOT_FOUND;

		}
	}
	

	TYPE newStruct;
	newStruct.name = structName;
	newStruct.size = size;
	newStruct.typeStruct = (void *)tmpStruct;
	newStruct.outputFormat = FORMAT_HEX; // to do, parse input
	this->TYPES.push_back(newStruct); // structure is considered as a type. As usual
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
	TYPE *tmpType= nullptr;
	if (getTypeByName(typeName) != 0) {
		found = true;
		tmpType = (TYPE *)getTypeByName(typeName);
	}
	if (!found) {
		printf("[-] Type with such name wasn't found\n");
		return;
	}
	switch (field)
	{
	case TYPE_FIELD_NAME:
		if (!newVal.empty()) {
			if (!checkIfNumber(newVal[0])) {
				tmpType->name = newVal;
			}
			else {
				printf("[-] Name can't start with digit\n");
			}
		}
		else {
			printf("[-] Name can't be empty\n");
		}
	case TYPE_FIELD_SIZE:
		tmpType->size = atoi(newVal.c_str());
		break;
	case TYPE_FIELD_FMT:
		if (newVal.compare("hex") == 0) {
			tmpType->outputFormat = FORMAT_HEX;
		}
		else if (newVal.compare("str") == 0) {
			tmpType->outputFormat = FORMAT_STRING;
		}
		else if (newVal.compare("int") == 0) {
			tmpType->outputFormat = FORMAT_INT;
		}
		break;
	}
	
}

void session::editVar(int field, std::string newVal, std::string varName)
{
	bool found = false;
	VARIABLE *tmpVar = nullptr;
	if (getVarByName(varName) != 0) {
		tmpVar = (VARIABLE*)getVarByName(varName);
		found = true;
	}
	if (!found) {
		printf("[-] Variable with such name wasn't found\n");
		return;
	}
	switch (field)
	{
	case TYPE_FIELD_NAME:
		//sanity check
		if (!newVal.empty()) {
			if (!checkIfNumber(newVal[0])) {
				tmpVar->name = newVal;
			}
			else {
				printf("[-] Name can't start with digit\n");
			}
		}
		else {
			printf("[-] Name can't be empty\n");
		}
		

		break;
	case TYPE_FIELD_TYPE:
		found = false;
		if (getTypeByName(newVal) != 0) {
			found = true;
			tmpVar->type = (TYPE*)getTypeByName(newVal);
		}
		if (!found) {
			printf("[-] Type name not found\n");
		}
		break;
	case TYPE_FIELD_VAL:
		if (processData(newVal,tmpVar->size,tmpVar->varAddr) != PROCESSING_OK) {
			printf("[-] Error occured while processing input\n");
		}
		break;
	case TYPE_FIELD_ADDR:
		if (processData(newVal, sizeof(uintptr_t), &(tmpVar->varAddr)) != PROCESSING_OK) {
			printf("[-] Error occured while processing input\n");
		}
	case TYPE_FIELD_FIELD:
		//check if struct;
		if (tmpVar->type->typeStruct == NULL) {
			printf("[-] Variable is not a structure\n");
		}
		//unpack data
		int fieldNum = atoi(newVal.substr(0, newVal.find_first_of('!')).c_str());
		newVal = newVal.substr(newVal.find_first_of('!') + 1);
		//find offset
		uintptr_t basePointer = (uintptr_t)tmpVar->varAddr;
		STRUCTURE *structData = (STRUCTURE *)tmpVar->type->typeStruct;
		long long int offset = 0;
		if (fieldNum > structData->fields.size() - 1) {
			printf("[-] field number out of bounds\n");
			return;
		}
		for (int i = 0; i < fieldNum; i++) {
			offset += structData->fields.at(i).size;
		}
		
		//sum pointer and offset
		basePointer += offset;
		if (processData(newVal, structData->fields.at(fieldNum).size, (void *) basePointer) != PROCESSING_OK) {
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
			if(getTypeByName(newVal) != 0){
				found = true;
				curFunc.ReturnType = (TYPE *)getTypeByName(newVal);
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

int session::createVariable(std::string type, std::string Name, std::vector<std::string> values) {
	VARIABLE newVar;
	TYPE *tmpType = nullptr;
	bool found = false;
	int size = 0;
	uintptr_t data;
	if (Name.empty()) {
		printf("[-] Name can't be empty\n");
		return TYPE_ERROR;
	}
	else if (checkIfNumber(Name[0])) {
		printf("[-] Name can't start with digit\n");
		return TYPE_ERROR;
	}
	if (getTypeByName(type) != 0) {
		found = true;
		tmpType = (TYPE*)getTypeByName(type);
	}
	if (!found) {
		return TYPE_NAME_NOT_FOUND;
	}
	if (values.empty()) {
		data = (uintptr_t)std::string("").data();
	}
	else {
		data = (uintptr_t)values.at(0).data();
	}
	size = tmpType->size;
	size_t modSize = size;
	if (size < 8) {//expand variable to 64 bits
		modSize = 8;
	}
	void * varAddr = malloc(modSize);
	if (varAddr == nullptr) {
		printf("[-] Error during allocation\n");
		return -1;
	}
	memset(varAddr, 0, modSize); // zero dat mem
	if (tmpType->typeStruct != nullptr) {//check if struct
		//call struct process
		if (processStructData((STRUCTURE*)tmpType->typeStruct, varAddr,values) == PROCESSING_OK) {
			newVar.name = Name;
			newVar.size = size;
			newVar.type = tmpType;
			newVar.varAddr = varAddr;
			this->VARIABLE_LIST.push_back(newVar);
		}
	}
	else {
		//call basic var process
		if (processData(std::string((char *)data), size, varAddr) == PROCESSING_OK) {
			newVar.name = Name;
			newVar.size = size;
			newVar.type = tmpType;
			newVar.varAddr = varAddr;
			this->VARIABLE_LIST.push_back(newVar);
		}
	}

	//check if pointer
	return 0;
}

int session::processStructData(STRUCTURE * structToFill, void* dstAddr, std::vector<std::string> values) {
	std::string input;
	std::size_t valuesSize = values.size();
	if (values.empty()) {
		return PROCESSING_OK;
	}
	void * structPointer;
	structPointer = dstAddr;
	for (int i = 0; i < structToFill->fields.size(); i++) {
		if (i > valuesSize - 1) {
			break; //less values than needed, but it's ok
		}
		if (processData(values.at(i), structToFill->fields.at(i).size, structPointer) != PROCESSING_OK) {
			return PROCESSING_ERR;
		}
		structPointer = (void*)( (uintptr_t)(structPointer) +structToFill->fields.at(i).size); // might be broken, need to debug
	}
	return PROCESSING_OK;
}

int session::processData(std::string data, int size, void* dstAddr) {
	if (data.empty()) {
		return PROCESSING_OK;
	}
	memset(dstAddr, 0, size);
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
		data = data.substr(1);
		if (getVarByName(data) != 0) {
			memcpy(dstAddr, &(((VARIABLE *)(getVarByName(data)))->varAddr), size);
			return PROCESSING_OK; //exit that loop

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
		printf("[%d] %s %s\n", i, curElement.type->name.c_str(), curElement.name.c_str());
	}
}

void session::printVariableValue(std::string varName)
{
	VARIABLE *curElement;
	bool found = false;
	if (this->VARIABLE_LIST.size() == 0) {
		printf("[-] No variables were allocated\n");
		return;
	}
	if(getVarByName(varName)!=0){
		found = true;
	}
	if (!found) {
		printf("[-] Variable with that name wasn't found\n");
		return;
	}
	
	curElement = (VARIABLE *)getVarByName(varName);
	if (curElement->type->typeStruct != nullptr) {
		STRUCTURE *tmpStruct = (STRUCTURE *)curElement->type->typeStruct;
		void * pointerAddr = curElement->varAddr;
		printf("[+] %s\n", varName.c_str());
		for (int i = 0; i < tmpStruct->fields.size(); i++) {
			printf("[+][& 0x%p][%d] = ", pointerAddr,i);
			printWithFormat(tmpStruct->fields.at(i).size, pointerAddr, tmpStruct->fields.at(i).outputFormat);
			pointerAddr = (void*)(((uintptr_t)pointerAddr) + tmpStruct->fields.at(i).size); // no support for nested structs yet
		}
	}
	else {
		printf("[+][& 0x%p] %s = ",curElement->varAddr, varName.c_str());
		printWithFormat(curElement->size, curElement->varAddr, curElement->type->outputFormat);
	}
	
	
}
void session::printFuncData(std::string funcName) {
	if (this->FUNCTIONS_MAP.find(funcName) != this->FUNCTIONS_MAP.end()) {
		FUNCTION_DATA tmpFunc = this->FUNCTIONS_MAP.find(funcName)->second;
		printf("[+] Address 0x%p \n", (uintptr_t)tmpFunc.funcAddr);
		printf("[+] Required argc %d \n", tmpFunc.numOfArgs);
		printf("[+] Return type %s \n", tmpFunc.ReturnType->name.c_str());
	}
	else {
		printf("[-] Function wasn't found\n");
	}
}
void session::printTypeData(std::string typeName) {
	bool found = false;
	if (getTypeByName(typeName) != 0) {
		found = true;
		TYPE* tmpType =(TYPE*) getTypeByName(typeName);
		printf("[+] Size %d\n", tmpType->size);
		printf("[+] Output format ");
		switch (tmpType->outputFormat)
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
		if (tmpType->typeStruct != nullptr) {
			printf("[+] Is structure\n");
			printf("[*] Structure fields:\n");
			STRUCTURE *tmpStruct = (STRUCTURE*)tmpType->typeStruct;
			for (int j = 0; j < tmpStruct->fields.size(); j++) {
				printf("[%d] %s\n", j, tmpStruct->fields.at(j).name.c_str());
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

uintptr_t session::getVarByName(std::string varName) {
	for (int i = 0; i < this->VARIABLE_LIST.size(); i++) {
		if (this->VARIABLE_LIST.at(i).name.compare(varName) == 0) {
			return (uintptr_t)&this->VARIABLE_LIST.at(i);
		}
	}
	return 0;
}

uintptr_t session::getTypeByName(std::string TypeName) {
	
	for (int i = 0; i < this->TYPES.size(); i++) {
		if (this->TYPES.at(i).name.compare(TypeName) == 0) {
			return (uintptr_t)&this->TYPES.at(i);
		}
	}
	return 0;
}

bool session::checkIfNumber(char testChar)
{
	if (testChar <= 57 && testChar >= 48) {//ascii codes for digits
		return true;
	}
	return false;
	
}

void session::callWrapper(std::string funcName,std::vector<std::string> args)
{
	bool isTokenized = false;
	bool found = false;
	bool passByRef = false;
	FUNCTION_DATA callFunc;
	std::string inputString;
	std::istringstream strStream;
	std::vector<void *> parsedArgs;
	std::string arg;
	uintptr_t ret_val = 0;

	if (this->FUNCTIONS_MAP.find(funcName) != this->FUNCTIONS_MAP.end()) {
		callFunc = this->FUNCTIONS_MAP.find(funcName)->second;
		if (callFunc.numOfArgs != 0) {
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
					if (getVarByName(args.at(i)) != 0) {
						VARIABLE *tmpVar = (VARIABLE *)getVarByName(args.at(i));
						found = true;
						if (passByRef) {
							parsedArgs.push_back(&(tmpVar->varAddr));
						}
						else {
							parsedArgs.push_back(tmpVar->varAddr);
						}
					}
				}
				if (!found) {
					printf("[-] Unknown variable name\n");
					return;
				}
				found = false;
			}

			ret_val = caller::preparedCall(callFunc.funcAddr, parsedArgs.data(), parsedArgs.size());
		}
		else {
			ret_val = caller::preparedCall(callFunc.funcAddr, nullptr, 0);
		}
	}
	else {
		printf("[-] Function wasn't found\n");
		return;
	}

	// push variable with return value and print it
	std::string varName = "return_" + funcName;
	if (getVarByName(varName) == 0) {
		std::vector<std::string> args;
		args.push_back(((std::to_string((long long int)ret_val)).c_str()));
		createVariable(callFunc.ReturnType->name, varName, args);
	}
	else {
		VARIABLE* retVar = (VARIABLE *)getVarByName(varName);
		memcpy(retVar->varAddr, &ret_val, callFunc.ReturnType->size);
	}


	printf("[+] Output value %d\n", ret_val); //later custom type

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
	//push location in variable list. needed for scripting
	if (getVarByName("SHELLC_LOCATION") == 0) {
		std::vector<std::string> varArgs;
		varArgs.push_back(std::string("0xdeadbeef").c_str());
		createVariable("DEFAULT", "SHELLC_LOCATION",varArgs);
	}
	VARIABLE *locVar = (VARIABLE *)getVarByName("SHELLC_LOCATION");
	memcpy(locVar->varAddr, &locPointer, sizeof(uintptr_t));
	
	if (noExec) {
		return;
	}
	printf("[+] Executing shellcode\n");

	std::thread shellcThr = std::thread(caller::shellcodeCall, (void*)locPointer);
	
	if (shellcThr.joinable()) {
		shellcThr.detach(); //avoid exceptions
	}

	caller::shellcodeCall((void *)locPointer);
}
#pragma once
#include <process.h>
#include <iostream>
#include <sstream>
#include "session.h"
#include "console.h"
#include "caller.h"

class console
{
public:
	console();
	void poll();
	void parseInput(std::vector<std::string> tokes);
	std::vector<std::string> delimitString(std::string);
	bool quickCallParse(std::string callCommand);
	void exit();
	void drop2Shell();
	void printAscii();//kek
	void transformLower(std::string &toTransform);
	~console();
private:
	session curSession = session();
};


#pragma once
#include <process.h>
#include <iostream>
#include "session.h"
#include <sstream>
class console
{
public:
	console();
	void poll();
	void parseInput(std::vector<std::string> tokes);
	std::vector<std::string> delimitString(std::string);
	void printHelp();
	void exit();
	void drop2Shell();
	void printAscii();//kek
	void transformLower(std::string &toTransform);
	~console();
private:
	session curSession = session();
};


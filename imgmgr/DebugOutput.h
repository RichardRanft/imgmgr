#pragma once
#include <string>
#include <stdarg.h>

class CDebugOutput
{
public:
	CDebugOutput();
	~CDebugOutput();

	std::string composeMessage(int, ...);
};


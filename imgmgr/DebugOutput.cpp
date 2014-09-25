#include "DebugOutput.h"

CDebugOutput::CDebugOutput()
{
}

CDebugOutput::~CDebugOutput()
{
}

std::string CDebugOutput::composeMessage(int count, ...)
{
	std::string message;

	va_list msg;

	va_start(msg, count);
	for (int i = 0; i < count; i++)
	{
		message.append(va_arg(msg, char*));
	}
	va_end(msg);

	return message;
}

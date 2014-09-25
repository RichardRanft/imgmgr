#include "ResourceInstance.h"
#include <Windows.h>


CResourceInstance::CResourceInstance()
{
	m_fileName = "";
	m_properties.m_height = 0;
	m_properties.m_width = 0;
	m_properties.m_size = 0;
}

CResourceInstance::~CResourceInstance()
{
}

bool CResourceInstance::loadAsset(string& fileName)
{
	return false;
}

void CResourceInstance::refreshAsset()
{
	// invalidate the current PNG file and initialize the new file.
}

void CResourceInstance::getWorkingPath(char* destBuff)
{
	wchar_t pathbuff[MAX_PATH];
	//cout << " ** getting current working directory ..." << endl;
	GetCurrentDirectoryW(sizeof(char)* MAX_PATH, pathbuff);
	if (pathbuff == NULL)
	{
		cout << " ** CResourceInstance::getWorkingPath : Unable to get working directory." << endl;
		return;
	}
	unsigned int strSize = sizeof(char)* MAX_PATH;
	int maxLen = MAX_PATH;
	wcstombs(destBuff, pathbuff, maxLen);
}

void CResourceInstance::getWorkingPath(string& destBuff)
{
	wchar_t pathbuff[MAX_PATH];
	//cout << " ** getting current working directory ..." << endl;
	GetCurrentDirectoryW(sizeof(char)* MAX_PATH, pathbuff);
	if (pathbuff == NULL)
	{
		cout << " ** CResourceInstance::getWorkingPath : Unable to get working directory." << endl;
		return;
	}
	unsigned int strSize = sizeof(char)* MAX_PATH;
	int maxLen = MAX_PATH;
	char temp[MAX_PATH];
	wcstombs(temp, pathbuff, maxLen);
	destBuff.append(temp);
}

SImageProperties CResourceInstance::getImageSize()
{
	return m_properties;
}

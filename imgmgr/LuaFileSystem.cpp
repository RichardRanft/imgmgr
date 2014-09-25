#include "LuaFileSystem.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <iterator>

using namespace std;

// Internal
bool CLuaFileSystem::split(string* str, vector<string*>* dest, const char* delim)
{
	//cout << " !! splitting " << str->c_str();
	char* nextTok;
	char* raw = (char*)str->c_str();
	char* token = strtok_s(raw, delim, &nextTok);
	string whitespaces(" \t\f\v\n\r");
	string tok;
	string result;
	while (token != NULL)
	{
		tok = token;
		result = token;
		size_t found = result.find_last_not_of(whitespaces);
		if (found != string::npos)
			result.erase(found + 1);
		else
		{
			result.clear();
			break;
		}
		dest->push_back(new string(result));
		token = strtok_s(NULL, delim, &nextTok);
	}
	return true;
}

bool CLuaFileSystem::getDirectoryList(string* startFolder, vector<string*>* output)
{
	char psBuffer[_MAX_PATH];
	string command = "dir ";
	command.append(startFolder->c_str());
	FILE* data = _popen(command.c_str(), "rt");
	vector<string*> lineParts;
	string dOut;
	if (data != NULL)
	{
		int line = 0;
		int col = 0;
		while (fgets(psBuffer, _MAX_PATH, data))
		{
			OutputDebugString(psBuffer);
			OutputDebugString("\n");
			line++;
			if (line > 7)
			{
				string* templine = new string(psBuffer);
				col = templine->find("<DIR>");
				lineParts.clear();
				if (col != string::npos)
				{
					string fileName;
					dOut = " ** getDirectoryList : ";
					dOut.append(fileName);
					dOut.append("\n");
					OutputDebugString(dOut.c_str());
					split(templine, &lineParts, " ");
					//cout << " ** lineParts.size() = " << lineParts.size() << endl;
					if (lineParts.size() > 4)
					{
						int parts = lineParts.size();
						int currPart = 4;
						while (currPart < parts)
						{
							fileName.append(lineParts[currPart++]->c_str());
							if (parts > 5 && currPart < parts)
								fileName.append(" ");
						}
					}
					if (fileName != "")
						output->push_back(new string(fileName));
				}
			}
		}
		return true;
	}
	return false;
}

bool CLuaFileSystem::getFilesInDir(string* startFolder, vector<string*>* output)
{
	char psBuffer[_MAX_PATH];
	string command = "dir ";
	command.append(startFolder->c_str());
	//cout << " !! command = " << command << endl;
	FILE* data = _popen(command.c_str(), "rt");
	vector<string*> lineParts;
	string dOut;
	if (data != NULL)
	{
		int line = 0;
		int col = 0;
		while (fgets(psBuffer, _MAX_PATH, data))
		{
			OutputDebugString(psBuffer);
			OutputDebugString("\n");
			line++;
			if (line > 2)
			{
				string* templine = new string(psBuffer);
				col = templine->find("<DIR>");
				lineParts.clear();
				if (col == string::npos)
				{
					string fileName;
					dOut = " ** getFilesInDir : ";
					dOut.append(psBuffer);
					dOut.append("\n");
					OutputDebugString(dOut.c_str());
					split(templine, &lineParts, " ");
					//cout << " ** lineParts.size() = " << lineParts.size() << endl;
					if (lineParts.size() > 4)
					{
						int parts = lineParts.size();
						int currPart = 4;
						while (currPart < parts)
						{
							//cout << " ** adding " << lineParts[currPart]->c_str() << endl;
							fileName.append(lineParts[currPart++]->c_str());
							if (parts > 5 && currPart < parts)
								fileName.append(" ");
						}
					}
					//cout << " ** filename = " << fileName << endl;
					if (fileName != "")
						output->push_back(new string(fileName));
				}
			}
		}
		return true;
	}
	return false;
}

// Lua interface
CLuaFileSystem::CLuaFileSystem(lua_State* L)
{
	m_luaState = L;
	isPrecious = false;
	//cout << "CLuaFileSystem created" << endl;
}

CLuaFileSystem::~CLuaFileSystem()
{
	m_luaState = nullptr;
	//cout << "CLuaFileSystem destroyed" << endl;
}

int CLuaFileSystem::getInput(lua_State* L)
{
	//cout << " ** in LuaIOManager::get" << endl;
	string buffer;
	getline(cin, buffer);
	//cout << " ** received: " << buffer.c_str() << endl;
	lua_pushstring(L, buffer.c_str());
	return 1;
}

int CLuaFileSystem::getFileList(lua_State* L)
{
	// get file list and push to a lua table
	int top = lua_gettop(L); // should be starting path
	string text = "";
	if (lua_isstring(L, top))
		text = lua_tostring(L, top);
	// create Lua table
	lua_newtable(L);
	vector<string*>* fileList = new vector<string*>();
	getFilesInDir(&text, fileList);
	int i = 0;
	for (vector<string*>::iterator it = fileList->begin(); it != fileList->end(); it++)
	{
		if (it == fileList->end() - 1)
			break;
		//cout << " ** getfileList : " << (*it)->c_str() << endl;
		lua_pushnumber(L, i++);
		lua_pushstring(L, (char*)(*it)->c_str());
		lua_settable(L, -3);
	}
	return 1;
}

int CLuaFileSystem::getDirList(lua_State* L)
{
	// get directory list and push to a lua table
	//cout << " ** entering getDirList..." << endl;
	int top = lua_gettop(L); // should be starting path
	string text = "";
	if (lua_isstring(L, top))
		text = lua_tostring(L, top);
	// create Lua table
	lua_newtable(L);
	vector<string*>* dirList = new vector<string*>();
	getDirectoryList(&text, dirList);
	int i = 0;
	for (vector<string*>::iterator it = dirList->begin(); it != dirList->end(); it++)
	{
		//cout << " ** getDirList : " << (*it)->c_str() << endl;
		lua_pushnumber(L, i++);
		lua_pushstring(L, (char*)(*it)->c_str());
		lua_settable(L, -3);
	}
	return 1;
}

// implementation
const char *CLuaFileSystem::className = "CLuaFileSystem";
const Luna < CLuaFileSystem >::FunctionType CLuaFileSystem::methods[] = {
	{ "getInput", &CLuaFileSystem::getInput },
	{ "getFileList", &CLuaFileSystem::getFileList },
	{ "getDirList", &CLuaFileSystem::getDirList },
	{ 0 }
};
const Luna < CLuaFileSystem >::PropertyType CLuaFileSystem::properties[] = {
	{ 0 }
};

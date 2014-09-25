#include "stdafx.h"
#include "Application.h"
#include "LuaFileSystem.h"
#include "ResourceManager.h"
#include "JPEGFile.h"
#include "PNGFile.h"
#include "BMPFile.h"
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

CApplication::CApplication()
{
	m_debugOut = new CDebugOutput();
}

CApplication::~CApplication()
{
}

static void l_message(const char *pname, const char *msg) {
	if (pname) luai_writestringerror("%s: ", pname);
	luai_writestringerror("%s\n", msg);
}

// Start the application.
bool CApplication::Run()
{
	string dOut = m_debugOut->composeMessage(1, " -- Running imgmgr...\n");
	OutputDebugString(dOut.c_str());
	lua_State *L = luaL_newstate();  /* create state */
	if (!startLua(L))
	{
		l_message("imgmgr", "cannot create Lua state: not enough memory");
		return EXIT_FAILURE;
	}

	loadScripts(L);

	lua_close(L);
	
	dOut = m_debugOut->composeMessage(1, " -- imgmgr complete\n");
	OutputDebugString(dOut.c_str());
	return false;
}

// Start the application.
bool CApplication::Run(const string* fileName)
{
	string dOut = m_debugOut->composeMessage(1, " -- Running imgmgr...\n");
	OutputDebugString(dOut.c_str());
	lua_State *L = luaL_newstate();  /* create state */
	if (!startLua(L))
	{
		l_message("imgmgr", "cannot create Lua state: not enough memory");
		return EXIT_FAILURE;
	}
	if (fileName->size() == 0)
	{
		cout << " -- imgmgr - passed null file name." << endl;
		return EXIT_FAILURE;
	}
	
	dOut = m_debugOut->composeMessage(3, " -- Running script : ", fileName->c_str(), "\n");
	OutputDebugString(dOut.c_str());

	int res = runScript(L, *fileName);

	lua_close(L);

	dOut = m_debugOut->composeMessage(1, " -- imgmgr complete\n");
	OutputDebugString(dOut.c_str());

	return EXIT_SUCCESS;
}

// Start the application.
bool CApplication::Run(int count, const char** args)
{
	lua_State *L = luaL_newstate();  /* create state */
	if (!startLua(L))
	{
		l_message("imgmgr", "cannot create Lua state: not enough memory");
		return EXIT_FAILURE;
	}
	if (args == nullptr)
	{
		cout << " -- imgmgr - passed empty command line." << endl;
		return EXIT_FAILURE;
	}

	int res = runScript(L, count, args);

	lua_close(L);

	return EXIT_SUCCESS;
}

bool CApplication::startLua(lua_State* L)
{
	string dOut = m_debugOut->composeMessage(1, " -- Starting Lua...\n");
	OutputDebugString(dOut.c_str());

	if (L == NULL)
		return false;

	luaL_checkversion(L);

	lua_gc(L, LUA_GCSTOP, 0);
	luaL_openlibs(L);
	lua_gc(L, LUA_GCRESTART, 0);

	if (L == NULL)
		return false;

	m_luaState = L;

	registerLuaClasses();

	return true;
}

void CApplication::registerLuaClasses()
{
	Luna<CLuaFileSystem>::Register(m_luaState);
	Luna<CResourceManager>::Register(m_luaState);
}

void CApplication::getScriptFileName(char* str, string& dest, const char* delim)
{
	char* nextTok;
	char* token = strtok_s(str, delim, &nextTok);
	string whitespaces(" \t\f\v\n\r");
	string tok;
	string result;

	while (token != NULL)
	{
		tok = token;
		if (tok.find(".lua") != string::npos)
		{
			result = token;
			size_t found = result.find_last_not_of(whitespaces);
			if (found != string::npos)
				result.erase(found + 1);
			else
				result.clear();			break;
		}
		token = strtok_s(NULL, delim, &nextTok);
	}
	dest = result.c_str();
}

bool CApplication::getFileList(string *dirName, vector<string*>* target)
{
	char psBuffer[128];
	string command = "dir ";
	command.append(dirName->c_str());
	FILE* data = _popen(command.c_str(), "rt");
	if (data != NULL)
	{
		int line = 0;
		while (fgets(psBuffer, 128, data))
		{
			line++;
			if (line > 7)
			{
				string* templine = new string(psBuffer);
				if (templine->find(".lua") != string::npos)
				{
					string fileName;
					getScriptFileName(psBuffer, fileName, " ");
					if (fileName != "")
						target->push_back(new string(fileName));
				}
			}
		}
		return true;
	}
	return false;
}

bool CApplication::runScript(lua_State* L, string filename)
{
	string dOut = m_debugOut->composeMessage(3, " -- Running ", filename.c_str(), "\n");
	OutputDebugString(dOut.c_str());

	bool result = false;

	result = luaL_dofile(L, filename.c_str());

	dOut = m_debugOut->composeMessage(4, " -- ", filename.c_str(), " exited ", (result == true ? "true\n" : "false\n") );
	OutputDebugString(dOut.c_str());
	return result;
}

bool CApplication::runScript(lua_State* L, int argCount, const char** args)
{
	lua_pushinteger(L, argCount);  /* 1st argument */
	lua_pushlightuserdata(L, args); /* 2nd argument */
	int status = lua_pcall(L, 2, 1, 0);
	int result = lua_toboolean(L, -1);  /* get result */

	return (result == 0 ? false : true);
}

bool CApplication::loadScripts(lua_State* L)
{
	vector<string*>* data = new vector<string*>();
	string* dir = new string(".");
	getFileList(dir, data);
	string fileName = "";

	string dOut = m_debugOut->composeMessage(1, " -- Begin loading scripts...\n");
	OutputDebugString(dOut.c_str());

	if (data != NULL && data->size() > 0)
	{
		for (vector<string*>::iterator i = data->begin(); i != data->end(); i++)
		{
			fileName = dir->c_str();
			fileName.append("/");
			fileName.append(**i);
			
			dOut = m_debugOut->composeMessage(3, " -- Loading ", fileName.c_str(), "\n");
			OutputDebugString(dOut.c_str());

			runScript(L, fileName.c_str());
		}
		dOut = m_debugOut->composeMessage(1, " -- Scripts loaded\n");
		OutputDebugString(dOut.c_str());
		return true;
	}

	dOut = m_debugOut->composeMessage(3, " -- No scripts found in ", dir->c_str(), "\n");
	OutputDebugString(dOut.c_str());
	return false;
}

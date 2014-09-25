#pragma once
#include "stdafx.h"
#include "ljpeg/jconfig.vc.win.h"
#include "LuaFileSystem.h"
#include "DebugOutput.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class CApplication
{
public:
	CApplication();
	virtual ~CApplication();
	// Start the application.
	virtual bool Run();
	virtual bool Run(const string*);
	virtual bool Run(int, const char**);

private:
	lua_State* m_luaState;
	CDebugOutput* m_debugOut;

	bool startLua(lua_State*);
	void getScriptFileName(char*, string& , const char*);
	bool getFileList(string*, vector<string*>*);
	bool runScript(lua_State*, string);
	bool runScript(lua_State*, int, const char**);
	bool loadScripts(lua_State*);
	void registerLuaClasses();
};


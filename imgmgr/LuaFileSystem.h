#pragma once

#include "Lua/lua.hpp"
#include "luna.h"
#include <string>
#include <vector>

using namespace std;

class CLuaFileSystem
{
private:
	lua_State* m_luaState;

	// internal
	bool getDirectoryList(string*, vector<string*>*);
	bool getFilesInDir(string*, vector<string*>*);
	bool split(string*, vector<string*>*, const char*);

public:
	CLuaFileSystem(lua_State* L);
	virtual ~CLuaFileSystem();

	static const char *className;
	static const Luna < CLuaFileSystem >::FunctionType methods[];
	static const Luna < CLuaFileSystem >::PropertyType properties[];

	bool isExisting; // This is used by Luna to see whether it's been created by createFromExisting.  Don't set it.
	bool isPrecious; // This is used to tell Luna not to garbage collect the object, in case other objects might reference it.  Set it in your class's constructor.

	// Lua interface
	int getInput(lua_State* L);
	int getFileList(lua_State* L);
	int getDirList(lua_State* L);
};


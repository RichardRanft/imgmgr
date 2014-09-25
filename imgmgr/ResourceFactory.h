#pragma once

#include "ResourceInstance.h"
#include "PropertyTemplate.h"
#include "Lua/lua.hpp"
#include <string>
#include <vector>

using namespace std;

class CResourceFactory
{
private:
	lua_State* m_luaState;
public:
	CResourceFactory();
	CResourceFactory(lua_State* L) {
		m_luaState = L;
	};
	~CResourceFactory();
	CResourceInstance* NewResource(const string&, const string&);
};

//lua_State* CResourceFactory::m_luaState = nullptr;

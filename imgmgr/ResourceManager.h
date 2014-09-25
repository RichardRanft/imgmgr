#pragma once

#include "Lua/lua.hpp"
#include "luna.h"
#include "ResourceInstance.h"
#include "ResourceFactory.h"
#include <string>
#include <unordered_map>

using namespace std;

struct hashing_func {
	unsigned long operator()(const string& key) const {
		unsigned long hash = 0;
		for (size_t i = 0; i<key.size(); i++)
			hash += (71 * hash + key[i]) % 5;
		return hash;
	}
};

struct key_equal_fn {
	bool operator()(const string& t1, const string& t2) const {
		return !(t1.compare(t2));
	}
};

class CResourceManager
{
private:
	lua_State* m_luaState;
	CResourceFactory* m_resFactory;

	unordered_map<string, CResourceInstance*, hashing_func, key_equal_fn> *m_AssetList;

	// internal interface
	void getImageResource(string* fileName)
	{
		// load an image from disk and create a CResourceInstance to pass back to Lua
	}

	string* getFileExtension(const string& filename);

public:
	CResourceManager(lua_State* L);
	virtual ~CResourceManager();

	static const char *className;
	static const Luna < CResourceManager >::FunctionType methods[];
	static const Luna < CResourceManager >::PropertyType properties[];

	bool isExisting; // This is used by Luna to see whether it's been created by createFromExisting.  Don't set it.
	bool isPrecious; // This is used to tell Luna not to garbage collect the object, in case other objects might reference it.  Set it in your classes constructor.

	// Lua interface
	int loadImage(lua_State* L);
	int getImageInfo(lua_State* L);
	int dumpResourceList(lua_State* L);
};


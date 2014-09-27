#include "ResourceManager.h"


CResourceManager::CResourceManager(lua_State* L)
{
	m_luaState = L;
	m_AssetList = new unordered_map<string, CResourceInstance*, hashing_func, key_equal_fn>();
	m_resFactory = new CResourceFactory(m_luaState);
}


CResourceManager::~CResourceManager()
{
	if (m_AssetList != nullptr)
	{
		m_AssetList->clear();
	}
}

// internal interface
string* CResourceManager::getFileExtension(const string& filename)
{
	// search for .
	int index = filename.find_last_of('.') + 1;
	string* ext = nullptr;
	if (index != string::npos)
	{
		ext = new string(filename.substr(index, filename.size() - index));
	}
	return ext;
}

// Lua interface
int CResourceManager::loadImage(lua_State* L)
{
	//cout << " ** in CResourceManager::loadImage" << endl;
	int top = lua_gettop(L); // should be image name
	string fileName = "";
	if (lua_isstring(L, top))
	{
		fileName = lua_tostring(L, top);

		CResourceInstance *image = (CResourceInstance *)m_resFactory->NewResource(*(getFileExtension(fileName)), fileName);
		if (image != nullptr)
		{
			// push pointer to the retrieved CResourceInstance into our managed list
			image->loadAsset((string&)fileName);

			//cout << " ** adding " << fileName << " to asset list" << endl;
			string name = fileName.c_str();
			m_AssetList->insert(unordered_map<string, CResourceInstance*>::value_type(name, image));
			// and to Lua
			lua_pushlightuserdata(L, image);
		}
		else
		{
			// return nil to Lua if the creation failed
			lua_pushnil(L);
		}
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

int CResourceManager::getImageInfo(lua_State* L)
{
	//cout << " ** in CResourceManager::getImageInfo" << endl;
	int top = lua_gettop(L); // should be image name
	string fileName = "";
	if (lua_isstring(L, top))
	{
		fileName = lua_tostring(L, top);
		//cout << " ** finding " << fileName << endl;
		bool found = false;
		SImageProperties temp;
		if (m_AssetList != nullptr)
		{
			unordered_map<string, CResourceInstance*>::iterator it = m_AssetList->find(fileName);
			if (it != m_AssetList->end())
			{
				//cout << " ** found " << fileName << endl;
				found = true;
				temp = it->second->ImageProperties();
			}
		}
		if (found)
		{
			//cout << " ** pushing " << fileName << " information back to Lua" << endl;
			lua_pushinteger(L, temp.m_width);
			lua_pushinteger(L, temp.m_height);
			lua_pushinteger(L, temp.m_size);
			return 3;
		}
	}
	cout << " ** " << fileName << " not found" << endl;
	lua_pushnil(L);
	return 1;
}

int CResourceManager::dumpResourceList(lua_State* L)
{
	//cout << " ** CResourceManager::dumpResourceList()" << endl;
	//int top = lua_gettop(L);
	int count = 0;
	if (m_AssetList != nullptr)
	{
		string tempname;
		lua_newtable(L);
		for (unordered_map<string, CResourceInstance*>::iterator it = m_AssetList->begin(); it != m_AssetList->end(); it++)
		{
			tempname = (*it).first;
			//cout << " ** asset " << count << " : " << tempname << " dumped" << endl;
			lua_pushnumber(L, count++);
			lua_pushstring(L, (char*)tempname.c_str());
			lua_settable(L, -3);
		}
	}
	else
	{
		lua_pushnil(L);
	}
	//cout << " ** count: " << count << endl;
	return 1;
}

// implementation
const char *CResourceManager::className = "CResourceManager";
const Luna < CResourceManager >::FunctionType CResourceManager::methods[] = {
	{ "loadImage", &CResourceManager::loadImage },
	{ "getImageInfo", &CResourceManager::getImageInfo },
	{ "dumpResourceList", &CResourceManager::dumpResourceList },
	{ 0 }
};
const Luna < CResourceManager >::PropertyType CResourceManager::properties[] = {
	{ 0 }
};

#pragma once

#include "Lua\lua.hpp"
#include "luna.h"
#include "PropertyTemplate.h"
#include "ResourceInstance.h"
#include "lpng/png.h"
#include <string>
#include <vector>
#include <Windows.h>

using namespace std;

class CBMPFile : public CResourceInstance
{
private:
	lua_State* m_luaState;
	string m_fileName;

public:
	CBMPFile(lua_State*);
	virtual ~CBMPFile();

	static const char *className;
	static const Luna < CBMPFile >::FunctionType methods[];
	static const Luna < CBMPFile >::PropertyType properties[];

	bool isExisting; // This is used by Luna to see whether it's been created by createFromExisting.  Don't set it.
	bool isPrecious; // This is used to tell Luna not to garbage collect the object, in case other objects might reference it.  Set it in your classes constructor.

	// internal interface
	PROPERTY_GET_SET(CBMPFile, Filename, const string&);
	PROPERTY_GET(CBMPFile, ImageProperties, const SImageProperties&);

	virtual bool loadAsset(string& fileName);
	virtual SImageProperties getImageSize();

protected:
	//	virtual const string& get_Value2() const { return this->m_value2; }
	//	virtual void          set_Value2(const string& value) { this->m_value2 = value; }
	virtual const string& get_Filename() const { return this->m_fileName; }
	virtual void set_Filename(const string& value) { this->m_fileName = value;  refreshAsset(); }

	virtual const SImageProperties& get_ImageProperties() const { return this->m_properties; }

	virtual void refreshAsset();
};


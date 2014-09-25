#pragma once

#include "Lua/lua.hpp"
#include "luna.h"
#include "PropertyTemplate.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

struct SImageProperties
{
	int m_width;
	int m_height;
	int m_size;
};

class CResourceInstance
{
private:
	string m_fileName;
	ifstream* m_filePointer;

public:
	CResourceInstance();
	virtual ~CResourceInstance();

	virtual SImageProperties getImageSize();
	virtual bool loadAsset(string& fileName);

	PROPERTY_GET_SET(CResourceInstance, Filename, const string&);
	PROPERTY_GET(CResourceInstance, ImageProperties, const SImageProperties&)

protected:
	SImageProperties m_properties;

	virtual const string& get_Filename() const { return this->m_fileName; }
	virtual void set_Filename(const string& value) { this->m_fileName = value;  refreshAsset(); }

	virtual const SImageProperties& get_ImageProperties() const { return this->m_properties; }

	virtual void refreshAsset();
	virtual void getWorkingPath(char*);
	virtual void getWorkingPath(string&);
};


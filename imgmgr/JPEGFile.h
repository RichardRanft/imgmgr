#pragma once

#include "ljpeg/jconfig.h"
#include "Lua/lua.hpp"
#include "luna.h"
#include "PropertyTemplate.h"
#include "ResourceInstance.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <Windows.h>

using namespace std;

class CJPEGFile : public CResourceInstance
{
private:
	lua_State* m_luaState;
	string m_fileName;
	FILE * m_filePointer;

	bool openFileStream(string&);
	INT32 next_marker(void);
	INT32 first_marker(void);
	void skip_variable(void);
	void process_COM(INT32 raw);
	void process_SOFn(INT32 marker);
	INT32 scan_JPEG_header(INT32 verbose, INT32 raw);
	INT32 read_1_byte(void);
	UINT32 read_2_bytes(void);
	void ERREXIT(const char*);

public:
	CJPEGFile(lua_State*);
	virtual ~CJPEGFile();

	static const char *className;
	static const Luna < CJPEGFile >::FunctionType methods[];
	static const Luna < CJPEGFile >::PropertyType properties[];

	bool isExisting; // This is used by Luna to see whether it's been created by createFromExisting.  Don't set it.
	bool isPrecious; // This is used to tell Luna not to garbage collect the object, in case other objects might reference it.  Set it in your classes constructor.

	// internal interface
	PROPERTY_GET_SET(CJPEGFile, Filename, const string&);
	PROPERTY_GET(CJPEGFile, ImageProperties, const SImageProperties&);

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


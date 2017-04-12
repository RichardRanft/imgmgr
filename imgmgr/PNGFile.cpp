#include "PNGFile.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

// global stream - yuck, but there it is.  Hooray for C libraries....
ifstream* sg_pStream;

// ----------------------------------------------------------------------------
// PNGLib "callbacks"
// ----------------------------------------------------------------------------
static void pngReadDataFn(png_structp  png_ptr,
	png_bytep   data,
	png_size_t  rlength)
{
	if (sg_pStream == NULL)
	{
		cerr << "Input stream not initialized in pngReadDataFn()" << endl;
		return;
	}
	try
	{
		sg_pStream->read((char*)data, rlength);
	}
	catch (ifstream::failure e)
	{
		cerr << "Exception reading file in pngReadDataFn()" << endl;
	}
}

void pngFlushDataFn(png_structp png_ptr)
{
	//
}

png_voidp pngMallocFn(png_structp png_ptr, png_size_t size)
{
	//return malloc(size);
	return (png_voidp)malloc(size);
}

void pngFreeFn(png_structp png_ptr, png_voidp mem)
{
	free(mem);
}

//--------------------------------------
void pngFatalErrorFn(png_structp png_ptr,
	png_const_charp pMessage)
{
	printf("Error reading PNG file:\n %s", pMessage);
}
//--------------------------------------
void pngWarnFn(png_structp png_ptr, png_const_charp pMessage)
{
	printf("Warning reading PNG file:\n %s", pMessage);
}
// ----------------------------------------------------------------------------

CPNGFile::CPNGFile(lua_State* L)
{
	//cout << " ** CPNGFile constructor" << endl;
	m_luaState = L;
	isPrecious = true;
}

CPNGFile::~CPNGFile()
{
	m_luaState = nullptr;
	if (m_filePointer != nullptr)
		delete(m_filePointer);
	if (m_fileName.size() > 0)
		m_fileName.clear();
}

void CPNGFile::refreshAsset()
{
	// invalidate the current PNG file and initialize the new file.
}

SImageProperties CPNGFile::getImageSize()
{
	return m_properties;
}

bool CPNGFile::openFileStream(string& fileName)
{
	m_filePointer = new ifstream(fileName, ios::binary);
	if (m_filePointer != nullptr)
		return true;
	return false;
}

bool CPNGFile::loadAsset(string& fileName)
{
	//cout << " ** loading PNG image " << fileName << endl;
	if (!(openFileStream(fileName)) || m_filePointer->bad())
	{
		// error out.
		cout << " ** unable to open " << fileName << endl;
		return false;
	}
	else
	{
		static const UINT32 cs_headerBytesChecked = 8;
		sg_pStream = m_filePointer;

		char header[cs_headerBytesChecked];
		m_filePointer->read(header, cs_headerBytesChecked);

		bool isPng = png_check_sig((png_const_bytep)header, cs_headerBytesChecked) != 0;
		if (isPng == false)
		{
			cout << " ** " << fileName << " is not a PNG file!" << endl;
			m_filePointer->close();
			return false;
		}

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL,
			pngFatalErrorFn,
			pngWarnFn);

		if (png_ptr == NULL)
		{
			m_filePointer->close();
			return false;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (info_ptr == NULL) {
			png_destroy_read_struct(&png_ptr,
				(png_infopp)NULL,
				(png_infopp)NULL);
			m_filePointer->close();
			return false;
		}

		png_infop end_info = png_create_info_struct(png_ptr);
		if (end_info == NULL) {
			png_destroy_read_struct(&png_ptr,
				&info_ptr,
				(png_infopp)NULL);
			m_filePointer->close();
			return false;
		}

		png_set_read_fn(png_ptr, NULL, pngReadDataFn);

		// Read off the info on the image.
		png_set_sig_bytes(png_ptr, cs_headerBytesChecked);
		png_read_info(png_ptr, info_ptr);

		png_uint_32 width;
		png_uint_32 height;
		INT32 bit_depth;
		INT32 color_type;

		png_get_IHDR(png_ptr, info_ptr,
			&width, &height,             // obv.
			&bit_depth, &color_type,     // obv.
			NULL,                        // interlace
			NULL,                        // compression_type
			NULL);                       // filter_type

		m_fileName = fileName.c_str();

		m_properties.m_height = height;
		m_properties.m_width = width;
		m_properties.m_size = (int)getFileSize(m_fileName);
	}
	m_filePointer->close();
	return true;
}


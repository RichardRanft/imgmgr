#include "BMPFile.h"

//#pragma pack(2)
//
//typedef struct                       /**** BMP file header structure ****/
//{
//	unsigned short bfType;           /* Magic number for file */
//	unsigned int   bfSize;           /* Size of file */
//	unsigned short bfReserved1;      /* Reserved */
//	unsigned short bfReserved2;      /* ... */
//	unsigned int   bfOffBits;        /* Offset to bitmap data */
//} BITMAPFILEHEADER;
//
//#pragma pack
//
//#  define BF_TYPE 0x4D42             /* "MB" */
//
//typedef struct                       /**** BMP file info structure ****/
//{
//	unsigned int   biSize;           /* Size of info header */
//	int            biWidth;          /* Width of image */
//	int            biHeight;         /* Height of image */
//	unsigned short biPlanes;         /* Number of color planes */
//	unsigned short biBitCount;       /* Number of bits per pixel */
//	unsigned int   biCompression;    /* Type of compression to use */
//	unsigned int   biSizeImage;      /* Size of image data */
//	int            biXPelsPerMeter;  /* X pixels per meter */
//	int            biYPelsPerMeter;  /* Y pixels per meter */
//	unsigned int   biClrUsed;        /* Number of colors used */
//	unsigned int   biClrImportant;   /* Number of important colors */
//} BITMAPINFOHEADER;

CBMPFile::CBMPFile(lua_State* L)
{
	m_luaState = L;
}


CBMPFile::~CBMPFile()
{
}

void CBMPFile::refreshAsset()
{
	// invalidate the current PNG file and initialize the new file.
}

SImageProperties CBMPFile::getImageSize()
{
	return m_properties;
}

bool CBMPFile::loadAsset(string& fileName)
{
	FILE* file;

	file = fopen(fileName.c_str(), "rb");

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	if (file != NULL) { // file opened
		size_t x = fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file); //reading the FILEHEADER

		fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);

		fclose(file);
	}

	m_properties.m_width = infoHeader.biWidth;
	m_properties.m_height = infoHeader.biHeight;
	m_properties.m_size = 0;

	return true;
}

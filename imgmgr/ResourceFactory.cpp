#include "ResourceInstance.h"

#include "PNGFile.h"
#include "JPEGFile.h"
#include "BMPFile.h"

#include "ResourceFactory.h"
#include "PropertyTemplate.h"
#include <string>
#include <vector>


CResourceFactory::CResourceFactory()
{
}

CResourceFactory::~CResourceFactory()
{
	m_luaState = nullptr;
}

CResourceInstance* CResourceFactory::NewResource(const string& description, const string& filename)
{
	//cout << " ** CResourceFactory::NewResource(" << description << ", " << filename << ")" << endl;
	if (description.size() < 1)
		return nullptr;
	if (description == "png")
	{
		//cout << " ** creating PNG resource ..." << endl;
		return new CPNGFile(m_luaState);
	}
	if (description == "jpg" || description == "jpeg")
	{
		cout << " ** creating JPEG resource ..." << endl;
		return new CJPEGFile(m_luaState);
	}
	if (description == "bmp")
	{
		//cout << " ** creating BMP resource ..." << endl;
		return new CBMPFile(m_luaState);
	}
	return nullptr;
}

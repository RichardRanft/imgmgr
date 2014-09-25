// imgmgr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"
#include "zlib/zlib.h"
#include "zlib/zutil.h"
#include <string>

int _tmain(int argc, _TCHAR* argv[])
{
	CApplication *app = new CApplication();
	if (argc > 1)
	{
		string* filename = new string();
		filename->append(argv[1]);
		app->Run(filename);
	}
	else
		app->Run();
	return 0;
}


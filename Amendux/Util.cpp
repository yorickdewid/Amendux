#include "stdafx.h"
#include "Util.h"

#include <Shlobj.h>
#include <iostream>

using namespace Amendux;

PWCHAR Util::getUserDocumentDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get document foler" << std::endl;

	return localDocPath;
}

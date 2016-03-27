#include "stdafx.h"
#include "Util.h"

#include <Shlobj.h>
#include <iostream>

using namespace Amendux;

PWCHAR Util::getUserDocumentDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}

/*PWCHAR Util::getUserContactDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Contacts, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}

PWCHAR Util::getUserDesktopDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}

PWCHAR Util::getUserDownloadDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}

PWCHAR Util::getUserFavoriteDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_Favorites, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}

PWCHAR Util::getUserAppDataDirectory()
{
	WCHAR *localDocPath = new WCHAR[128];

	if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &localDocPath) != S_OK)
		std::cerr << "Cannot get folder" << std::endl;

	return localDocPath;
}*/

// music
// video
// pictures
#include "stdafx.h"
#include "Util.h"

#include <Shlobj.h>
#include <iostream>

using namespace Amendux;

PWCHAR Util::getDirectory(Util::Directory folderId)
{
	KNOWNFOLDERID kfid;
	WCHAR *localDocPath = new WCHAR[128];
	
	switch (folderId)
	{
		case Util::Directory::USER_DOCUMENTS:
			kfid = FOLDERID_Documents;
			break;
		case Util::Directory::USER_CONTACT:
			kfid = FOLDERID_Contacts;
			break;
		case Util::Directory::USER_DESKTOP:
			kfid = FOLDERID_Desktop;
			break;
		case Util::Directory::USER_DOWNLOAD:
			kfid = FOLDERID_Downloads;
			break;
		case Util::Directory::USER_FAVORITE:
			kfid = FOLDERID_Favorites;
			break;
		case Util::Directory::USER_APPDATA:
			kfid = FOLDERID_LocalAppData;
			break;
		case Util::Directory::USER_PICTURES:
			kfid = FOLDERID_Pictures;
			break;
		case Util::Directory::USER_MUSIC:
			kfid = FOLDERID_Music;
			break;
		case Util::Directory::USER_VIDEOS:
			kfid = FOLDERID_Videos;
			break;
		default:
			break;
	}

	if (SHGetKnownFolderPath(kfid, 0, NULL, &localDocPath) != S_OK)
	{
		std::cerr << "Cannot get folder" << std::endl;
	}

	return localDocPath;
}


std::wstring Util::generateUUID()
{
	GUID gidReference;
	wchar_t szGuidW[40];
	HRESULT hCreateGuid = CoCreateGuid(&gidReference);
	StringFromGUID2(gidReference, szGuidW, 40);
	return std::wstring(szGuidW);
}

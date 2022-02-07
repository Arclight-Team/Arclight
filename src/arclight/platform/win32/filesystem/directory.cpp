/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 directory.cpp
 */

#include "filesystem/directory.hpp"

//#include <KnownFolders.h>


enum class KnownFolder {
	Desktop,
	Documents,
	Downloads,
	Pictures,
	Videos
};


static Directory getKnownFolder(KnownFolder dir) {
/*
	GUID kfid;

	switch(dir) {

		case KnownFolder::Desktop:
			kfid = FOLDERID_Desktop;
			break;

		case KnownFolder::Documents:
			kfid = FOLDERID_Documents;
			break;

		case KnownFolder::Downloads:
			kfid = FOLDERID_Downloads;
			break;

		case KnownFolder::Pictures:
			kfid = FOLDERID_Pictures;
			break;
		
		case KnownFolder::Videos:
			kfid = FOLDERID_Videos;
			break;

		default:
			arc_force_assert("Illegal known folder type");

	}
*/
	//wchar_t* pathCharArray;
	//SHGetKnownFolderPath(kfid, 0, nullptr, pathCharArray);

return Directory();
}



Directory Directory::getDesktopFolder() {
	return getKnownFolder(KnownFolder::Desktop);
}



Directory Directory::getDocumentFolder() {
	return getKnownFolder(KnownFolder::Documents);
}



Directory Directory::getDownloadFolder() {
	return getKnownFolder(KnownFolder::Downloads);
}



Directory Directory::getPictureFolder() {
	return getKnownFolder(KnownFolder::Pictures);
}



Directory Directory::getVideoFolder() {
	return getKnownFolder(KnownFolder::Videos);
}
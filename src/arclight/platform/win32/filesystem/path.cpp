/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 path.cpp
 */

#include "filesystem/path.hpp"
#include "util/assert.hpp"
#include "util/log.hpp"
#include "locale/unicode.hpp"
#include "os/process.hpp"
#include "types.hpp"

#include <string>
#include <vector>
#include <filesystem>

#include <Windows.h>
#include <ShlObj.h>



Path Path::getApplicationDirectory() {

	try {

		return Process::getCurrentExecutablePath().parent();

	} catch (std::exception& e) {

		LogE("Path").print("Failed to query application directory path: %s", e.what());
		return {};

	}

}



Path Path::getFolderPath(Folder folder) {

	GUID kfid;

	switch (folder) {

		case Folder::Desktop:
			kfid = FOLDERID_Desktop;
			break;

		case Folder::Documents:
			kfid = FOLDERID_Documents;
			break;

		case Folder::Downloads:
			kfid = FOLDERID_Downloads;
			break;

		case Folder::Images:
			kfid = FOLDERID_Pictures;
			break;

		case Folder::Music:
			kfid = FOLDERID_Music;
			break;

		case Folder::Videos:
			kfid = FOLDERID_Videos;
			break;

		case Folder::User:
			kfid = FOLDERID_Profile;
			break;

		case Folder::Recent:
			kfid = FOLDERID_Recent;
			break;

		case Folder::Temp:

			try {
				return Path(std::filesystem::temp_directory_path());
			} catch (...) {
				return Path();
			}

		default:
			arc_force_assert("Illegal known folder type");
			break;

	}

	wchar_t* pathCharArray;

	if (SUCCEEDED(SHGetKnownFolderPath(kfid, KF_FLAG_DEFAULT , nullptr, &pathCharArray))) {

		std::string pathString = Unicode::convertString<Unicode::UTF16LE, Unicode::UTF8>(pathCharArray);
		CoTaskMemFree(pathCharArray);

		return Path(pathString);

	}

	return Path();

}
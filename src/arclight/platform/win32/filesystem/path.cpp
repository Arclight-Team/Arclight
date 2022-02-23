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
#include "types.hpp"

#include <string>
#include <vector>
#include <filesystem>

#include <Windows.h>
#include <ShlObj.h>



Path Path::getApplicationDirectory() {

	u32 length = 0x200;
	std::vector<wchar_t> filename;

	try {

		filename.resize(length);

		while(GetModuleFileNameW(nullptr, filename.data(), length) == length) {

			if(length < 0x8000) {

				length *= 2;
				filename.resize(length);

			} else {

				/*
					Ideally, this cannot happen because the windows path limit is specified to be 0x7FFF (excl. null terminator byte)
					If this changes in future windows versions, long path names could fail since it would require to allocate fairly large buffers
					This is why we stop here with an error.
				*/
				Log::error("Path", "Failed to query application directory path: Path too long");
				return Path();

			}

		}
		
		std::wstring str(filename.data());

		return Path(std::filesystem::path(str)).parent();

	} catch (std::exception& e) {

		Log::error("Path", "Failed to query application directory path: %s", e.what());
		return Path();

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

		std::wstring wstr(pathCharArray);
		CoTaskMemFree(pathCharArray);

		return Path(std::string(wstr.begin(), wstr.end()));

	}

	return Path();

}
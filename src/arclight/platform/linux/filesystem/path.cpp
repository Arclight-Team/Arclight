/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 path.cpp
 */

#include "filesystem/path.hpp"
#include "util/log.hpp"
#include "types.hpp"

#include <string>
#include <vector>
#include <filesystem>

#include <unistd.h>



Path Path::getApplicationDirectory() {

	constexpr const char* symlinkName = "/proc/self/exe";
	SizeT length = 0x200;

	std::vector<char> filename(length);

	try {

		while(true) {

			ssize_t readLength = readlink(symlinkName, filename.data(), filename.size());

			if(readLength == length) {

				//If length exceeds 0x10000 bytes, cancel
				if(length >= 0x10000) {

					Log::error("Path", "Failed to query application directory path: Path name exceeds 0x10000 bytes");
					return Path();

				}

				//Double buffer and retry
				length *= 2;
				filename.resize(length);

			} else if (readLength == -1) {

				//Error occured while reading the symlink
				Log::error("Path", "Failed to query application directory path: Cannot read symbolic link");
				return Path();

			} else {

				//Read was successful, return filename
				std::string str(filename.data(), readLength);

				return Path(std::filesystem::path(str)).parent();

			}
			
		}

	} catch (std::exception& e) {

		Log::error("Path", "Failed to query application directory path: %s", e.what());
		return Path();

	}

}
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

#include "windows.h"



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
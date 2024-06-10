/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Symlink.cpp
 */

#include "Filesystem/Symlink.hpp"

#include <filesystem>



Symlink::Symlink() : Symlink(Path()) {}
Symlink::Symlink(const Path& path) : Symlink(FSEntry(path)) {}
Symlink::Symlink(FSEntry entry) : entry(std::move(entry)) {}



bool Symlink::exists() const {
	return entry.exists();
}



bool Symlink::create(const Path& target, bool asFile) {

	try {

		if (asFile) {
			std::filesystem::create_symlink(target.getHandle(), getPath().getHandle());
		} else {
			std::filesystem::create_directory_symlink(target.getHandle(), getPath().getHandle());
		}

	} catch (const std::exception&) {
		return false;
	}

	return true;

}



bool Symlink::copy(const Path& where, FSCopyExisting copyExisting) const {

	std::filesystem::copy_options options = std::filesystem::copy_options::copy_symlinks;

	switch (copyExisting) {

		case FSCopyExisting::Skip: options |= std::filesystem::copy_options::skip_existing; break;
		case FSCopyExisting::Update: options |= std::filesystem::copy_options::update_existing; break;
		case FSCopyExisting::Overwrite: options |= std::filesystem::copy_options::overwrite_existing; break;
		default: break;

	}

	if (!entry.isSymlink()) {
		return false;
	}

	try {
		std::filesystem::copy(getPath().getHandle(), where.getHandle(), options);
	} catch (const std::exception&) {
		return false;
	}

	return true;

}



bool Symlink::rename(const Path& to) {
	return entry.rename(to);
}



bool Symlink::remove() {
	return entry.remove();
}



Path Symlink::getPath() const {
	return entry.getPath();
}



FSEntry Symlink::getFSEntry() const {
	return entry;
}
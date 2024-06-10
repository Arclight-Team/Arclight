/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Symlink.hpp
 */

#pragma once

#include "FSEntry.hpp"



enum class FSCopySymlink {
	Follow,
	Copy,
	Skip
};



class Symlink {

public:

	Symlink();
	Symlink(const Path& path);
	Symlink(FSEntry entry);

	bool exists() const;

	bool create(const Path& target, bool asFile = true);
	bool copy(const Path& where, FSCopyExisting copyExisting = FSCopyExisting::Skip) const;
	bool rename(const Path& to);
	bool remove();

	Path getPath() const;
	FSEntry getFSEntry() const;

private:

	FSEntry entry;

};

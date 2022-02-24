/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 directory.hpp
 */

#pragma once

#include "symlink.hpp"
#include "fsentry.hpp"

#include <vector>
#include <variant>



class Directory;

class DirectoryIterator {

public:

	enum class Flag {
		None = 0x0,
		Recursive = 0x1,
		FollowDirSymlinks = 0x2,
		SkipPermissionDenied = 0x4
	};

	using enum Flag;

	DirectoryIterator();
	DirectoryIterator(const Directory& dir, Flag flags = None, FSEntry::Type typeMask = FSEntry::All);

	DirectoryIterator& operator++();
	DirectoryIterator operator++(int);

	FSEntry operator*() const;
	FSEntry operator->() const;

	DirectoryIterator& advance();

	bool operator==(const DirectoryIterator& other) const;

private:

	void reset();

	bool hasFlag(Flag flag) const;

	std::variant<std::filesystem::directory_iterator, std::filesystem::recursive_directory_iterator> it;
	Flag flags;
	FSEntry::Type types;
	bool end;

};

ARC_CREATE_BITMASK_ENUM(DirectoryIterator::Flag)



class Directory {

public:

	struct Counts {
		umax files;
		umax subdirs;
		umax symlinks;
		umax others;
	};

	enum class Sorting {
		NameAscending,
		NameDescending,
		TypeAscending,
		TypeDescending,
		DateAscending,
		DateDescending,
		SizeAscending,
		SizeDescending
	};

	Directory();
	Directory(const Path& path);
	Directory(FSEntry entry);

	DirectoryIterator begin() const;
	DirectoryIterator end() const;

	DirectoryIterator operator()(DirectoryIterator::Flag flags = DirectoryIterator::None, FSEntry::Type typeMask = FSEntry::All) const;
	DirectoryIterator iterate(DirectoryIterator::Flag flags = DirectoryIterator::None, FSEntry::Type typeMask = FSEntry::All) const;

	Counts countEntries(bool recursive = false) const;
	
	umax getCount(FSEntry::Type type, bool recursive = false) const;
	umax getFileCount(bool recursive = false) const;
	umax getDirectoryCount(bool recursive = false) const;
	umax getTotalCount(bool recursive = false) const;

	std::vector<FSEntry> listEntries(bool recursive = false) const;
	std::vector<FSEntry> listEntries(Sorting sorting, bool recursive = false) const;

	template<class Filter>
	std::vector<FSEntry> filterEntries(Filter&& filter, bool recursive = false) {

		DirectoryIterator::Flag flags = DirectoryIterator::SkipPermissionDenied;

		if(recursive) {
			flags |= DirectoryIterator::Recursive;
		}

		std::vector<FSEntry> entries;

		for(const FSEntry& entry : iterate(flags)) {

			if(filter(entry)) {
				entries.push_back(entry);
			}

		}

		return entries;

	}

	bool empty() const;

	bool exists();

	bool create();
	bool createSingle();
	bool copy(const Path& where, bool recursive = true, bool onlyDirs = false, FSCopyExisting copyExisting = FSCopyExisting::Skip, FSCopySymlink copySymlink = FSCopySymlink::Copy) const;
	bool rename(const Path& to);
	bool remove();

	Path getPath() const;
	FSEntry getFSEntry() const;

private:

	FSEntry entry;

};



DirectoryIterator begin(const DirectoryIterator& dir);
DirectoryIterator end(const DirectoryIterator&);
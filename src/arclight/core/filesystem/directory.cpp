/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 directory.cpp
 */

#include "directory.hpp"
#include "util/bool.hpp"
#include "util/assert.hpp"
#include "util/typetraits.hpp"

#include <algorithm>



DirectoryIterator::DirectoryIterator() : flags(DirectoryIterator::None), types(FSEntry::All), end(true) {}

DirectoryIterator::DirectoryIterator(const Directory& dir, DirectoryIterator::Flag flags, FSEntry::Type typeMask) : flags(flags), types(typeMask), end(false) {

	std::filesystem::directory_options opt = std::filesystem::directory_options::none;

	if(hasFlag(FollowDirSymlinks)) {
		opt |= std::filesystem::directory_options::follow_directory_symlink;
	}

	if(hasFlag(SkipPermissionDenied)) {
		opt |= std::filesystem::directory_options::skip_permission_denied;
	}

	if(hasFlag(Recursive)) {
		it = std::filesystem::recursive_directory_iterator(dir.getPath().getHandle(), opt);
	} else {
		it = std::filesystem::directory_iterator(dir.getPath().getHandle(), opt);
	}

	reset();

}



DirectoryIterator& DirectoryIterator::operator++() {
	return advance();
}



DirectoryIterator DirectoryIterator::operator++(int) {
	
	DirectoryIterator it = *this;
	++*this;

	return it;

}



FSEntry DirectoryIterator::operator*() const {

	if(hasFlag(Recursive)) {
		return FSEntry(*std::get<std::filesystem::recursive_directory_iterator>(it));
	} else {
		return FSEntry(*std::get<std::filesystem::directory_iterator>(it));
	}

}



FSEntry DirectoryIterator::operator->() const {
	return **this;
}



bool DirectoryIterator::hasFlag(Flag flag) const {
	return (flags & flag) == flag;
}



DirectoryIterator& DirectoryIterator::advance() {

	auto exec = [this](auto& it) {

		bool found = false;

		while(!found) {

			it++;

			if(it == TT::RemoveRef<decltype(it)>{}) {
				end = true;
				break;
			}

			const std::filesystem::directory_entry& entry = *it;
			FSEntry::Type type = FSEntry(entry).getType();

			if((type & types) == type) {
				found = true;
			}

		}

	};

	if(hasFlag(Recursive)) {
		exec(std::get<std::filesystem::recursive_directory_iterator>(it));
	} else {
		exec(std::get<std::filesystem::directory_iterator>(it));
	}

	return *this;

}



bool DirectoryIterator::operator==(const DirectoryIterator& other) const {
	
	if(end || other.end) {
		return end == other.end;
	}

	return it == other.it;

}



void DirectoryIterator::reset() {

	auto exec = [this](auto& it) {

		if(it == TT::RemoveRef<decltype(it)>{}) {
			end = true;
			return;
		}

		const std::filesystem::directory_entry& entry = *it;
		FSEntry::Type type = FSEntry(entry).getType();

		if((type & types) != type) {
			advance();
		}

	};

	if(hasFlag(Recursive)) {
		exec(std::get<std::filesystem::recursive_directory_iterator>(it));
	} else {
		exec(std::get<std::filesystem::directory_iterator>(it));
	}

}




constexpr static u32 getOrderRank(FSEntry::Type type) {

	switch(type) {

		case FSEntry::Directory:
			return 0;
		case FSEntry::File:
			return 1;
		case FSEntry::Symlink:
			return 2;
		case FSEntry::BlockDevice:
			return 3;
		case FSEntry::CharacterDevice:
			return 4;
		case FSEntry::Pipe:
			return 5;
		case FSEntry::Socket:
			return 6;
		default:
			return 7;
		case FSEntry::Unknown:
			return 8;

	}

}



Directory::Directory() : Directory(Path::getApplicationDirectory()) {}
Directory::Directory(const Path& path) : Directory(FSEntry(path)) {}
Directory::Directory(FSEntry entry) : entry(std::move(entry)) {}



DirectoryIterator Directory::begin() const {
	return DirectoryIterator(*this, DirectoryIterator::SkipPermissionDenied);
}



DirectoryIterator Directory::end() const {
	return DirectoryIterator();
}



DirectoryIterator Directory::operator()(DirectoryIterator::Flag flags, FSEntry::Type typeMask) const {
	return iterate(flags, typeMask);
}



DirectoryIterator Directory::iterate(DirectoryIterator::Flag flags, FSEntry::Type typeMask) const {
	return DirectoryIterator(*this, flags, typeMask);
}



Directory::Counts Directory::countEntries(bool recursive) const {

	Directory::Counts counts {};
	DirectoryIterator::Flag flags = DirectoryIterator::SkipPermissionDenied;

	if(recursive) {
		flags |= DirectoryIterator::Recursive;
	}

	for(const FSEntry& entry : iterate(flags)) {

		switch(entry.getType()) {

			case FSEntry::File:
				counts.files++;
				break;

			case FSEntry::Directory:
				counts.subdirs++;
				break;

			case FSEntry::Symlink:
				counts.symlinks++;
				break;

			default:
				counts.others++;
				break;

		}

	}

	return counts;

}



umax Directory::getCount(FSEntry::Type type, bool recursive) const {

	DirectoryIterator::Flag flags = DirectoryIterator::SkipPermissionDenied;

	if(recursive) {
		flags |= DirectoryIterator::Recursive;
	}

	umax count = 0;

	for(const FSEntry& entry : iterate(flags, type)) {
		count++;
	}

	return count;

}



umax Directory::getFileCount(bool recursive) const {
	return getCount(FSEntry::File, recursive);
}



umax Directory::getDirectoryCount(bool recursive) const {
	return getCount(FSEntry::Directory, recursive);
}



umax Directory::getTotalCount(bool recursive) const {

	DirectoryIterator::Flag flags = DirectoryIterator::SkipPermissionDenied;

	if(recursive) {
		flags |= DirectoryIterator::Recursive;
	}

	umax count = 0;

	for(const FSEntry& entry : iterate(flags)) {
		count++;
	}

	return count;

}



std::vector<FSEntry> Directory::listEntries(bool recursive) const {

	DirectoryIterator::Flag flags = DirectoryIterator::SkipPermissionDenied;

	if(recursive) {
		flags |= DirectoryIterator::Recursive;
	}

	std::vector<FSEntry> entries;

	for(const FSEntry& entry : iterate(flags)) {
		entries.push_back(entry);
	}

	return entries;

}



std::vector<FSEntry> Directory::listEntries(Sorting sorting, bool recursive) const {

	auto lessPredicate = []<Sorting S>(const FSEntry& a, const FSEntry& b) -> bool {

		constexpr static bool isAsc = Bool::one(S, Sorting::NameAscending, Sorting::TypeAscending, Sorting::DateAscending, Sorting::SizeAscending);

		constexpr static auto compare = [](const auto& a, const auto& b) -> bool {
			return isAsc ? a < b : a > b;
		};
		
		u32 rankA = getOrderRank(a.getType());
		u32 rankB = getOrderRank(b.getType());

		if(rankA != rankB) {

			return compare(rankA, rankB);

		} else {
			
			if constexpr (Bool::one(S, Sorting::NameAscending, Sorting::NameDescending)) {

				return compare(a.getPath().toNativeString(), b.getPath().toNativeString());

			} else if constexpr (Bool::one(S, Sorting::TypeAscending, Sorting::TypeDescending)) {

				if(a.getType() == FSEntry::File) {

					std::string extA = a.getPath().getExtension();
					std::string extB = b.getPath().getExtension();

					if(extA != extB) {
						return compare(extA, extB);
					} else {
						return compare(a.getPath().getStem(), b.getPath().getStem());
					}

				} else {

					return compare(a.getPath().toNativeString(), b.getPath().toNativeString());

				}

			} else if constexpr (Bool::one(S, Sorting::DateAscending, Sorting::DateDescending)) {

				return compare(a.getLastModifiedTime(), b.getLastModifiedTime());

			} else if constexpr (Bool::one(S, Sorting::SizeAscending, Sorting::SizeDescending)) {

				if(a.getType() == FSEntry::File) {

					return false;

				} else {

					return compare(a.getPath().toNativeString(), b.getPath().toNativeString());

				}

			} else {

				arc_force_assert("Illegal sorting mode");

			}

		}

	};

	std::vector<FSEntry> entries = listEntries(recursive);

	switch(sorting) {

		default:
		case Sorting::NameAscending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::NameAscending>(a, b); });
			break;

		case Sorting::NameDescending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::NameAscending>(a, b); });
			break;

		case Sorting::TypeAscending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::TypeAscending>(a, b); });
			break;

		case Sorting::TypeDescending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::TypeDescending>(a, b); });
			break;

		case Sorting::DateAscending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::DateAscending>(a, b); });
			break;

		case Sorting::DateDescending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::DateDescending>(a, b); });
			break;

		case Sorting::SizeAscending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::SizeAscending>(a, b); });
			break;

		case Sorting::SizeDescending:
			std::sort(entries.begin(), entries.end(), [lessPredicate] (const FSEntry& a, const FSEntry& b) { return lessPredicate.template operator()<Sorting::SizeDescending>(a, b); });
			break;

	}

	return entries;

}



bool Directory::empty() const {

	for (const FSEntry& entry : DirectoryIterator(getPath())) {
		return false;
	}

	return true;

}



bool Directory::exists() {
	return entry.exists();
}



bool Directory::create() {

	if (exists()) {
		return true;
	}

	try {
		std::filesystem::create_directories(getPath().getHandle());
	} catch (const std::exception& e) {
		return false;
	}

	return true;

}



bool Directory::createSingle() {

	if (exists()) {
		return true;
	}

	try {
		std::filesystem::create_directory(getPath().getHandle());
	} catch (const std::exception&) {
		return false;
	}

	return true;

}



bool Directory::copy(const Path& where, bool recursive, bool onlyDirs, FSCopyExisting copyExisting, FSCopySymlink copySymlink) {

	std::filesystem::copy_options options = std::filesystem::copy_options::none;

	if (recursive) {
		options |= std::filesystem::copy_options::recursive;
	}

	if (onlyDirs) {
		options |= std::filesystem::copy_options::directories_only;
	}

	switch (copyExisting) {

		case FSCopyExisting::Skip: options |= std::filesystem::copy_options::skip_existing; break;
		case FSCopyExisting::Update: options |= std::filesystem::copy_options::update_existing; break;
		case FSCopyExisting::Overwrite: options |= std::filesystem::copy_options::overwrite_existing; break;
		default: break;

	}

	switch (copySymlink) {

		case FSCopySymlink::Copy: options |= std::filesystem::copy_options::copy_symlinks; break;
		case FSCopySymlink::Skip: options |= std::filesystem::copy_options::skip_symlinks; break;
		default: break;

	}

	if (!entry.isDirectory()) {
		return false;
	}

	try {
		std::filesystem::copy(getPath().getHandle(), where.getHandle(), options);
	} catch (const std::exception&) {
		return false;
	}

	return true;

}



bool Directory::rename(const Path& to) {
	return entry.rename(to);
}



bool Directory::remove() {
	return entry.remove();
}



Path Directory::getPath() const {
	return entry.getPath();
}



FSEntry Directory::getFSEntry() const {
	return entry;
}



DirectoryIterator begin(const DirectoryIterator& dir) {
	return dir;
}



DirectoryIterator end(const DirectoryIterator&) {
	return DirectoryIterator();
}
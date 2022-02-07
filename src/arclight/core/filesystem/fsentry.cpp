/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fsentry.cpp
 */

#include "fsentry.hpp"

#include <chrono>
#include <filesystem>



FSEntry::FSEntry(const Path& path) : entry(path.getHandle()) {}

FSEntry::FSEntry(const std::filesystem::directory_entry& entry) : entry(entry) {}




bool FSEntry::exists() const {
	return entry.exists();
}



FSEntry::Type FSEntry::getType() const {

	switch(entry.symlink_status().type()) {

		case std::filesystem::file_type::regular:
			return FSEntry::File;
		case std::filesystem::file_type::directory:
			return FSEntry::Directory;
		case std::filesystem::file_type::symlink:
			return FSEntry::Symlink;
		case std::filesystem::file_type::block:
			return FSEntry::BlockDevice;
		case std::filesystem::file_type::character:
			return FSEntry::CharacterDevice;
		case std::filesystem::file_type::fifo:
			return FSEntry::Pipe;
		case std::filesystem::file_type::socket:
			return FSEntry::Socket;

		default:
		case std::filesystem::file_type::unknown:
		case std::filesystem::file_type::not_found:
		case std::filesystem::file_type::none:
			return FSEntry::Unknown;

	}

	return FSEntry::Unknown;

}



bool FSEntry::isFile() const {
	return entry.is_regular_file();
}



bool FSEntry::isDirectory() const {
	return entry.is_directory();
}



bool FSEntry::isSymlink() const {
	return entry.is_symlink();
}



bool FSEntry::isBlockDevice() const {
	return entry.is_block_file();
}



bool FSEntry::isCharDevice() const {
	return entry.is_character_file();
}



bool FSEntry::isPipe() const {
	return entry.is_fifo();
}



bool FSEntry::isSocket() const {
	return entry.is_socket();
}


bool FSEntry::isUnknown() const {
	return getType() == Type::Unknown;
}



Path FSEntry::getPath() const {
	return Path(entry.path());
}



umax FSEntry::getHardLinkCount() const {
	return entry.hard_link_count();
}



u64 FSEntry::getLastModifiedTime() const {
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::clock_cast<std::chrono::system_clock>(entry.last_write_time()).time_since_epoch()).count();
}



void FSEntry::setLastModifiedTime(u64 nanos) {
	std::filesystem::last_write_time(entry.path(), std::chrono::clock_cast<std::chrono::file_clock>(std::chrono::sys_time{std::chrono::milliseconds{nanos}}));
}



//FSPermission matches POSIX so the cast is safe
FSPermission FSEntry::getPermissions() const {
	return static_cast<FSPermission>(entry.symlink_status().permissions());
}



bool FSEntry::hasPermissions(FSPermission perms) const {
	return hasPermissions(getPermissions(), perms);
}



void FSEntry::setPermissions(FSPermission perms) {
	std::filesystem::permissions(entry.path(), static_cast<std::filesystem::perms>(perms), std::filesystem::perm_options::replace | std::filesystem::perm_options::nofollow);
}



void FSEntry::addPermissions(FSPermission perms) {
	std::filesystem::permissions(entry.path(), static_cast<std::filesystem::perms>(perms), std::filesystem::perm_options::add | std::filesystem::perm_options::nofollow);
}



void FSEntry::removePermissions(FSPermission perms) {
	std::filesystem::permissions(entry.path(), static_cast<std::filesystem::perms>(perms), std::filesystem::perm_options::remove | std::filesystem::perm_options::nofollow);
}



std::string FSEntry::getPermissionString() const {
	return getPermissionString(getPermissions());
}



std::string FSEntry::getModeString() const {
	return getModeString(getType(), getPermissions());
}



std::string FSEntry::getPermissionString(FSPermission perms) {

	constexpr char readSym = 'r';
	constexpr char writeSym = 'w';
	constexpr char execSym = 'x';
	constexpr char xidxSym = 's';
	constexpr char xidSym = 'S';
	constexpr char sbitxSym = 't';
	constexpr char sbitSym = 'T';

	std::string s(9, '-');

	if(hasPermissions(perms, FSPermission::OwnerRead)) { s[0] = readSym; }
	if(hasPermissions(perms, FSPermission::OwnerWrite)) { s[1] = writeSym; }
	if(hasPermissions(perms, FSPermission::GroupRead)) { s[3] = readSym; }
	if(hasPermissions(perms, FSPermission::GroupWrite)) { s[4] = writeSym; }
	if(hasPermissions(perms, FSPermission::UserRead)) { s[6] = readSym; }
	if(hasPermissions(perms, FSPermission::UserWrite)) { s[7] = writeSym; }

	if(hasPermissions(perms, FSPermission::OwnerExecute)) { 
		s[2] = hasPermissions(perms, FSPermission::SetUID) ? xidxSym : execSym;
	} else if (hasPermissions(perms, FSPermission::SetUID)) {
		s[2] = xidSym;
	}

	if(hasPermissions(perms, FSPermission::GroupExecute)) { 
		s[5] = hasPermissions(perms, FSPermission::SetGID) ? xidxSym : execSym;
	} else if (hasPermissions(perms, FSPermission::SetGID)) {
		s[5] = xidSym;
	}

	if(hasPermissions(perms, FSPermission::UserExecute)) { 
		s[8] = hasPermissions(perms, FSPermission::SBit) ? sbitxSym : execSym;
	} else if (hasPermissions(perms, FSPermission::SBit)) {
		s[8] = sbitSym;
	}

	return s;

}



std::string FSEntry::getModeString(Type type, FSPermission perms) {

	char prefix = '-';

	switch(type) {

		case File:
			prefix = '-';
			break;
			
		case Directory:
			prefix = 'd';
			break;

		case Symlink:
			prefix = 'l';
			break;

		case BlockDevice:
			prefix = 'b';
			break;

		case CharacterDevice:
			prefix = 'c';
			break;

		case Pipe:
			prefix = 'p';
			break;

		case Socket:
			prefix = 's';
			break;

		default:
		case Unknown:
			prefix = '?';
			break;

	}

	return prefix + getPermissionString(perms);

}



bool FSEntry::hasPermissions(FSPermission perms, FSPermission compare) {
	return (perms & compare) == compare;
}
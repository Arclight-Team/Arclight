/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 VirtualMemory.cpp
 */

#include "Memory/VirtualMemory.hpp"
#include "Common/Assert.hpp"

#include <Windows.h>



constexpr static DWORD protectionToProtectionFlags(VirtualMemory::Protection protection) {

	switch (protection) {

		case VirtualMemory::Protection::Execute:            return PAGE_EXECUTE;
		case VirtualMemory::Protection::ReadOnly:           return PAGE_READONLY;
		case VirtualMemory::Protection::ReadWrite:          return PAGE_READWRITE;
		case VirtualMemory::Protection::ExecuteRead:        return PAGE_EXECUTE_READ;
		case VirtualMemory::Protection::ExecuteReadWrite:   return PAGE_EXECUTE_READWRITE;

	}

	arc_force_assert("Bad protection setting");
	return PAGE_READONLY;

}


void* VirtualMemory::allocate(SizeT size, Protection protection) {
	return VirtualAlloc(nullptr, size, MEM_COMMIT, protectionToProtectionFlags(protection));
}



bool VirtualMemory::deallocate(void* ptr) {
	return VirtualFree(ptr, 0, MEM_RELEASE);
}



bool VirtualMemory::protect(void* start, SizeT size, Protection protection) {

	DWORD oldProtection;
	return VirtualProtect(start, size, protectionToProtectionFlags(protection), &oldProtection);

}
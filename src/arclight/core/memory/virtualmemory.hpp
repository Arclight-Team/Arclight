/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 virtualmemory.hpp
 */

#pragma once

#include "common/types.hpp"



namespace VirtualMemory {

	enum class Protection {
		Execute,
		ReadOnly,
		ReadWrite,
		ExecuteRead,
		ExecuteReadWrite
	};

	/*
	 *  Allocates virtual memory pages with the given protection
	 *
	 */
	void* allocate(SizeT size, Protection protection);

	/*
	 *  Deallocates virtual memory allocated with allocate(). Returns true if the deallocation was successful, false otherwise.
	 */
	bool deallocate(void* ptr);

	/*
	 *  Sets the page protection for all pages containing the addresses in the range [start; start + size]. Returns true if the page protection has been applied correctly.
	 */
	bool protect(void* start, SizeT size, Protection protection);

}
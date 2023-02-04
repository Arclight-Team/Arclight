/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 task.cpp
 */

#include "task.hpp"



bool Task::valid() const {
	return result.valid();
}



bool Task::finished() const {
	return wait(0);
}



void Task::wait() const {

	if (!result.valid()) {
		return;
	}

	try {
		result.wait();
	} catch (...) {}

}



bool Task::wait(u64 millis) const {

	if (result.valid()) {

		try {
			return result.wait_for(std::chrono::milliseconds(millis)) == std::future_status::ready;
		} catch (...) {}

	}

	return true;

}
/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 taskexecutable.cpp
 */

#include "taskexecutable.hpp"
#include "util/log.hpp"



TaskExecutable::TaskExecutable(const std::function<Any()>& function) : function(function), ready(false) {}


void TaskExecutable::execute() noexcept {

	if (ready && function) {

		try {
			result.set_value(function());
		} catch (...) {
			result.set_exception(std::current_exception());
		}

		// Prohibit task from reinvoking
		function = nullptr;

	}

}



Task TaskExecutable::getTask() noexcept {

	Task task;

	try {

		task.result = result.get_future();
		ready = true;

	} catch (...) {

		LogW("Task Executable") << "Task already queried";

	}

	return task;

}
/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 task.hpp
 */

#pragma once

#include "types.hpp"
#include "stdext/any.hpp"

#include <utility>
#include <functional>
#include <future>



class Task {

public:

	Task() = default;

	bool valid() const;
	bool finished() const;
	void wait() const;
	bool wait(u64 millis) const;

	template<class ThreadResultT>
	ThreadResultT getResult() {

		if (result.valid()) {
			return result.get().cast<ThreadResultT>();
		}

		throw BadAnyAccess();

	}

private:

	friend class TaskExecutable;

	std::future<Any> result;

};
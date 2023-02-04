/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 taskexecutable.hpp
 */

#pragma once

#include "task.hpp"



class TaskExecutable {

public:

	explicit TaskExecutable(const std::function<Any()>& function = {});

	void execute() noexcept;
	Task getTask() noexcept;

	template<class Func, class... Args> requires ((!CC::LValueRefType<Args> && ...) && CC::Invocable<Func, Args&&...>)
	static std::pair<Task, TaskExecutable> createTask(Func&& func, Args&&... args) {

		std::function<Any()> invocable = [func, ...args = std::move(args)]() -> Any {

			if constexpr (CC::Void<TT::InvokeResult<Func, Args...>>) {

				func(args...);
				return {};

			} else {

				return func(args...);

			}

		};

		TaskExecutable exec(invocable);
		Task task = exec.getTask();

		return {std::move(task), std::move(exec)};

	}

private:

	std::function<Any()> function;
	std::promise<Any> result;
	bool ready;

};
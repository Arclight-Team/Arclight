/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 threadpool.hpp
 */

#pragma once

#include "taskexecutable.hpp"
#include "concurrent/thread.hpp"
#include "stdext/concurrentqueue.hpp"
#include "util/assert.hpp"



class ThreadPool {

	constexpr static SizeT PriorityCount = 3;
	constexpr static SizeT MaxPriority = PriorityCount - 1;
	constexpr static u32 DynamicScale = 10;
	constexpr static u32 MaxTasks = 512;

public:

	ThreadPool();
	~ThreadPool();

	void create(SizeT threadCount = Thread::getHardwareThreadCount());
	void destroy();

	bool isActive() const;
	SizeT getThreadCount() const;
	SizeT getTotalTaskCount() const;
	SizeT getMaxTaskCount() const;

	void scale(SizeT threadCount);
	void assistDispatch();

	template<class Func, class... Args> requires CC::Invocable<Func, Args&&...>
	Task addTask(u32 priority, Func&& func, Args&&... args) {

		arc_assert(priority < PriorityCount, "Illegal thread priority %d", priority);

		auto [task, exec] = TaskExecutable::createTask(func, std::forward<Args>(args)...);

		while (!context->taskQueues[priority].push(std::move(exec)));

		context->tasks.fetch_add(1);
		context->tasks.notify_one();

		return std::move(task);

	}

private:

	using TaskQueue = ConcurrentQueue<TaskExecutable, MaxTasks>;

	struct ThreadPoolContext {

		TaskQueue taskQueues[PriorityCount];
		std::atomic_uint64_t tasks;
		std::vector<std::atomic_flag> active;

	};


	static void threadMain(ThreadPoolContext* context, SizeT threadID);


	std::vector<Thread> threads;
	std::unique_ptr<ThreadPoolContext> context;

};

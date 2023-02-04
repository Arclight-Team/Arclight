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

public:

	constexpr static SizeT PriorityCount = 3;
	constexpr static SizeT MaxPriority = PriorityCount - 1;
	constexpr static u32 DynamicScale = 10;
	constexpr static u32 MaxTasks = 512;


	ThreadPool() : maxThreads(0) {}
	~ThreadPool();

	void create(SizeT threadCount = Thread::getHardwareThreadCount());
	void destroy();

	bool isActive() const;
	SizeT getTotalTaskCount() const;

	void assistDispatch();

	template<class Func, class... Args> requires ((!CC::LValueRefType<Args> && ...) && CC::Invocable<Func, Args&&...>)
	Task run(u32 priority, Func&& func, Args&&... args) {

		arc_assert(priority < PriorityCount, "Illegal thread priority %d", priority);

		if (!isActive()) {

			LogW("Thread Pool") << "Thread pool not created";
			return {};

		}

		auto [task, exec] = TaskExecutable::createTask(func, std::forward<Args>(args)...);

		while (!context->taskQueues[priority].push(std::move(exec)));

		return std::move(task);

	}

private:

	using TaskQueue = ConcurrentQueue<TaskExecutable, MaxTasks>;

	class ThreadPoolContext {

	public:

		ThreadPoolContext() = default;

		TaskQueue taskQueues[PriorityCount];
		std::atomic_flag active;

	};

	static void threadMain(ThreadPoolContext* context);

	std::vector<Thread> threads;
	std::mutex scaleMutex;
	SizeT maxThreads;

	std::unique_ptr<ThreadPoolContext> context;

};

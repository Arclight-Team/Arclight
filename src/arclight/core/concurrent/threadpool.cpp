/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 threadpool.cpp
 */

#include "threadpool.hpp"



ThreadPool::ThreadPool() : context(std::make_unique<ThreadPoolContext>()) {}

ThreadPool::~ThreadPool() {
	destroy();
}



void ThreadPool::create(SizeT threadCount) {

	if (isActive()) {
		LogW("Thread Pool") << "Thread pool already started";
		return;
	}

	threads.resize(threadCount);
	context->active = std::vector<std::atomic_flag>(threadCount);

	for (SizeT i = 0; i < threadCount; i++) {

		context->active[i].test_and_set();
		threads[i].start(threadMain, context.get(), i);

	}

}



void ThreadPool::destroy() {

	if (!isActive()) {
		return;
	}

	// If we support chained tasks at some point, lock here
	while (context->tasks.load()) {
		assistDispatch();
	}

	for (std::atomic_flag& f : context->active) {
		f.clear();
	}

	context->tasks.fetch_add(1);
	context->tasks.notify_all();

	for (Thread& thread : threads) {
		thread.finish();
	}

	threads.clear();
	context->active.clear();

}



bool ThreadPool::isActive() const {
	return !threads.empty();
}



SizeT ThreadPool::getThreadCount() const {
	return threads.size();
}



SizeT ThreadPool::getTotalTaskCount() const {

	if (!isActive()) {
		return 0;
	}

	return context->tasks.load();

}



SizeT ThreadPool::getMaxTaskCount() const {
	return MaxTasks;
}



void ThreadPool::scale(SizeT threadCount) {

	if (!isActive()) {
		return;
	}

	SizeT currentThreadCount = getThreadCount();

	// Clear active flags
	for (SizeT i = 0; i < currentThreadCount; i++) {
		context->active[i].clear();
	}

	// Trick threads into active mode
	context->tasks.fetch_add(1);
	context->tasks.notify_all();

	// Wait for thread termination
	for (SizeT i = 0; i < currentThreadCount; i++) {
		threads[i].finish();
	}

	// Restore previous state
	context->tasks.fetch_sub(1);

	// Resize to new thread count
	threads.resize(threadCount);
	context->active = std::vector<std::atomic_flag>(threadCount);

	// Launch threads
	for (SizeT i = 0; i < threadCount; i++) {

		context->active[i].test_and_set();
		threads[i].start(threadMain, context.get(), i);

	}

}



void ThreadPool::assistDispatch() {

	if (!isActive()) {
		return;
	}

	TaskExecutable exec;

	while (context->tasks.load() > 0) {

		for (TaskQueue& queue : context->taskQueues) {

			if (queue.pop(exec)) {

				context->tasks.fetch_sub(1);
				exec.execute();

				break;

			}

		}

	}

}



void ThreadPool::threadMain(ThreadPoolContext* context, SizeT threadID) {

	while (context->active[threadID].test_and_set()) {

		for (TaskQueue& queue : context->taskQueues) {

			TaskExecutable exec;
			bool success = queue.pop(exec);

			if (success) {

				context->tasks.fetch_sub(1);
				exec.execute();

				break;

			}

		}

		context->tasks.wait(0);

	}

}
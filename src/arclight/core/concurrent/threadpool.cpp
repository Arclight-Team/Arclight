/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 threadpool.cpp
 */

#include "threadpool.hpp"



ThreadPool::~ThreadPool() {
	destroy();
}



void ThreadPool::create(SizeT threadCount) {

	if (isActive()) {
		LogW("Thread Pool") << "Thread pool already started";
		return;
	}

	context = std::make_unique<ThreadPoolContext>();

	maxThreads = threadCount;
	threads.resize(threadCount);

	context->active.test_and_set();

	for (Thread& thread : threads) {
		thread.start(threadMain, context.get());
	}

}



void ThreadPool::destroy() {

	if (!isActive()) {
		return;
	}

	context->active.clear();

	for (Thread& thread : threads) {
		thread.finish();
	}

	threads.clear();
	context.reset();

}



bool ThreadPool::isActive() const {
	return context != nullptr;
}



SizeT ThreadPool::getTotalTaskCount() const {

	if (!isActive()) {
		return 0;
	}

	SizeT tasks;

	for (const TaskQueue& queue : context->taskQueues) {
		tasks += queue.size();
	}

	return tasks;

}



void ThreadPool::assistDispatch() {

	if (!isActive()) {
		return;
	}

	TaskExecutable exec;
	bool done = true;

	do {

		done = true;

		for (TaskQueue& queue : context->taskQueues) {

			if (queue.pop(exec)) {

				exec.execute();
				done = false;
				break;

			}

		}

	} while (!done);

}



void ThreadPool::threadMain(ThreadPoolContext* context) {

	while (context->active.test()) {

		for (TaskQueue& queue : context->taskQueues) {

			TaskExecutable exec;
			bool success = queue.pop(exec);

			if (success) {

				exec.execute();
				break;

			}

		}

	}

}
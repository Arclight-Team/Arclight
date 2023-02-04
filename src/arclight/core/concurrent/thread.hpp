/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 thread.hpp
 */

#pragma once

#include "util/assert.hpp"
#include "types.hpp"

#include <thread>
#include <future>



class Thread {

public:

	Thread();
	~Thread();

	Thread(const Thread& thread) = delete;
	Thread& operator=(const Thread& thread) = delete;
	Thread(Thread&& thread) noexcept;
	Thread& operator=(Thread&& thread) noexcept;

	template<class Function, class... Args> requires ((!CC::LValueRefType<Args> && ...) && CC::Invocable<Function, Args&&...>)
	bool start(Function&& f, Args&&... args) {

		//Start only if it's not running already
		if (done()) {

			std::packaged_task<void()> task([f, ...args = std::move(args)](){
				f(args...);
			});

			future = task.get_future();
			thread = std::thread(std::move(task));

			return true;

		} else {

			return false;

		}

	}

	void finish();
	bool tryFinish(u64 timeoutMicros = 2000);	

	bool running() const;
	bool finished() const;
	bool done() const noexcept;
	std::thread::id getID() const noexcept;

	static u32 getHardwareThreadCount() noexcept;
	
private:

	std::thread thread;
	std::future<void> future;

};
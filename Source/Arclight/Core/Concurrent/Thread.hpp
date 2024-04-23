/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Thread.hpp
 */

#pragma once

#include "Common/Assert.hpp"
#include "Common/Types.hpp"
#include "StdExt/Any.hpp"

#include <thread>
#include <future>



class BadThreadResult : public ArclightException {

public:

	using ArclightException::ArclightException;
	BadThreadResult() noexcept : ArclightException("Bad Thread Result") {}

	virtual const char* name() const noexcept override { return "Bad Thread Result"; }

};



class Thread {

public:

	enum class DestructorPolicy {
		Join,
		Detach,
		Terminate
	};

	enum class State {
		Empty,
		Running,
		Finished
	};

	Thread();
	~Thread();

	template<class Function, class... Args> requires CC::Invocable<Function, Args&&...>
	explicit Thread(Function&& function, Args&&... args) : Thread() {
		start(std::forward<Function>(function), std::forward<Args>(args)...);
	}

	Thread(const Thread& thread) = delete;
	Thread& operator=(const Thread& thread) = delete;
	Thread(Thread&& thread) noexcept = default;
	Thread& operator=(Thread&& thread) noexcept = default;

	template<class Function, class... Args> requires CC::Invocable<Function, Args&&...>
	bool start(Function&& function, Args&&... args) {

		//Start only if it's not running already
		if (!running()) {

			std::packaged_task<Any()> task = std::packaged_task<Any()>([f = std::forward<Function>(function), ...a = std::forward<Args>(args)]() {

				if constexpr (CC::Returns<TT::Decay<Function>, void, TT::Decay<Args>...>) {

					std::invoke(TT::Decay<Function>(f), TT::Decay<Args>(a)...);
					return Any();

				} else {

					return Any(std::invoke(TT::Decay<Function>(f), TT::Decay<Args>(a)...));

				}

			});

			future = task.get_future();
			thread = std::thread(std::move(task));

			return true;

		}

		return false;

	}

	DestructorPolicy getDestructorPolicy() const noexcept;
	void setDestructorPolicy(DestructorPolicy policy) noexcept;

	State getThreadState() const noexcept;

	void finish();
	bool tryFinish(u64 us = 1000);

	// Will throw if the thread exited with an uncaught exception
	template<class R> requires !CC::Equal<R, void>
	R getResult() {

		if (finished()) {
			return future.get().cast<R>();
		}

		throw BadThreadResult();

	}

	template<class R> requires !CC::Equal<R, void>
	R getUncheckedResult() {

		if (finished()) {
			return future.get().unsafeCast<R>();
		}

		throw BadThreadResult();

	}

	bool running() const noexcept;
	bool finished() const noexcept;
	bool empty() const noexcept;

	bool detach() noexcept;

	std::thread::id getID() const noexcept;

	static SizeT getHardwareThreadCount() noexcept;
	static SizeT getFalseSharingSize() noexcept;

private:

	std::thread thread;
	std::future<Any> future;

	DestructorPolicy dtorPolicy;

};
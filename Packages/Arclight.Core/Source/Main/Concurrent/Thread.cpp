/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Thread.cpp
 */

#include "Concurrent/Thread.hpp"
#include "Util/Log.hpp"
#include "System/Hardware.hpp"

#include <chrono>
#include <utility>



Thread::Thread() : dtorPolicy(DestructorPolicy::Join) {}


Thread::~Thread() {

	switch (dtorPolicy) {

		case DestructorPolicy::Join:

			if (running()) {
				LogW("Thread") << "Thread has not been finished manually, force-finishing it";
				finish();
			}

		break;

		case DestructorPolicy::Detach:
			detach();
		break;

		case DestructorPolicy::Terminate:
			break;

	}

}



Thread::DestructorPolicy Thread::getDestructorPolicy() const noexcept {
	return dtorPolicy;
}



void Thread::setDestructorPolicy(DestructorPolicy policy) noexcept {
	dtorPolicy = policy;
}



Thread::State Thread::getThreadState() const noexcept {

	if (thread.joinable()) {
		return State::Running;
	}

	if (future.valid()) {
		return State::Finished;
	}

	return State::Empty;

}



void Thread::finish() {

	if (running()) {
		thread.join();
	}

}



bool Thread::tryFinish(u64 us) {

	if (running()) {

		auto result = future.wait_for(std::chrono::microseconds(us));

		if (result != std::future_status::ready) {
			return false;
		}

		thread.join();

	}

	return true;

}



bool Thread::running() const noexcept {
	return getThreadState() == State::Running;
}



bool Thread::finished() const noexcept {
	return getThreadState() == State::Finished;
}



bool Thread::empty() const noexcept {
	return getThreadState() == State::Empty;
}



bool Thread::detach() noexcept {

	try {
		thread.detach();
	} catch (...) {
		return false;
	}

	return true;

}



std::thread::id Thread::getID() const noexcept {
	return thread.get_id();
}




SizeT Thread::getHardwareThreadCount() noexcept {
	return System::Hardware::getHardwareConcurrency();
}



SizeT Thread::getFalseSharingSize() noexcept {
	return std::hardware_destructive_interference_size;
}
#include "thread.h"
#include "util/log.h"

#include <chrono>



Thread::Thread() {}


Thread::~Thread() {

	if (!done()) {
		Log::warn("Thread", "Thread has not been finished manually, force-finishing it");
		finish();
	}

}



void Thread::finish() {

	if (!done()) {
		thread.join();
	}

}



bool Thread::tryFinish(u64 usDelay) {

	if (!done()) {

		auto result = future.wait_for(std::chrono::microseconds(usDelay));

		if (result != std::future_status::ready) {
			return false;
		}

		thread.join();

	}

	return true;

}



bool Thread::running() const {

	if (future.valid()) {

		auto result = future.wait_for(std::chrono::nanoseconds(0));

		if (result != std::future_status::ready) {
			return true;
		}

	}

	return false;

}



bool Thread::finished() const {

	if (future.valid()) {

		auto result = future.wait_for(std::chrono::nanoseconds(0));

		if (result != std::future_status::ready) {
			return false;
		}

	}

	return true;

}



bool Thread::done() const {
	return !thread.joinable();
}



u64 Thread::getID() const {
	return std::hash<std::thread::id>()(thread.get_id());
}




u32 Thread::getHardwareThreadCount() {
	return std::thread::hardware_concurrency();
}
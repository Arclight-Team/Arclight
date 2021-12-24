#include "thread.hpp"
#include "util/log.hpp"

#include <chrono>
#include <utility>



Thread::Thread() {}


Thread::~Thread() {

	if (!done()) {
		Log::warn("Thread", "Thread has not been finished manually, force-finishing it");
		finish();
	}

}



Thread::Thread(Thread&& thread) noexcept :
	thread(std::exchange(thread.thread, {})),
	future(std::move(thread.future)) {}



Thread& Thread::operator=(Thread&& thread) noexcept {

	//Thread could still be running, contrary to move-construction
	if (!done()) {

		//To keep noexcept, we must ensure we terminate the task properly (to prevent fatal crashes when stack-unwinding)
		try {

			Log::error("Thread", "Thread cannot be move-target while running, force-finishing it");
			finish();

		} catch (std::exception& e) {

			//The thread is terminated anyways so we just perform the move.
			Log::error("Thread", "Fatal error: Exception caught while terminating thread on move assignment\n%s", e.what());
			arc_abort();

		}

	}

	this->thread = std::exchange(thread.thread, {});
	this->future = std::move(thread.future);

	return *this;

}



void Thread::finish() {

	if (!done()) {
		thread.join();
	}

}



bool Thread::tryFinish(u64 timeoutMicros) {

	if (!done()) {

		auto result = future.wait_for(std::chrono::microseconds(timeoutMicros));

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



bool Thread::done() const noexcept {
	return !thread.joinable();
}



std::thread::id Thread::getID() const noexcept {
	return thread.get_id();
}




u32 Thread::getHardwareThreadCount() noexcept {
	return std::thread::hardware_concurrency();
}
#pragma once

#include "util/assert.hpp"
#include "types.hpp"

#include <thread>
#include <future>



class Thread final {

public:

	Thread();
	~Thread();

	Thread(const Thread& thread) = delete;
	Thread& operator=(const Thread& thread) = delete;
	Thread(Thread&& thread) noexcept;
	Thread& operator=(Thread&& thread) noexcept;

	template<class Function, class... Args>
	bool start(Function&& f, Args&&... args) {

		//Start only if it's not running already
		if (done()) {

			std::packaged_task<void()> task([f, args...](){
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
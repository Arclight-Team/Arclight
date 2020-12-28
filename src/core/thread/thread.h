#pragma once

#include <thread>
#include <future>

#include "types.h"
#include "util/assert.h"



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
			
			task = std::move([&f, &args...]() {
				std::forward<Function>(f)(std::forward<Args>(args)...);
			});

			future = task.get_future();
			thread = std::move(std::thread(task));
			return true;

		} else {
			return false;
		}

	}

	void finish();
	bool tryFinish(u64 usDelay = 2000);	

	bool running() const;
	bool finished() const;
	bool done() const noexcept;
	u64 getID() const noexcept;

	static u32 getHardwareThreadCount() noexcept;
	
private:

	std::thread thread;
	std::packaged_task<void()> task;
	std::future<void> future;

};
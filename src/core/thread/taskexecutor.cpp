#include "taskexecutor.h"



TaskExecutor::TaskExecutor() {
	
}



TaskExecutor::~TaskExecutor() {

#ifdef ARC_TASK_END_WAIT
	assistDispatch();
#endif

	stop();

}



void TaskExecutor::start() {

	running.test_and_set(std::memory_order_release);

	for (Thread& t : threads) {
		t.start(&TaskExecutor::taskMain, this, false);
	}

}



void TaskExecutor::assistDispatch() noexcept {
	taskMain(true);
}



void TaskExecutor::forceClear() noexcept {

	TaskFunction function;

	for (i32 i = 4; i >= 0; i--) {

		while (!taskQueues[i].empty()) {
			taskQueues[i].pop(function);
		}

	}

}



void TaskExecutor::stop() {

	running.clear();

#ifdef ARC_TASK_SLEEP_ATOMIC
	queuedTaskCount.store(-1, std::memory_order_seq_cst);
	queuedTaskCount.notify_all();
#elif defined(ARC_TASK_SPIN)
	queuedTaskCount.store(-1, std::memory_order_seq_cst);
#endif

	for (Thread& t : threads) {
		t.finish();
	}

}



void TaskExecutor::setThreadCount(u32 threadCount) {
	threads.resize(threadCount);
}



void TaskExecutor::taskMain(bool assist) {

	TaskFunction function;
	std::any result;

	while (running.test(std::memory_order_acquire)) {

		for (i32 i = 4; i >= 0; i--) {

			if (taskQueues[i].pop(function)) {

#ifndef ARC_TASK_PERIODIC_SLEEP
				queuedTaskCount.fetch_sub(1, std::memory_order_seq_cst);
#endif
				try {
					function(result);
				} catch (std::exception& e) {
					Log::error("Task Executor", "An exception has been thrown in an async function: %s", e.what());
				}

				goto end_cycle;

			}

		}

		if(!assist) {

			//Wait until there is a new element

#ifdef ARC_TASK_SLEEP_ATOMIC
			queuedTaskCount.wait(0, std::memory_order_seq_cst);
#elif defined(ARC_TASK_SPIN)

			while(!queuedTaskCount.load(std::memory_order_acquire)) {
				arc_spin_yield();
			}
		
#elif defined(ARC_TASK_PERIODIC_SLEEP)
			std::this_thread::sleep_for(std::chrono::microseconds(ARC_TASK_SLEEP_DURATION));
#endif
		} else {
			//If we're in assist mode, return here
			return;
		}

	end_cycle:;

	};

}
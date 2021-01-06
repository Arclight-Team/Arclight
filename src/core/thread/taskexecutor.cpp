#include "taskexecutor.h"



TaskExecutor::TaskExecutor() {
	
}



TaskExecutor::~TaskExecutor() {
	
	running.clear();

#ifdef ARC_TASK_SLEEP_ATOMIC
	queuedTaskCount.store(-1, std::memory_order_seq_cst);
	queuedTaskCount.notify_all();
#endif

	for (Thread& t : threads) {
		t.finish();
	}

}



void TaskExecutor::start() {

	running.test_and_set(std::memory_order_release);

	for (Thread& t : threads) {
		t.start(&TaskExecutor::taskMain, this);
	}

}



void TaskExecutor::setThreadCount(u32 threadCount) {
	threads.resize(threadCount);
}



void TaskExecutor::taskMain() {

	TaskFunction function;
	std::any result;

	while (running.test(std::memory_order_acquire)) {

		bool success = false;

		for (i32 i = 4; i >= 0; i--) {

			success = taskQueue[i].pop(function);

			if (success) {

#ifdef ARC_TASK_SLEEP_ATOMIC
				queuedTaskCount.fetch_sub(1, std::memory_order_seq_cst);
#endif

				function(result);

				break;

			}

		}

		if (!success && running.test(std::memory_order_acquire)) {

#ifdef ARC_TASK_SLEEP_ATOMIC
			queuedTaskCount.wait(0, std::memory_order_seq_cst);
#elif defined(ARC_TASK_SPIN)

			while (true) {

				if (!emptyFlag.test(std::memory_order_acquire)) {
					break;
				}

				while(emptyFlag.test(std::memory_order_relaxed)) {
					arc_spin_yield();
				}

			}
		
#elif defined(ARC_TASK_PERIODIC_SLEEP)
			std::this_thread::sleep_for(std::chrono::microseconds(ARC_TASK_SLEEP_DURATION));
#endif
		}

	}

}
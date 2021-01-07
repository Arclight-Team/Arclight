#include "taskexecutor.h"



TaskExecutor::TaskExecutor() {
	
}



TaskExecutor::~TaskExecutor() {

#ifdef ARC_TASK_END_WAIT
	waitEmpty();
#endif

	stop();

}



void TaskExecutor::start() {

	running.test_and_set(std::memory_order_release);

	for (Thread& t : threads) {
		t.start(&TaskExecutor::taskMain, this);
	}

}



void TaskExecutor::waitEmpty() noexcept {

	TaskFunction function;

	for (i32 i = 4; i >= 0; i--) {
		Log::info("", "Queue size: %d", taskQueues[i].size());

		while (!taskQueues[i].empty()) {
			arc_spin_yield();
		}

	}

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



void TaskExecutor::taskMain() {

	TaskFunction function;
	std::any result;

	while (running.test(std::memory_order_acquire)) {

		for (i32 i = 4; i >= 0; i--) {

			if (taskQueues[i].pop(function)) {

#ifdef ARC_TASK_SLEEP_ATOMIC
				queuedTaskCount.fetch_sub(1, std::memory_order_seq_cst);
#endif
				Log::info("", "RUN");
				function(result);
				goto end_cycle;

			}

		}

#ifdef ARC_TASK_SLEEP_ATOMIC
		queuedTaskCount.wait(0, std::memory_order_seq_cst);
#elif defined(ARC_TASK_SPIN)

		while(!queuedTaskCount.load(std::memory_order_acquire)) {
			arc_spin_yield();
		}
		
#elif defined(ARC_TASK_PERIODIC_SLEEP)
		std::this_thread::sleep_for(std::chrono::microseconds(ARC_TASK_SLEEP_DURATION));
#endif

end_cycle:;

	}

}
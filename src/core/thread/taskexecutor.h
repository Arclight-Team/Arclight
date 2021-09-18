#pragma once

#include <vector>
#include <unordered_map>

#include "arcconfig.h"
#include "arcintrinsic.h"
#include "stdext/concurrentqueue.h"
#include "thread.h"
#include "task.h"
#include "types.h"


typedef u64 TaskID;
typedef std::function<void(std::any&)> TaskFunction;

enum class TaskPriority {
	Weak,
	Low,
	Normal,
	High,
	Critical
};


class TaskExecutor {

public:

	constexpr static inline u32 queueSize = 512;
	typedef ConcurrentQueue<TaskFunction, queueSize> TaskQueue;

	TaskExecutor();
	~TaskExecutor();

	void start();
	void stop();

	void assistDispatch() noexcept;
	void forceClear() noexcept;

	template<class Function, class... Args, typename Result = std::invoke_result_t<Function, Args...>>
	TaskID run(Function&& function, Args&&... args) {

		TaskFunction x = [&](std::any& result) {

			if constexpr (std::is_same_v<Result, void>) {
				std::invoke(std::forward<Function>(function), std::forward<Args>(args)...);
			} else {
				result = std::invoke(std::forward<Function>(function), std::forward<Args>(args)...);
			}

		};

		if (taskQueues[2].push(std::move(x))) {
#if defined(ARC_TASK_SLEEP_ATOMIC)
			queuedTaskCount.fetch_add(1, std::memory_order_seq_cst);
			queuedTaskCount.notify_one();
#elif defined(ARC_TASK_SPIN)
			queuedTaskCount.fetch_add(1, std::memory_order_seq_cst);
#endif
		}

		return 0;

	}

	void setThreadCount(u32 threadCount);

private:

	void taskMain(bool assist);

	std::vector<Thread> threads;
	TaskQueue taskQueues[5];
	std::unordered_map<TaskID, Task> tasks;
	std::atomic_flag running;

#if defined(ARC_TASK_SLEEP_ATOMIC) || defined(ARC_TASK_SPIN)
	std::atomic<u32> queuedTaskCount;
#endif

};
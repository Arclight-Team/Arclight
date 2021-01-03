#pragma once

#include <vector>


enum class TaskPriority {
	Weak,
	Low,
	Normal,
	High,
	Critical
};


class TaskExecutor {

public:

	

private:

	std::vector<Thread> threads;
	ConcurrentQueue<TaskFunction

};
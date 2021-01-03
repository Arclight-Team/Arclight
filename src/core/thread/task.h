#pragma once

#include <any>
#include "future.h"



class Task;

class TaskRunnable {

public:

	constexpr TaskRunnable(Task& task) : task(task) {}

	void execute();

private:

	Task& task;

};


enum class TaskState {
	Idle,
	Launchable,
	Running,
	Finished
};



class Task {

public:

	//Task();

	template<class Function, class... Args, typename Result = std::invoke_result_t<Function, Args...>>
	TaskFuture setFunction(Function&& function, Args&&... args) {
		
		taskFunction = [&](std::any& result) {
			result = std::invoke(std::forward(function), std::forward(args)...);
		};

		return TaskFuture(promise.get_future());

	}

	template<class Function, class... Args, typename Result = std::invoke_result_t<Function, Args...>>
	SharedTaskFuture setSharedFunction(Function&& function, Args&&... args) {

		taskFunction = [&](std::any& result) {
			result = std::invoke(std::forward(function), std::forward(args)...);
		};

		return SharedTaskFuture(promise.get_future());

	}

	TaskRunnable getTaskRunnable();
	void interrupt();

private:

	friend class TaskRunnable;

	std::function<void(std::any)> taskFunction;
	std::promise<std::any> promise;
	TaskState state;

};
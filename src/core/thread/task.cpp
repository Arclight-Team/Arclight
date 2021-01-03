#include "task.h"



void TaskRunnable::execute() {
/*
	if (interrupt.test(std::memory_order_acquire)) {
		return;
	}
*/
	std::any result;
	task.taskFunction(result);
	task.promise.set_value(result);

}



TaskRunnable Task::getTaskRunnable() {
	return TaskRunnable(*this);
}
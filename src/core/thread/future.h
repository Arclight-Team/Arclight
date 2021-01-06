#pragma once

#include <future>


class TaskFuture {

	explicit TaskFuture(std::future<std::any>&& future) : future(std::move(future)) {}



private:

	std::future<std::any> future;

};
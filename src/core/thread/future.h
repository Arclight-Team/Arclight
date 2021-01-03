#pragma once

#include <future>


class TaskFuture {

	explicit TaskFuture(std::future<std::any>&& future) : future(std::move(future)) {}



private:

	std::future<std::any> future;

};



class SharedTaskFuture {

	explicit SharedTaskFuture(std::shared_future<std::any>&& future) : future(std::move(future)) {}


private:

	std::shared_future<std::any> future;

};
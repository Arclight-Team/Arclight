/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 action.hpp
 */

#pragma once

#include <functional>

struct Action
{
	Action() { 
		reset(); 
	}
	Action(std::nullptr_t) : Action() {}
	template<class Func, class T, class ...Args>
	Action(Func func, T obj, Args&& ...args) {
		function = std::bind(func, obj, args...);
	}
	template<class Func, class ...Args>
	Action(Func func, Args&& ...args) {
		function = std::bind(func, args...);
	}

	void reset() {
		function = std::function<void()>();
	}

	void set(std::nullptr_t) {
		reset();
	}
	template<class Func, class T, class ...Args>
	void set(Func func, T obj, Args&& ...args) {
		function = std::bind(func, obj, args...);
	}
	template<class Func, class ...Args>
	void set(Func func, Args&& ...args) {
		function = std::bind(func, args...);
	}

	void invoke() {
		if (function) {
			function();
		}
	}

	void operator()() {
		invoke();
	}

	std::function<void()> function;

};
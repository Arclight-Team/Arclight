/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.hpp
 */

#pragma once

#define GUI_BEGIN	namespace GUI {
#define GUI_END		}

//User includes
#include "util/log.hpp"
#include "util/assert.hpp"
#include "math/vector.hpp"
#include "types.hpp"

#include "action.hpp"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "imgui.h"

#endif


#include <vector>
#include <string>

#define gui_assert(cond, msg, ...) arc_assert(cond, msg, __VA_ARGS__)
#define gui_force_assert(msg, ...) arc_force_assert(msg, __VA_ARGS__)

#define IMPL_ACTION_SETTER(x) \
	void set##x##Action(Action action) { m_##x = action; }

using HandleT = u32;

constexpr inline HandleT InvalidHandle = 0xFFFFFFFFu;

GUI_BEGIN

namespace {
	constexpr const char* logSystemName = "GL Engine";
}

template<typename... Args>
inline void debug(const std::string& msg, Args&&... args) {
	Log::debug(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void info(const std::string& msg, Args&&... args) {
	Log::info(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void warn(const std::string& msg, Args&&... args) {
	Log::warn(logSystemName, msg, std::forward<Args>(args)...);
}

template<typename... Args>
inline void error(const std::string& msg, Args&&... args) {
	Log::error(logSystemName, msg, std::forward<Args>(args)...);
}

GUI_END
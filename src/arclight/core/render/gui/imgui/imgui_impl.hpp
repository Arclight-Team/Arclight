/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imgui_impl.hpp
 */

#pragma once
#include "../common.hpp"

class Window;


GUI_BEGIN

void initialize(::Window& window);
void destroy();

void prepareRender();
void render();

GUI_END
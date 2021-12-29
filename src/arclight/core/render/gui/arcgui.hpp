/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcgui.hpp
 */

#pragma once

#include "common.hpp"

#include "control.hpp"
#include "container.hpp"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "imgui/imgui_impl.hpp"

#include "imgui/window.hpp"
#include "imgui/menubar.hpp"
#include "imgui/menu.hpp"
#include "imgui/menuitem.hpp"
#include "imgui/label.hpp"
#include "imgui/button.hpp"
#include "imgui/checkbox.hpp"
#include "imgui/slider.hpp"

// https://github.com/AirGuanZ/imgui-filebrowser
#include "imgui/imfilebrowser.hpp"

GUI_BEGIN

using ImGui::FileBrowser;

GUI_END

#endif
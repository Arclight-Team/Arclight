/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ArcGui.hpp
 */

#pragma once

#include "Common.hpp"

#include "Control.hpp"
#include "Container.hpp"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "ImGui/ImGuiImpl.hpp"

#include "ImGui/Window.hpp"
#include "ImGui/MenuBar.hpp"
#include "ImGui/Menu.hpp"
#include "ImGui/MenuItem.hpp"
#include "ImGui/Label.hpp"
#include "ImGui/Button.hpp"
#include "ImGui/CheckBox.hpp"
#include "ImGui/Slider.hpp"

// https://github.com/AirGuanZ/imgui-filebrowser
#include "ImGui/ImFileBrowser.hpp"

GUI_BEGIN

using ImGui::FileBrowser;

GUI_END

#endif
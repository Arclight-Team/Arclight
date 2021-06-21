#pragma once

#include "common.h"

#include "control.h"
#include "container.h"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "imgui/imgui_impl.h"

#include "imgui/window.h"
#include "imgui/menubar.h"
#include "imgui/menu.h"
#include "imgui/menuitem.h"
#include "imgui/label.h"
#include "imgui/button.h"
#include "imgui/checkbox.h"
#include "imgui/slider.h"

// https://github.com/AirGuanZ/imgui-filebrowser
#include "imgui/imfilebrowser.h"

GUI_BEGIN

using ImGui::FileBrowser;

GUI_END

#endif
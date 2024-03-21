/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MenuBar.cpp
 */

#include "MenuBar.hpp"

GUI_BEGIN

MenuBar::MenuBar() {}



bool MenuBar::begin() {

	IImGuiControl::applyRenderModifiers();

	return ImGui::BeginMenuBar();

}

void MenuBar::end() {
	ImGui::EndMenuBar();
}

GUI_END
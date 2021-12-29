/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 menubar.cpp
 */

#include "menubar.hpp"

GUI_BEGIN

MenuBar::MenuBar() {}



bool MenuBar::begin() {
	return ImGui::BeginMenuBar();
}

void MenuBar::end() {
	ImGui::EndMenuBar();
}

GUI_END
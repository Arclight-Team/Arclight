/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 menu.cpp
 */

#include "menu.hpp"

GUI_BEGIN

Menu::Menu() : m_Enabled(true) {}

Menu::Menu(const std::string& text, bool enabled ) {
	create(text, enabled);
}



void Menu::create(const std::string& text, bool enabled) {
	m_Text = text;
	m_Enabled = enabled;
}



bool Menu::isEnabled() const {
	return m_Enabled;
}

void Menu::setEnabled(bool enabled) {
	m_Enabled = enabled;
}


std::string Menu::getText() const {
	return m_Text;
}

void Menu::setText(const std::string& text) {
	m_Text = text;
}



bool Menu::begin() {

	IImGuiControl::applyRenderModifiers();

	bool open = ImGui::BeginMenu(m_Text.c_str(), m_Enabled);

	if (open != m_Open) {
		m_Open = open;
		if (m_Open)
			m_OnExpanded();
		else
			m_OnCollapsed();
	}

	return m_Open;

}

void Menu::end() {
	ImGui::EndMenu();
}


GUI_END
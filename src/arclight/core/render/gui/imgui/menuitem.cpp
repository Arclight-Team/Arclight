/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 menuitem.cpp
 */

#include "menuitem.hpp"

GUI_BEGIN

MenuItem::MenuItem() : m_Selected(false), m_Enabled(true) {}

MenuItem::MenuItem(const std::string& text, const std::string& shortcut, bool selected) {
	create(text, shortcut, selected);
}



void MenuItem::create(const std::string& text, const std::string& shortcut, bool selected) {
	m_Text = text;
	m_ShortcutText = shortcut;
	m_Selected = selected;
	m_Enabled = true;
}



bool MenuItem::isEnabled() const {
	return m_Enabled;
}

void MenuItem::setEnabled(bool enabled) {
	m_Enabled = enabled;
}



bool MenuItem::isSelected() const {
	return m_Selected;
}

void MenuItem::setSelected(bool enabled) {
	m_Selected = enabled;
}



std::string MenuItem::getText() const {
	return m_Text;
}

void MenuItem::setText(const std::string& text) {
	m_Text = text;
}



std::string MenuItem::getShortcutText() const {
	return m_ShortcutText;
}

void MenuItem::setShortcutText(const std::string& text) {
	m_ShortcutText = text;
}



void MenuItem::update() {

	IImGuiControl::applyRenderModifiers();

	if (ImGui::MenuItem(m_Text.c_str(), m_ShortcutText.c_str(), &m_Selected, m_Enabled)) {
		m_OnClicked();
	}

}

GUI_END
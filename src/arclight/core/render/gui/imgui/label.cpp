/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 label.cpp
 */

#include "label.hpp"

GUI_BEGIN

Label::Label() {}

Label::Label(const std::string& text) {
	create(text);
}



void Label::create(const std::string& text) {
	m_Text = text;
}



std::string Label::getText() const {
	return m_Text;
}

void Label::setText(const std::string& text) {
	m_Text = text;
}



void Label::update() {

	IImGuiControl::applyRenderModifiers();

	ImGui::TextUnformatted(m_Text.c_str(), m_Text.c_str() + m_Text.length());

}

GUI_END
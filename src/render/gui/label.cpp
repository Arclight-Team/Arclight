#include "label.h"

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
	ImGui::TextUnformatted(m_Text.c_str(), m_Text.c_str() + m_Text.length());
}

GUI_END
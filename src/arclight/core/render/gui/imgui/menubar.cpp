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
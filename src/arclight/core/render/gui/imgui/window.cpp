/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 window.cpp
 */

#include "window.hpp"

GUI_BEGIN

Window::Window() : m_Open(true), m_Flags(0) {
	m_CondAlwaysEnd = true;
}

Window::Window(const std::string& title, bool opened) {

	create(title, opened);

}



void Window::create(const std::string& title, bool opened) {

	m_Title = title;
	m_Open = opened;
	m_Collapsed = false;

}



bool Window::isOpen() const {
	return m_Open;
}

void Window::setOpened(bool opened) {
	if (opened)
		open();
	else
		close();
}

void Window::open() {
	m_Open = true;
	m_OnOpened();
}

void Window::close() {
	m_Open = false;
	m_OnClosed();
}



void Window::setCloseButton(bool visible) {
	m_CloseButtonVisible = visible;
}



std::string Window::getTitle() const {
	return m_Title;
}

void Window::setTitle(const std::string& title) {
	m_Title = title;
}



HandleT Window::addWindow(Window& window) {

	HandleT handle = IContainer::getNextHandle();

	m_Windows.emplace_back(handle, &window);

	return handle;

}



void Window::update() {

	IImGuiControl::applyRenderModifiers();

	IContainer::update();

	// Always update children windows
	for (auto& p : m_Windows) {
		if (p.second) {
			p.second->update();
		}
	}

}

bool Window::begin() {
	
	bool open = m_Open;
	if (open) {

		m_Begun = true;
		bool collapsed = !ImGui::Begin(m_Title.c_str(), &m_Open, m_Flags);

		if (collapsed != m_Collapsed) {
			m_Collapsed = collapsed;
			if (m_Collapsed)
				m_OnCollapsed();
			else
				m_OnExpanded();
		}

	}

	if (open != m_Open) {
		if (m_Open)
			m_OnOpened();
		else
			m_OnClosed();
	}

	// Update children only when the window is open and not collapsed
	return !m_Collapsed && m_Open;
}

void Window::end() {

	if (m_Begun) {
		m_Begun = false;
		ImGui::End();
	}

}

GUI_END
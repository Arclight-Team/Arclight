/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 window.hpp
 */

#pragma once

#include "imgui_impl.hpp"
#include "../common.hpp"
#include "../control.hpp"
#include "../container.hpp"

GUI_BEGIN

class MenuBar;

class Window : public IContainer, public IImGuiControl
{
public:

	Window();
	Window(const std::string& title, bool opened = true);

	void create(const std::string& title, bool opened = true);

	bool isOpen() const;
	void setOpened(bool opened);
	void open();
	void close();

	void setCloseButton(bool visible);

	std::string getTitle() const;
	void setTitle(const std::string& title);

	HandleT addWindow(Window& window);

	template<Control T>
	HandleT addControl(T& control) {

		if constexpr (std::is_same_v<T, MenuBar>) {
			m_Flags |= ImGuiWindowFlags_MenuBar;
		}

		if constexpr (std::is_same_v<T, Window>) {
			return addWindow(control);
		}

		return IContainer::addControl(&control);

	}
	IControl& getControl(HandleT handle) {
		return IContainer::getControl(handle);
	}

	virtual void update() override;

	IMPL_ACTION_SETTER(OnExpanded)
	IMPL_ACTION_SETTER(OnCollapsed)
	IMPL_ACTION_SETTER(OnOpened)
	IMPL_ACTION_SETTER(OnClosed)

protected:

	virtual bool begin() override;
	virtual void end() override;

private:

	std::string m_Title;
	bool m_Open;
	ImGuiWindowFlags m_Flags;
	bool m_CloseButtonVisible;

	std::vector<std::pair<HandleT, Window*>> m_Windows;
	bool m_Begun;
	bool m_Collapsed;
	Action m_OnExpanded;
	Action m_OnCollapsed;
	Action m_OnOpened;
	Action m_OnClosed;

};

GUI_END
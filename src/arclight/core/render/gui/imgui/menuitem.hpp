/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 menuitem.hpp
 */

#pragma once

#include "imgui_impl.hpp"
#include "../common.hpp"
#include "../control.hpp"
#include "../action.hpp"

GUI_BEGIN

class MenuItem : public IControl, public IImGuiControl
{
public:

	MenuItem();
	MenuItem(const std::string& text, const std::string& shortcut = "", bool selected = false);

	void create(const std::string& text, const std::string& shortcut = "", bool selected = false);

	bool isEnabled() const;
	void setEnabled(bool enabled);

	bool isSelected() const;
	void setSelected(bool selected);

	std::string getText() const;
	void setText(const std::string& text);
	
	std::string getShortcutText() const;
	void setShortcutText(const std::string& text);
	
	IMPL_ACTION_SETTER(OnClicked)

protected:

	virtual void update() override;

private:

	std::string m_Text;
	std::string m_ShortcutText;
	bool m_Selected;
	bool m_Enabled;

	Action m_OnClicked;

};

GUI_END
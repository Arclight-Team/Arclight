/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 button.hpp
 */

#pragma once

#include "imgui_impl.hpp"
#include "../common.hpp"
#include "../control.hpp"
#include "../action.hpp"

GUI_BEGIN

class Button : public IControl, public IImGuiControl
{
public:

	Button();
	Button(const std::string& text, const Vec2f& size = Vec2f());

	void create(const std::string& text, const Vec2f& size = Vec2f());

	std::string getText() const;
	void setText(const std::string& text);

	Vec2f getSize() const;
	void setSize(const Vec2f& size);

	IMPL_ACTION_SETTER(OnClicked)

protected:

	virtual void update() override;

private:

	std::string m_Text;
	Vec2f m_Size;

	Action m_OnClicked;

};

GUI_END
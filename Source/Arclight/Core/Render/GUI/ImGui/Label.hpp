/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Label.hpp
 */

#pragma once

#include "ImGuiImpl.hpp"
#include "../Common.hpp"
#include "../Control.hpp"

GUI_BEGIN

class Label : public IControl, public IImGuiControl
{
public:

	Label();
	Label(const std::string& text);

	void create(const std::string& text);

	std::string getText() const;
	void setText(const std::string& text);

protected:

	virtual void update() override;

private:

	std::string m_Text;

};

GUI_END
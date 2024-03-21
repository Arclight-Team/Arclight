/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MenuBar.hpp
 */

#pragma once

#include "ImGuiImpl.hpp"
#include "../Common.hpp"
#include "../Control.hpp"
#include "../Container.hpp"

GUI_BEGIN

class MenuBar : public IContainer, public IImGuiControl
{
public:

	MenuBar();

	template<Control T>
	HandleT addControl(T& control) {
		return IContainer::addControl(&control);
	}
	IControl& getControl(HandleT handle) {
		return IContainer::getControl(handle);
	}

	using IContainer::update;

protected:

	virtual void update() override {
		IContainer::update();
	}
	virtual bool begin() override;
	virtual void end() override;


private:

};

GUI_END
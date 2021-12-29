/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 container.hpp
 */

#pragma once

#include "common.hpp"
#include "control.hpp"

GUI_BEGIN

class IControl;

class IContainer : public IControl
{
public:

	bool controlExists(HandleT handle) const;
	bool swapControls(HandleT first, HandleT second);

protected:

	HandleT getNextHandle() const;
	i64 getControlIndex(HandleT handle) const;
	HandleT addControl(IControl* control);
	IControl& getControl(HandleT handle);

	virtual void update() override;

protected:

	std::vector<std::pair<HandleT,IControl*>> m_Controls;
	bool m_CondAlwaysEnd = false;
};

GUI_END
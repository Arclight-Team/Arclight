/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Control.hpp
 */

#pragma once

#include "Common.hpp"

GUI_BEGIN

class IControl
{
public:

	constexpr HandleT getHandle() const {
		return m_Handle;
	}

protected:

	virtual bool begin();
	virtual void update() = 0;
	virtual void end();

private:

	friend class IContainer;

	int m_Handle;

};

template<class T>
concept Control = std::is_base_of_v<IControl, T>;

GUI_END
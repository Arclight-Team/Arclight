/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cursor.hpp
 */

#pragma once

#include "math/vector.hpp"



class InputSystem;

class Cursor {

public:

	explicit Cursor(const InputSystem& system);

	void setPosition(const Vec2d& pos);
	void setPosition(double x, double y);
	Vec2d getPosition() const;

	void show();
	void hide();
	void free();

	void enableRawMotion();
	void disableRawMotion();

private:

	const InputSystem& system;

};
/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 dynamicsworld.hpp
 */

#pragma once

#include "math/vector.hpp"
#include <memory>



class RigidBody;

class DynamicsWorld {

public:

	constexpr DynamicsWorld() = default;

	void create();

	void setWorldGravity(const Vec3x& gravity);

	//RIGIDBODY TRANSITION
/*
	void addRigidBody(RigidBody& body);
	void deleteRigidBody(RigidBody& body);
*/
	void simulate(double dt, u32 tps);

private:

	std::shared_ptr<struct DynamicsWorldConfiguration> config;

};
/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderhandle.hpp
 */

#pragma once

#include "render/gle/shaderprogram.hpp"



class ShaderHandle {

public:

	explicit ShaderHandle(GLE::ShaderProgram&& program) : program(std::move(program)) {}

	~ShaderHandle() {
		program.destroy();
	}

	GLE::ShaderProgram program;

};
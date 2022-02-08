/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springshaders.hpp
 */

#pragma once



namespace SpringShader {


	constexpr const char* rectangularOutlineVS = R"(
#version 330

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec4 flatTransform;
layout (location = 2) in vec2 translation;

uniform mat4 projection;

void main() {

	mat2 transform;
	transform[0] = vec2(flatTransform.x, flatTransform.y);
	transform[1] = vec2(flatTransform.z, flatTransform.w);

	gl_Position = projection * vec4(transform * vertex + translation, 0.0, 1.0);

}
	)";

	constexpr const char* rectangularOutlineFS = R"(
#version 330

out vec4 outColor;

void main() {
	outColor = vec4(1.0);
}
	)";

	constexpr const char* rectangularOutlineUProjection = "projection";

}
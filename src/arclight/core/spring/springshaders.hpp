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

	mat3x2 transform = mat3x2(vec2(flatTransform.x, flatTransform.y), vec2(flatTransform.z, flatTransform.w), translation);
	vec2 transformedVertex = transform * vec3(vertex, 1.0);

	gl_Position = projection * vec4(transformedVertex, 0.0, 1.0);

}
	)";

	constexpr const char* rectangularOutlineFS = R"(
#version 330

out vec4 outColor;

void main() {
	outColor = vec4(gl_FragCoord.xy / 1000.0, 1.0, 1.0);
}
	)";

	constexpr const char* rectangularOutlineUProjection = "projection";

}
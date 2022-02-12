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
#version 430

layout (location = 0) in vec2 vertex;
layout (location = 1) in vec4 rsTransform;
layout (location = 2) in vec4 tsTransform;

out vec2 uv;

uniform mat4 projection;

void main() {

	vec2 translation = tsTransform.xy;
	vec2 scale = tsTransform.zw;
	vec2 transformedVertex = mat2(rsTransform.xy, rsTransform.zw) * (vertex * scale) + translation;

	uv = vertex * 0.96 + 0.5;
	gl_Position = projection * vec4(transformedVertex, 0.0, 1.0);

}
	)";

	constexpr const char* rectangularOutlineFS = R"(
#version 430

in vec2 uv;
out vec4 outColor;

uniform sampler2D sampleTexture;

void main() {
	outColor = vec4(texture(sampleTexture, uv).rgb, 1.0);
}
	)";

	constexpr const char* rectangularOutlineUProjection = "projection";

}
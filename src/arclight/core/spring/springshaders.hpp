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
layout (location = 1) in vec4 transform;
layout (location = 2) in vec2 translation;
layout (location = 3) in uint typeID;

struct Type {
	vec2 origin;
	vec2 uvBase;
	vec2 uvScale;
	uvec2 diffuseIndex;
};

layout (std430, binding = 0) buffer TypeBuffer {
	Type types[];
} typeBuffer;

out vec3 uv;

uniform mat4 projection;

void main() {

	Type type = typeBuffer.types[typeID];
	vec2 origin = type.origin;
	vec2 transformedVertex = mat2(transform.xy, transform.zw) * (vertex - origin) + translation;

	uv = vec3(vertex * type.uvScale + type.uvBase, type.diffuseIndex.x);
	gl_Position = projection * vec4(transformedVertex, 0.0, 1.0);

}
	)";

	constexpr const char* rectangularOutlineFS = R"(
#version 430

in vec3 uv;
out vec4 outColor;

uniform sampler2DArray diffuseTexture;

void main() {
	outColor = vec4(texture(diffuseTexture, uv).rgb, 1.0);
}
	)";

	constexpr const char* rectangularOutlineUProjection = "projection";

}
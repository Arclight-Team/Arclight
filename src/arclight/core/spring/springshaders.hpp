/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 springshaders.hpp
 */

#pragma once

#include "spring.hpp"



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
	uvec2 texIdx;
};

layout (std430, binding = 0) buffer TypeBuffer {
	Type types[];
} typeBuffer;

out vec2 uv;
out flat uvec2 texIdx;

uniform mat4 projection;

void main() {

	Type type = typeBuffer.types[typeID];

	vec2 origin = type.origin;
	vec2 transformedVertex = mat2(transform.xy, transform.zw) * (vertex - origin) + translation;

	uv = vec2(vertex * type.uvScale + type.uvBase);
	texIdx = type.texIdx;

	gl_Position = projection * vec4(transformedVertex, 0.0, 1.0);

}
	)";

	constexpr const char* rectangularOutlineFS = R"(
#version 430

in vec2 uv;
in flat uvec2 texIdx;

out vec4 outColor;

layout (std140, binding = 0) uniform CTTable {
	uint packedIdx[4096];
};

uniform sampler2DArray textures[16];


uint getTexTableIndex(uint ctID) {
	return bitfieldExtract(packedIdx[ctID / 8], int(bitfieldExtract(ctID, 0, 1)) * 4, 4);
}


void main() {
	outColor = vec4(texture(textures[getTexTableIndex(texIdx.x)], vec3(uv, texIdx.y)).rgb, 1.0);
}
	)";

	constexpr const char* rectangularOutlineUProjection = "projection";

}
#version 430

in vec2 uv;
in flat uvec2 texIdx;

out vec4 outColor;

layout (std140, binding = 1) uniform CTTable {
	uint packedIdx[4096];
};

uniform sampler2DArray textures[16];



uint getTexTableIndex(uint ctID) {
	return bitfieldExtract(packedIdx[ctID / 8], int(bitfieldExtract(ctID, 0, 1)) * 4, 4);
}


void main() {
	outColor = vec4(texture(textures[getTexTableIndex(texIdx.x)], vec3(uv, texIdx.y)).rgb, 1.0);
}
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

layout (std140, binding = 0) uniform SharedData {
    mat4 projection;
};

out vec2 uv;
out flat uvec2 texIdx;

void main() {

	Type type = typeBuffer.types[typeID];

	vec2 origin = type.origin;
	vec2 transformedVertex = mat2(transform.xy, transform.zw) * (vertex - origin) + translation;

	uv = vec2(vertex * type.uvScale + type.uvBase);
	texIdx = type.texIdx;

	gl_Position = projection * vec4(transformedVertex, 0.0, 1.0);

}
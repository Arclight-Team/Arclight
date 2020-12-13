#version 430
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
					
out vec3 pos;					
out vec2 uv;
out vec3 nrml;
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;

void main(){
	pos = vec3(modelMatrix * vec4(vertex, 1.0));
	uv = texcoord;
	//nrml = vec3(modelMatrix * vec4(normal, 1.0));
	nrml = normal;
	gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
#version 430
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec3 normal;
					
out vec3 pos;					
out vec2 uv;
out vec3 nrml;
uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 mvpMatrix;


void main(){
	pos = vec3(modelViewMatrix * vec4(vertex, 1.0));
	uv = texcoord;
	nrml = normalize(normalMatrix * normal);
	//nrml = normal;
	gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
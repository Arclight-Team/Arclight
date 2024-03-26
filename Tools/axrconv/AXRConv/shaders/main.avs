#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 uvCoord;

uniform mat4 mvpMatrix;


void main(){
	uvCoord = uv;
	gl_Position = mvpMatrix * vec4(position, 1.0);
}
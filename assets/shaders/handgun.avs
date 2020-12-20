#version 430
layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 texcoord;
								
out vec2 uv;
uniform mat4 mvpMatrix;

void main(){
	uv = texcoord;
	gl_Position = mvpMatrix * vec4(vertex, 0.0, 1.0);
}
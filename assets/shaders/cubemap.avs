#version 330 core
layout (location = 0) in vec3 vertex;

out vec3 uv;

uniform mat4 mvpMatrix;

void main(){
    uv = vertex;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 lightMVPMatrix;


void main(){
    gl_Position = lightMVPMatrix * vec4(vertex, 1.0);
}
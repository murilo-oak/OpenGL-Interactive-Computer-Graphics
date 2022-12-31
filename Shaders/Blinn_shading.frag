#version 450

in vec4 color;
in vec3 normal;
layout(location = 0) out vec4 ocolor;

void main() {
	
	ocolor = color;
}
#version 450

in vec4 color;
layout(location = 0) out vec4 ocolor;

void main() {
	ocolor = color;
}
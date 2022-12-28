#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;

out vec4 color;
uniform mat4 mvp;

void main() {
	color = vec4(icolor, 1.0);
	gl_Position = mvp * vec4(iposition, 1);
}
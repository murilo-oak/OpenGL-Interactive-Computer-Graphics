#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;
layout(location = 2) in vec3 inormal;

out vec4 color;
out vec3 normal;
out vec4 pos;

uniform mat4 mvp;
uniform mat3 mv;

void main() {
	color = vec4(icolor,1);
	pos = vec4(iposition, 1);
	normal = inormal;
	gl_Position =  mvp * vec4(iposition * 0.05, 1);
}
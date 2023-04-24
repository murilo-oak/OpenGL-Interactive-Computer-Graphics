#version 450

layout(location = 0) in vec3 iposition;
layout(location = 1) in vec3 icolor;
layout(location = 2) in vec3 inormal;
layout(location = 3) in vec2 txc;

out vec4 color;
out vec3 normal;
out vec4 pos;
out mat4 mv;
out vec3 lightD;
out vec2 texCoord;
out vec3 normalObject;

uniform mat4 mvp;
uniform mat3 mv3;
uniform vec3 lightDir;



void main() {
	lightD = mv3 * lightDir;
	color = vec4(icolor,1);
	pos = vec4(iposition, 1);
	normalObject = inormal;
	normal = mv3 * inormal;
	texCoord = txc;
	gl_Position =  mvp * vec4(iposition * 0.05, 1);
}
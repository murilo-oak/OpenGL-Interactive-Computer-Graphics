#version 450

layout(location = 0) in vec3 iposition;

out vec4 pos;
out mat4 mv;

uniform mat4 mvp;
uniform mat3 mv3;
uniform mat4 mv4;
uniform mat3 projection;
uniform vec3 lightDir;

void main() {
	pos = vec4(iposition, 1);
	gl_Position =  mvp * vec4(iposition, 1);
}
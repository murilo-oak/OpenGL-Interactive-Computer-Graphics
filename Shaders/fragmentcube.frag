#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
in mat4 mv;
in vec3 lightD;
in vec2 texCoord;

layout(location = 0) out vec4 ocolor;
uniform samplerCube skybox;

void main() {
	ocolor = vec4(1,0,0,1);
	ocolor = texture(skybox, pos.xyz);
}
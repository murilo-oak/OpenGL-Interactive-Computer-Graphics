#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
in mat4 mv;
in vec3 lightD;
in vec2 texCoord;
in vec3 normalObject;

layout(location = 0) out vec4 ocolor;
uniform sampler2D tex;

uniform vec3 cameraPos;
uniform samplerCube skybox;

uniform vec2 windowSize;

void main() {
	vec3 I = pos.xyz * 0.05 - cameraPos;
	vec3 N = normalObject;
	
	vec4 texColor =  texture(tex, vec2(gl_FragCoord.x/windowSize.x, -gl_FragCoord.y/windowSize.y));

	float test = dot(normalize(cameraPos), N);

	if(texColor.a > 0){
		ocolor = texColor;
	}
	else{
		ocolor = texture(skybox, reflect(I, N));
	}

}
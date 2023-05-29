#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
in vec3 lightD;
in vec2 texCoord;
in vec3 normalObject;

layout(location = 0) out vec4 ocolor;
uniform sampler2D tex;
uniform samplerCube skybox;

uniform vec3 cameraPos;

uniform mat3 mv3;
uniform mat4 mv4;
uniform mat4 invMv4;


float difuseLight( vec3 normalSurface, vec3 lightDir){
	return max(0, dot(normalSurface, lightDir));
}
void main() {
	vec3 I = pos.xyz * 0.05 - cameraPos;
	vec3 camDir = I;
	
	vec3 normalN = normalize(normal);
	vec3 halfVec = normalize((camDir + lightD)/2);
	
	float specular = dot(normalN, halfVec);
	float difuse = difuseLight(normalN, lightD);
	

	vec3 N = normalObject;

	ocolor = 0.9 * texture(skybox, reflect(I, N)) + 0.3 * (difuse + ceil(difuse) * pow(specular, 1500));;
}
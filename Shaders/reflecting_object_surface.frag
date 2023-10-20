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

float difuseLight( vec3 normalSurface, vec3 lightDir){
	return max(0, dot(normalSurface, lightDir));
}
void main() {
	vec3 camDir = pos.xyz * 0.05 - cameraPos;
	
	vec3 normalN = normalize(normal);
	vec3 halfVec = normalize((-camDir + lightD)/2);
	
	//Light calculations
	float specular = dot(normalN, halfVec);
	float difuse = difuseLight(normalN, lightD);
	
	//Masks
	float specularMask = clamp(dot(normalN, halfVec), 0, 1);
	float specularLight = pow(specularMask, 500);
	
	difuse += 0.4;
	difuse = clamp(difuse, 0, 1);

	bool difuseMask = difuse > 0;

	//Final specular light calculation. There's specular light only where there is difuse light
	specularLight *= int(difuseMask);

	vec3 N = normalObject;

	//Final Color shader
	ocolor = (difuse + 0.2) * (texture(skybox, reflect(camDir, N))) + specularLight;
}
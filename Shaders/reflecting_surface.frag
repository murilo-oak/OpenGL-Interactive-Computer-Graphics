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
	
	vec3 camDir = normalize(mv3 * pos.xyz);
	vec3 normalN = normalize(normal);
	vec3 halfVec = normalize((camDir + lightD)/2);
	float specular = dot(normalN, halfVec);
	float difuse = difuseLight(normalN, lightD);
	vec3 I = -(pos.xyz - cameraPos.xyz);
	vec3 N = normalObject;
	ocolor = normalize(I.xyz).xyzz;
	ocolor = reflect(I, N).xyzz;
	ocolor = texture(skybox, reflect(I, N));
	//ocolor = normalObject.xyzz; 

	//ocolor = normalN.xyzz;
	//ocolor = normalize(camDir.xyz).xyzz;
	//ocolor = camDir.xyzz;
	//vec3 I = normalize((invMv4 * vec4(normalize(camDir),1)).xyz);
	//ocolor = 0.9 * texture(skybox, -reflect(I, normalObject)) + 0.1 *  (difuse + ceil(difuse) * pow(specular, 1500));;
}
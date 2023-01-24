#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
in mat4 mv;
in vec3 lightD;

layout(location = 0) out vec4 ocolor;


float difuseLight( vec3 normalSurface, vec3 lightDir){
	return max(0, dot(normalSurface, lightDir));
}
void main() {
	vec3 camDir = - (mv * pos).xyz;
	vec3 normalN = normalize(normal);
	vec3 halfVec = normalize((camDir + lightD)/2);
	float specular = dot(normalN, halfVec);
	float difuse = difuseLight(normalN, lightD);
	//ocolor = vec4(pow(specular, 90).xxx, 1);
	ocolor =  (difuse * color + ceil(difuse) * pow(specular, 2000));
}
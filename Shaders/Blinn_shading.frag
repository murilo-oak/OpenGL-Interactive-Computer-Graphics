#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
in mat4 mv;
in vec3 lightD;
in vec2 texCoord;

layout(location = 0) out vec4 ocolor;
uniform sampler2D tex;


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
	//ocolor =  (difuse * color + ceil(difuse) * pow(specular, 2000));
	//ocolor =  texture(tex, vec2(texCoord.x/2, texCoord.y/2)) * (difuse +0.1f+ ceil(difuse) * pow(specular, 2000));
	ocolor =  texture(tex, vec2(texCoord.x, texCoord.y)) * (difuse +0.2f+ ceil(difuse) * pow(specular, 2000));
	//ocolor = vec4(1-texCoord.x, 1-texCoord.y, 0, 1);
	//ocolor = color;
	//ocolor = vec4(normalN,1);
}
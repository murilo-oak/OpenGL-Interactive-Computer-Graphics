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
uniform mat4 mvp;

uniform vec3 cameraPos;


float difuseLight( vec3 normalSurface, vec3 lightDir){
	return max(0, dot(normalSurface, lightDir));
}
void main() {
	//ocolor = vec4(normal,1);
	vec3 cameraToObject = normalize((0.05*pos.xyz ) - cameraPos);

	//vec3 cameraToObject = (inverse(mv) * texCoord.xyxy).xyz;

	vec4 newtex =  inverse(mvp) * texCoord.xyxy;

	vec3 camDir = - (mv * pos).xyz;
	vec3 normalN = normalize(normal);
	vec3 halfVec = normalize((camDir + lightD)/2);
	float specular = dot(normalN, halfVec);
	float difuse = difuseLight(normalN, lightD);
	//ocolor = vec4(pow(specular, 90).xxx, 1);
	//ocolor =  (difuse * color + ceil(difuse) * pow(specular, 2000));
	ocolor =  texture(tex, vec2(texCoord.x, texCoord.y)) * (difuse +0.1f+ ceil(difuse) * pow(specular, 2000));
	//ocolor =  texture(tex, vec2(cameraToObject.x, cameraToObject.y));
	//ocolor =  texture(tex, vec2(newtex.x, newtex.y));
	//ocolor = vec4(1,0,0,0);
	//ocolor = texture(tex, 0.1*texCoord);
	//ocolor = vec4(1,0,0,1);
	//ocolor =  texture(tex, vec2(texCoord.x, 1-texCoord.y));
	//ocolor = vec4(texCoord.x, texCoord.y, 0, 1);
	//ocolor = color;
	//ocolor = normalN.xyzz;
}
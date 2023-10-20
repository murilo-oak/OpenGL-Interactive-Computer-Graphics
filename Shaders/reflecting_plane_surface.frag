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
uniform samplerCube skybox;

uniform vec3 cameraPos;
uniform vec2 windowSize;

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
	float specularLight = pow(specularMask, 2000);
	
	difuse += 0.4;
	difuse = clamp(difuse, 0, 1);

	//Final specular light calculation. There's specular light only where there is difuse light
	bool test = difuse > 0;
	specularLight *= int(test);

	vec3 N = normalObject;
	
	//Get pixel color that will rendered by shader.
	vec4 texColor =  texture(tex, vec2(gl_FragCoord.x/windowSize.x, -gl_FragCoord.y/windowSize.y));
	vec4 skyTex = texture(skybox, reflect(camDir, N));

	//if texColor has alpha > 1, it means that it renders the redenred texture of the object reflecting cubemap
	if(texColor.a > 0){
		ocolor =  (difuse + 0.2) * texColor + specularLight;
	}
	else{ // renders the cube map, if there's no object in the front
		ocolor =  (difuse + 0.2) * skyTex + specularLight;
	}

}
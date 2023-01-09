#version 450

in vec4 color;
in vec3 normal;
in vec4 pos;
layout(location = 0) out vec4 ocolor;

void main() {
	
	//ocolor = color;
	//ocolor = vec4(abs(normal.x), abs(normal.y), abs(normal.z), 1);
	//ocolor = pos;
	ocolor = vec4(normalize(normal), 1);
    //ocolor = vec4(normal, 1);
	//ocolor = vec4(1.0f,1.0f,1.0f, 1.0);
}
#pragma once
#include "object3D.h"

class Plane : public Object3D
{
public:
	Plane();
	~Plane();

	void generateBuffers();
	void set(unsigned texSizeWidth, unsigned texSizeHeight);
	
	void setFrameBuffer();
	void resizeFrameBuffer(unsigned int windowWidth, unsigned int windowHeight);
};


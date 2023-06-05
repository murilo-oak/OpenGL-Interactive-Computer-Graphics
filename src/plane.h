#pragma once
#include "object3D.h"

class Plane : public Object3D
{
public:
	Plane();
	~Plane();

	void generateBuffers();
	void set();
	void setFrameBuffer();
};


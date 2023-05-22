#pragma once
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include"../cyCodeBase/cyGL.h"
#include "camera.h"
#include "cubemap.h"
#include "object3D.h"
#include "plane.h"



class IScene
{
public:
	IScene()
	{
	}
	bool m_setup{ false };

	//virtual void setup() = 0;
	virtual void setup(float angleX, float angleY, unsigned int windowHeight, unsigned int windowWidth, Camera& cam, Object3D& object3D, Plane& plane, cy::GLSLProgram& program, cy::GLSLProgram& skyboxProgram, glm::vec3& lightDir, Cubemap& cube) = 0;
	virtual void update() = 0;
	virtual void render() = 0;
};


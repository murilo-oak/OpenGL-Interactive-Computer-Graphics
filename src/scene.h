#pragma once
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include"../cyCodeBase/cyGL.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "camera.h"
#include "cubemap.h"
#include "object3D.h"
#include "plane.h"
#include "mouseinput.h"

class IScene
{
public:
	IScene()
	{
	}
	bool m_setup{ false };

	//virtual void setup() = 0;
	virtual void setup(unsigned int windowHeight, unsigned int windowWidth, Object3D& object3D, Plane& plane, cy::GLSLProgram& program, cy::GLSLProgram& skyboxProgram, Cubemap& cube) = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void onRightButton(MouseInput mouse) = 0;
	virtual void onLeftButton(MouseInput mouse) = 0;
	virtual void onLeftButton2(MouseInput mouse) = 0;
};


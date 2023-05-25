#pragma once
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include"../ThirdParty/include/cyCodeBase/cyGL.h"

#include "camera.h"
#include "cubemap.h"
#include "object3D.h"
#include "plane.h"
#include "mouseinput.h"

#include "../ThirdParty/include/glm/glm.hpp"
#include "../ThirdParty/include/glm/gtc/matrix_transform.hpp"
#include "../ThirdParty/include/glm/gtc/type_ptr.hpp"

class IScene
{
public:
	IScene(){}

	bool m_setup{ false };

	virtual void setup(unsigned int windowHeight, unsigned int windowWidth) = 0;
	virtual void update() = 0;
	virtual void render() = 0;
	virtual void onRightButton(MouseInput mouse) = 0;
	virtual void onLeftButton(MouseInput mouse) = 0;
	virtual void onLeftButton2(MouseInput mouse) = 0;
	virtual void recompileShaders() = 0;
};


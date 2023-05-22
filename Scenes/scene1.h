#pragma once
#include "../src/scene.h"

class Scene1 : public IScene
{
public:
	void setup(float angleX, float angleY, unsigned int windowHeight, unsigned int windowWidth, Camera& cam, Object3D& object3D, Plane& plane, cy::GLSLProgram& program, cy::GLSLProgram& skyboxProgram, glm::vec3& lightDir, Cubemap& cube) override;
	void update() override;
	void render() override;
};


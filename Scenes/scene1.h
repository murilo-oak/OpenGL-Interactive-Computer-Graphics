#pragma once
#include "../src/scene.h"

class Scene1 : public IScene
{
public:
	Camera cam{};
	Object3D object3D{};
	Plane plane{};

	Cubemap cube{};

	cy::GLSLProgram program{};
	cy::GLSLProgram skyboxProgram{};

	float angleX{ 0.90f };
	float angleY{ 0.90f };

	glm::vec3 lightDir{ 0.0f, 1.0f, 0.0f };
	unsigned int m_windowHeight{}, m_windowWidth{};

	void setup(unsigned int windowHeight, unsigned int windowWidth) override;
	void update() override;
	void render() override;

	void onRightButton(MouseInput mouse) override;
	void onLeftButton(MouseInput mouse) override;
	void onLeftButton2(MouseInput mouse) override;

	void setUniformVariables(GLuint programID, unsigned int windowHeight, unsigned int windowWidth);
	void updateLightCamUniforms(GLuint programID);
	void updateUniformVariables(GLuint programID);
};


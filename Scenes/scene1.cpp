#include "scene1.h"


void Scene1::setUniformVariables(GLuint programID, unsigned int windowHeight, unsigned int windowWidth,
		cy::GLSLProgram& program, cy::GLSLProgram& skyboxProgram, glm::vec3& lightDir) {
	glUseProgram(programID);

	GLint sampler{};
	sampler = glGetUniformLocation(programID, "skybox");
	glUniform1i(sampler, 0);

	sampler = glGetUniformLocation(programID, "tex");
	glUniform1i(sampler, 0);

	if (programID == skyboxProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(cam.m_projection * glm::mat4(glm::mat3(cam.m_view))));
	}

	if (programID == program.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(cam.m_mvp));
	}

	GLint uniformLocmv = glGetUniformLocation(programID, "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(cam.m_mv));

	GLint uniformLocmv4 = glGetUniformLocation(programID, "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(cam.m_mv4));

	uniformLocmv4 = glGetUniformLocation(programID, "invMv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(cam.m_invMv4));

	GLint uniformTransLoc = glGetUniformLocation(program.GetID(), "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &lightDir[0]);
}

void Scene1::setup( unsigned int windowHeight, unsigned int windowWidth, cy::GLSLProgram &program, cy::GLSLProgram &skyboxProgram, Cubemap& cube) {
	cube.loadImageFilesCubeMap(
		"cubemap/cubemap_posx.png",
		"cubemap/cubemap_negx.png",
		"cubemap/cubemap_posy.png",
		"cubemap/cubemap_negy.png",
		"cubemap/cubemap_posz.png",
		"cubemap/cubemap_negz.png"
	);

	cam.updatePosition(angleX, angleY);
	cam.setMVP(windowWidth, windowHeight);

	object3D.loadFromFile("teapot.obj");
	object3D.set();
	plane.set();

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/reflecting_surface.frag", GL_FRAGMENT_SHADER);
	//fragmentS.CompileFile("Shaders/Blinn_shading.frag", GL_FRAGMENT_SHADER);

	program.CreateProgram();
	program.AttachShader(fragmentS);
	program.AttachShader(vertexS);
	program.Link();

	object3D.setTexture(windowWidth, windowHeight);

	cube.set();

	setUniformVariables(program.GetID(), windowHeight, windowWidth, program, skyboxProgram, lightDir);

	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	skyboxProgram.CreateProgram();
	skyboxProgram.AttachShader(fragmentS);
	skyboxProgram.AttachShader(vertexS);
	skyboxProgram.Link();

	setUniformVariables(skyboxProgram.GetID(), windowHeight, windowWidth, program, skyboxProgram, lightDir);
};

void Scene1::update() 
{
	cam.update(angleX, angleY);
};

void Scene1::render() {};

void Scene1::onRightButton(MouseInput mouse) {
	angleY += (mouse.getY() - mouse.getLastY()) / 400.0f;
	angleX += (mouse.getX() - mouse.getLastX()) / 400.0f;
};

void Scene1::onLeftButton(MouseInput mouse)
{
	cam.updatePosition(angleX, angleY, ((mouse.getY() - mouse.getLastY()) / 40.0f));
};

void Scene1::onLeftButton2(MouseInput mouse)
{
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), (mouse.getX() - mouse.getLastX()) / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), (mouse.getY() - mouse.getLastY()) / 40.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 newLightDir = rotationMatrixX * rotationMatrixY * glm::vec4(glm::vec3(lightDir), 1.0f);
	lightDir = newLightDir;
};
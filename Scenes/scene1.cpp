#include "scene1.h"


void Scene1::setUniformVariables(GLuint programID, unsigned int windowHeight, unsigned int windowWidth) 
{
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

void Scene1::setup(unsigned int windowHeight, unsigned int windowWidth) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

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

	setUniformVariables(program.GetID(), windowHeight, windowWidth);

	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	skyboxProgram.CreateProgram();
	skyboxProgram.AttachShader(fragmentS);
	skyboxProgram.AttachShader(vertexS);
	skyboxProgram.Link();

	setUniformVariables(skyboxProgram.GetID(), windowHeight, windowWidth);
};

void Scene1::update() 
{
	cam.update(angleX, angleY);
};

void Scene1::render() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glUseProgram(skyboxProgram.GetID());

	glDepthMask(GL_FALSE);
	glBindVertexArray(cube.m_vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube.texCubeID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);


	glUseProgram(program.GetID());


	glBindVertexArray(object3D.m_vao);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, object3D.m_texWidth, object3D.m_texHeight);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object3D.m_texID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object3D.m_ebo);

	glDrawElements(GL_TRIANGLES, object3D.m_facesIndex.size(), GL_UNSIGNED_INT, 0);

	/*glGenerateTextureMipmap(renderedTexture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vaoPlane);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboPlane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/


	//glDrawArrays(GL_POINTS, 0, objectVertices.size());
	glutSwapBuffers();
};

void Scene1::onRightButton(MouseInput mouse) {
	angleY += (mouse.getY() - mouse.getLastY()) / 400.0f;
	angleX += (mouse.getX() - mouse.getLastX()) / 400.0f;

	updateUniformVariables(program.GetID());
	updateUniformVariables(skyboxProgram.GetID());
};

void Scene1::onLeftButton(MouseInput mouse)
{
	cam.updatePosition(angleX, angleY, ((mouse.getY() - mouse.getLastY()) / 40.0f));

	updateUniformVariables(program.GetID());
	updateUniformVariables(skyboxProgram.GetID());
};

void Scene1::onLeftButton2(MouseInput mouse)
{
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), (mouse.getX() - mouse.getLastX()) / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), (mouse.getY() - mouse.getLastY()) / 40.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 newLightDir = rotationMatrixX * rotationMatrixY * glm::vec4(glm::vec3(lightDir), 1.0f);
	lightDir = newLightDir;

	updateLightCamUniforms(program.GetID());
};

void Scene1::updateLightCamUniforms(GLuint programID) {
	glUseProgram(programID);

	GLint uniformTransLoc = glGetUniformLocation(programID, "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &lightDir[0]);
}

void Scene1::updateUniformVariables(GLuint programID) {
	glUseProgram(programID);

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

	updateLightCamUniforms(programID);
}
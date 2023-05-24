#include "scene1.h"

void Scene1::setUniformVariables(GLuint programID, unsigned int windowHeight, unsigned int windowWidth) 
{
	glUseProgram(programID);

	GLint sampler{};
	sampler = glGetUniformLocation(programID, "skybox");
	glUniform1i(sampler, 0);

	sampler = glGetUniformLocation(programID, "tex");
	glUniform1i(sampler, 0);

	if (programID == m_skyboxProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_projection * glm::mat4(glm::mat3(m_cam.m_view))));
	}

	if (programID == m_objectProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_mvp));
	}

	GLint uniformLocmv = glGetUniformLocation(programID, "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv));

	GLint uniformLocmv4 = glGetUniformLocation(programID, "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv4));

	uniformLocmv4 = glGetUniformLocation(programID, "invMv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_invMv4));

	GLint uniformTransLoc = glGetUniformLocation(m_objectProgram.GetID(), "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &m_cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &m_lightDir[0]);
}

void Scene1::setup(unsigned int windowHeight, unsigned int windowWidth) {
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	m_cubemap.loadImageFilesCubeMap(
		"cubemap/cubemap_posx.png",
		"cubemap/cubemap_negx.png",
		"cubemap/cubemap_posy.png",
		"cubemap/cubemap_negy.png",
		"cubemap/cubemap_posz.png",
		"cubemap/cubemap_negz.png"
	);

	m_cam.updatePosition();
	m_cam.setMVP(windowWidth, windowHeight);

	m_object3D.set("teapot.obj");
	m_plane.set();

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/reflecting_surface.frag", GL_FRAGMENT_SHADER);
	//fragmentS.CompileFile("Shaders/Blinn_shading.frag", GL_FRAGMENT_SHADER);

	m_objectProgram.CreateProgram();
	m_objectProgram.AttachShader(fragmentS);
	m_objectProgram.AttachShader(vertexS);
	m_objectProgram.Link();

	m_object3D.setTexture(windowWidth, windowHeight);

	m_cubemap.set();

	setUniformVariables(m_objectProgram.GetID(), windowHeight, windowWidth);

	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	m_skyboxProgram.CreateProgram();
	m_skyboxProgram.AttachShader(fragmentS);
	m_skyboxProgram.AttachShader(vertexS);
	m_skyboxProgram.Link();

	setUniformVariables(m_skyboxProgram.GetID(), windowHeight, windowWidth);
};

void Scene1::update() 
{
	m_cam.update();
	updateUniformVariables(m_objectProgram.GetID());
	updateUniformVariables(m_skyboxProgram.GetID());
};

void Scene1::render() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	glUseProgram(m_skyboxProgram.GetID());

	glDepthMask(GL_FALSE);
	glBindVertexArray(m_cubemap.m_vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.texCubeID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);

	glEnable(GL_DEPTH_TEST);


	glUseProgram(m_objectProgram.GetID());


	glBindVertexArray(m_object3D.m_vao);
	
	/// <summary>
	/// 
	/// </summary>
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_object3D.m_frameBuffer);
	/// <summary>
	/// 
	/// </summary>
	
	glViewport(0, 0, m_object3D.m_texWidth, m_object3D.m_texHeight);
	
	/// <summary>
	/// ////
	/// </summary>
	//glClear(GL_DEPTH_BUFFER_BIT);
	////////


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_object3D.m_texID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_object3D.m_ebo);

	glDrawElements(GL_TRIANGLES, m_object3D.m_facesIndex.size(), GL_UNSIGNED_INT, 0);

	/// <summary>
	/// //////////////
	/// </summary>
	glGenerateTextureMipmap(m_plane.m_renderedTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	//glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(m_plane.m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_ebo);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_plane.m_renderedTexture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	////////////////////////////////////////

	//glDrawArrays(GL_POINTS, 0, objectVertices.size());
	glutSwapBuffers();
};

void Scene1::onRightButton(MouseInput mouse) {
	m_cam.m_angle.x += mouse.getDeltaX() / 400.0f;
	m_cam.m_angle.y += mouse.getDeltaY() / 400.0f;
};

void Scene1::onLeftButton(MouseInput mouse)
{
	m_cam.updatePosition(mouse.getDeltaY() / 40.0f);
};

void Scene1::onLeftButton2(MouseInput mouse)
{
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), mouse.getDeltaX() / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), mouse.getDeltaY() / 40.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	
	m_lightDir = rotationMatrixX * rotationMatrixY * glm::vec4(glm::vec3(m_lightDir), 1.0f);
};

void Scene1::updateUniformVariables(GLuint programID) {
	glUseProgram(programID);

	if (programID == m_skyboxProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_projection * glm::mat4(glm::mat3(m_cam.m_view))));
	}

	if (programID == m_objectProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_mvp));
	}

	GLint uniformLocmv = glGetUniformLocation(programID, "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv));

	GLint uniformLocmv4 = glGetUniformLocation(programID, "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv4));

	uniformLocmv4 = glGetUniformLocation(programID, "invMv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_invMv4));

	GLint uniformTransLoc = glGetUniformLocation(programID, "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &m_cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &m_lightDir[0]);
}
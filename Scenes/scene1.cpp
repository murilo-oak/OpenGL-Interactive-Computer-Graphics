#include "scene1.h"

void Scene1::setup(unsigned int windowWidth, unsigned int windowHeight) {
	// Store the window dimensions
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	// Load the image files to create the cubemap
	m_cubemap.loadImageFilesCubeMap(
		"cubemap/cubemap_posx.png",
		"cubemap/cubemap_negx.png",
		"cubemap/cubemap_posy.png",
		"cubemap/cubemap_negy.png",
		"cubemap/cubemap_posz.png",
		"cubemap/cubemap_negz.png"
	);
	
	// Set camera position and MVP mat
	m_cam.updatePosition();
	m_cam.setMVP(windowWidth, windowHeight);

	//Set objects
	m_object3D.set("3DObjects/teapot.obj");
	m_plane.set(m_windowWidth, m_windowHeight);
	
	// Compile and link the shaders for the object program
	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/reflecting_object_surface.frag", GL_FRAGMENT_SHADER);

	m_objectProgram.CreateProgram();
	m_objectProgram.AttachShader(fragmentS);
	m_objectProgram.AttachShader(vertexS);
	m_objectProgram.Link();
	
	// Set the texture of the 3D object with the window dimensions
	m_object3D.setTexture(windowWidth, windowHeight);

	m_cubemap.set();
	
	// Set the uniform variables for the object program
	setUniformVariables(m_objectProgram.GetID(), windowHeight, windowWidth);

	// Compile and link the shaders for the skybox program
	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	m_skyboxProgram.CreateProgram();
	m_skyboxProgram.AttachShader(fragmentS);
	m_skyboxProgram.AttachShader(vertexS);
	m_skyboxProgram.Link();

	// Set the uniform variables for the skybox program
	setUniformVariables(m_skyboxProgram.GetID(), windowHeight, windowWidth);

	// Compile and link the shaders for the reflecting plane program
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/reflecting_plane_surface.frag", GL_FRAGMENT_SHADER);

	m_planeProgram.CreateProgram();
	m_planeProgram.AttachShader(fragmentS);
	m_planeProgram.AttachShader(vertexS);
	m_planeProgram.Link();

	// Set the uniform variables for the reflecting plane program
	setUniformVariables(m_planeProgram.GetID(), windowHeight, windowWidth);
	
};

void Scene1::update() 
{
	m_cam.update();

	updateUniformVariables(m_objectProgram.GetID());
	updateUniformVariables(m_skyboxProgram.GetID());
	updateUniformVariables(m_planeProgram.GetID());
};

void Scene1::render() 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_windowWidth, m_windowHeight);
	
	// Draw skybox before anything
	drawSkybox();
	
	glEnable(GL_DEPTH_TEST);
	
	draw3dObj();
	drawPlane();
	
	glutSwapBuffers();
};

void inline Scene1::drawSkybox()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glUseProgram(m_skyboxProgram.GetID());

	glDepthMask(GL_FALSE);
	
	glBindVertexArray(m_cubemap.m_vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.m_texCubeID);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthMask(GL_TRUE);
}

void inline Scene1::draw3dObj() 
{
	glUseProgram(m_objectProgram.GetID());

	glBindVertexArray(m_object3D.m_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_object3D.m_texID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_object3D.m_ebo);

	glDrawElements(GL_TRIANGLES, m_object3D.m_facesIndex.size(), GL_UNSIGNED_INT, 0);
}

void inline Scene1::drawPlane() 
{
	// Bind framebuffer and setup frame
	glBindFramebuffer(GL_FRAMEBUFFER, m_plane.m_frameBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_windowWidth, m_windowHeight);

	// Bind the texture to be rendered
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_plane.m_renderedTexture);

	bool cameraCanSeeObject = glm::dot(m_cam.m_position, glm::vec3(0, 1, 0)) > 0;

	if (cameraCanSeeObject) {
		// Render the 3d object reflected on the plane
		glUseProgram(m_objectProgram.GetID());
		glBindVertexArray(m_object3D.m_vao);

		// Reflect camera view
		glm::mat4 view = glm::lookAt(
			glm::reflect(m_cam.m_position, glm::vec3(0, 1, 0)),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		glm::mat4 mvp = m_cam.m_projection * m_cam.m_model * view;

		// Send the reflected mvp to the shader
		GLint uniformLoc = glGetUniformLocation(m_objectProgram.GetID(), "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

		// Draw into texture the 3d object
		glDrawElements(GL_TRIANGLES, m_object3D.m_facesIndex.size(), GL_UNSIGNED_INT, 0);
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	// Draw plane with the rendered texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(m_planeProgram.GetID());

	glBindVertexArray(m_plane.m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_ebo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_plane.m_renderedTexture);
	glUniform1i(glGetUniformLocation(m_planeProgram.GetID(), "tex"), 0);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap.m_texCubeID);
	glUniform1i(glGetUniformLocation(m_planeProgram.GetID(), "skybox"), 1);

	// Drawplane
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Scene1::reshapeWindow(unsigned int windowWidth, unsigned int windowHeight) {

	// When height is too small, reshape viewport to not show the skybox boundaries
	if ((float)windowHeight / (float)windowWidth < 0.3) {
		reshapeWindow((float)windowHeight / 0.31, windowHeight);
		return;
	}

	// Makes sure that window won't crash if one of the window sides have size zero
	if (windowHeight > 0 && windowWidth > 0) {
		m_cam.setMVP(windowWidth, windowHeight);
		m_windowWidth = windowWidth;
		m_windowHeight = windowHeight;

		m_plane.resizeFrameBuffer(windowWidth, windowHeight);
	}
};

void Scene1::onRightButton(MouseInput mouse) {
	// Change position camera positon around scene origin
	m_cam.m_angle.x += mouse.getDeltaX() / 400.0f;
	m_cam.m_angle.y += mouse.getDeltaY() / 400.0f;
};

void Scene1::onLeftButton(MouseInput mouse)
{
	// Change camera zoom to the scene origin
	m_cam.updatePosition(mouse.getDeltaY() / 40.0f);
};

void Scene1::onLeftButton2(MouseInput mouse)
{
	// Change light direction
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), mouse.getDeltaX() / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), mouse.getDeltaY() / 40.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	
	m_lightDir = rotationMatrixX * rotationMatrixY * glm::vec4(glm::vec3(m_lightDir), 1.0f);
};

void Scene1::setUniformVariables(GLuint programID, unsigned int windowHeight, unsigned int windowWidth)
{
	glUseProgram(programID);

	GLint sampler{};
	sampler = glGetUniformLocation(programID, "skybox");
	glUniform1i(sampler, 0);

	sampler = glGetUniformLocation(programID, "tex");
	glUniform1i(sampler, 0);

	updateUniformVariables(programID);
	
}

void Scene1::updateUniformVariables(GLuint programID) {
	glUseProgram(programID);

	if (programID == m_skyboxProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_projection * glm::mat4(glm::mat3(m_cam.m_view))));
	}
	else {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(m_cam.m_mvp));
	}

	GLint uniformLocmv = glGetUniformLocation(programID, "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv));

	GLint uniformLocmv4 = glGetUniformLocation(programID, "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_mv4));

	uniformLocmv4 = glGetUniformLocation(programID, "invMv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(m_cam.m_invMv4));

	if (programID == m_objectProgram.GetID() || programID ==m_planeProgram.GetID()) {
		GLint uniformTransLoc = glGetUniformLocation(programID, "cameraPos");
		glUniform3fv(uniformTransLoc, 1, &m_cam.m_position[0]);
	}
	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &m_lightDir[0]);

	GLuint uniformWindowSize = glGetUniformLocation(programID, "windowSize");
	glUniform2fv(uniformWindowSize, 1, glm::value_ptr(glm::vec2(m_windowWidth, m_windowHeight)));
}

void Scene1::recompileShaders() 
{
	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/reflecting_object_surface.frag", GL_FRAGMENT_SHADER);

	m_objectProgram.CreateProgram();
	m_objectProgram.AttachShader(fragmentS);
	m_objectProgram.AttachShader(vertexS);
	m_objectProgram.Link();

	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	m_skyboxProgram.CreateProgram();
	m_skyboxProgram.AttachShader(fragmentS);
	m_skyboxProgram.AttachShader(vertexS);
	m_skyboxProgram.Link();

	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/reflecting_plane_surface.frag", GL_FRAGMENT_SHADER);

	m_planeProgram.CreateProgram();
	m_planeProgram.AttachShader(fragmentS);
	m_planeProgram.AttachShader(vertexS);
	m_planeProgram.Link();
}
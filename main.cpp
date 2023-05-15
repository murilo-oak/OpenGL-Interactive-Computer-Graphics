#include <iostream>
#include <vector>
#include <algorithm>
#define GLEW_STATIC
#include "GL/glew.h"
#include <GL/freeglut.h>

#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyGL.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "lodepng/lodepng.h"

#include "src/Cubemap.h"
#include "src/camera.h"
#include "src/object3D.h"

GLuint vao{};
GLuint vaoPlane{};
GLuint vaoCube{};

GLuint vbo{};
GLuint vboPlane{};
GLuint vboCube{};

GLuint vboNormals{};
GLuint vboNormalsPlane{};
GLuint vboNormalsCube{};

GLuint vboTexCoords{};
GLuint vboTexCoordsPlane{};
GLuint vboTexCoordsCube{};

GLuint ebo{};
GLuint eboPlane{};
GLuint eboCube{};

GLuint shaderProgram;
GLuint shaderSkyboxProgram;

GLuint frameBuffer{};
GLuint renderedTexture{};
int texID{};

GLuint texCubeID{};

std::vector<cy::Vec3f> geometryMesh;
std::vector<unsigned int> facesIndex;

int preMouseX;
int preMouseY;

int windowWidth{500};
int windowHeight{500};

int texWidth{ 500 }, texHeight{ 500 };

float angleX = 0.90f;
float angleY = 0.90f;


std::vector<Object3D::vertex> vertices{
	// positions          // colors           
	{13.5f,  13.5f, 0.0f,   1.0f, 0.0f, 0.0f},   // top right
	{13.5f, -13.5f, 0.0f,   0.0f, 1.0f, 0.0f },   // bottom right
	{-13.5f, -13.5f, 0.0f,   0.0f, 0.0f, 1.0f},   // bottom left
	{-13.5f,  13.5f, 0.0f,   1.0f, 1.0f, 0.0f}   // top left 
};

std::vector<Object3D::normal> normalsPlane{
	// positions          // colors           
	{0.0f, 0.0f, 1.0f},   // top right
	{0.0f, 0.0f, 1.0f },   // bottom right
	{0.0f, 0.0f, 1.0f},   // bottom left
	{0.0f, 0.0f, 1.0f}   // top left 
};

std::vector<glm::vec2> texcoordPlane{
	{1.0f, 1.0f},   // top right
	{1.0f, 0.0f },   // bottom right
	{0.0f, 0.0f},   // bottom left
	{0.0f, 1.0f}    // top left 
};

unsigned int facesPlane[] {
	2, 1, 0,
	2, 0, 3
};

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

glm::mat4 rotX = glm::mat4(1.0f);
glm::mat4 rotY = glm::mat4(1.0f);

glm::vec3 lightDir(0.0f, 1.0f, 0.0f);

std::vector<Object3D::vertex> objectVertices{};
std::vector<Object3D::normal> objectNormals{};
std::vector<glm::vec2> objectTexCoords{};

cy::GLSLProgram program;
cy::GLSLProgram skyboxProgram;

Cubemap cube;
Camera cam;
Object3D object3D;

void myIdle() {
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			glutLeaveMainLoop();
			break;
	}
	glutPostRedisplay();
}
void updateMouse(int x, int y) {
	preMouseX = x;
	preMouseY = y;
}

void setUniformVariables(GLuint programID) {
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

void updateLightCamUniforms(GLuint programID) {
	glUseProgram(programID);

	GLint uniformTransLoc = glGetUniformLocation(programID, "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &lightDir[0]);
}

void updateUniformVariables(GLuint programID) {
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

void onLeftButton(int x, int y) {
	cam.updatePosition(angleX, angleY, ((y - preMouseY) / 40.0f));
	cam.setViewMat();


	cam.updateMVP();

	updateUniformVariables(program.GetID());
	

	updateUniformVariables(skyboxProgram.GetID());

	glutPostRedisplay();

	updateMouse(x, y);
}


void onLeftButton2(int x, int y) {
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), (x - preMouseX) / 40.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), (y - preMouseY) / 40.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 newLightDir = rotationMatrixX * rotationMatrixY * glm::vec4(glm::vec3(lightDir), 1.0f);
	lightDir = newLightDir;
	
	updateLightCamUniforms(program.GetID());

	glutPostRedisplay();

	updateMouse(x, y);
}

void onRightButton(int x, int y) {
	angleY += (y - preMouseY) / 400.0f;
	angleX += (x - preMouseX) / 400.0f;
	
	//updateCameraPos();
	cam.updatePosition(angleX, angleY);
	cam.setViewMat();
	
	cam.updateMVP();

	updateUniformVariables(program.GetID());
	updateUniformVariables(skyboxProgram.GetID());

	glutPostRedisplay();

	updateMouse(x, y);
}

bool controlIsPressed = false;

void myMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			updateMouse(x, y);
			glutMotionFunc(onLeftButton);
		}
		else if (button == GLUT_RIGHT_BUTTON && !controlIsPressed) {
			updateMouse(x, y);
			glutMotionFunc(onRightButton);
		}	
	}
}
void motion(int x, int y) {
}

void  specialFunc(int key, int x, int y) {
	if (key == GLUT_KEY_CTRL_L) {
		updateMouse(x, y);
		glutMotionFunc(onLeftButton2);
	}

}

void myDisplayTeapot(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, texWidth, texHeight);
	glUseProgram(skyboxProgram.GetID());
	
	glDepthMask(GL_FALSE);
	glBindVertexArray(vaoCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_DEPTH_TEST);


	glUseProgram(program.GetID());


	glBindVertexArray(vao);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, texWidth, texHeight);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
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
}
void setObject() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//positions
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glNamedBufferStorage(vbo, object3D.m_vertices.size() * sizeof(Object3D::vertex), object3D.m_vertices.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, r));

	glEnableVertexAttribArray(0);//antes de renderizar
	glEnableVertexAttribArray(1);//antes de renderizar

	//normals
	glCreateBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glNamedBufferStorage(vboNormals, object3D.m_normals.size() * sizeof(Object3D::normal), object3D.m_normals.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::normal), 0);

	glEnableVertexAttribArray(2);//antes de renderizar

	//texcoord
	glCreateBuffers(1, &vboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glNamedBufferStorage(vboTexCoords, object3D.m_texCoords.size() * sizeof(glm::vec2), object3D.m_texCoords.data(), 0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(3);

	glGenVertexArrays(1, &vaoPlane);
	glBindVertexArray(vaoPlane);

	//indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object3D.m_facesIndex.size() * sizeof(unsigned int), object3D.m_facesIndex.data(), GL_STATIC_DRAW);
}

void setFrameBuffer() {
	//framebuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	//renderedTexture
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//depth buffer
	GLuint depthBuffer{};
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texWidth, texHeight);

	//configure framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };

	//if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
	//	return false;
	//}
}

void setPlane() {
	//positions
	glCreateBuffers(1, &vboPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
	glNamedBufferStorage(vboPlane, 4 * sizeof(Object3D::vertex), vertices.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, r));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	//normals
	glCreateBuffers(1, &vboNormalsPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalsPlane);
	glNamedBufferStorage(vboNormalsPlane, normalsPlane.size() * sizeof(Object3D::normal), normalsPlane.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::normal), 0);

	glEnableVertexAttribArray(2);//antes de renderizar

	//texcoord
	glCreateBuffers(1, &vboTexCoordsPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoordsPlane);
	glNamedBufferStorage(vboTexCoordsPlane, texcoordPlane.size() * sizeof(glm::vec2), texcoordPlane.data(), 0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(3);

	glGenBuffers(1, &eboPlane);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboPlane);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), facesPlane, GL_STATIC_DRAW);

	setFrameBuffer();
}
void setObjectTexture(unsigned width, unsigned height, std::vector<unsigned char> image) {
	glGenTextures(1, &texCubeID);
	//texture
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	//filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Tiling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
}

void setCubemapTexConfig() {
	glGenTextures(1, &texCubeID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeID);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, cube.width, cube.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cube.image[i].data());
	}

	//cube filter parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void setCubemapConfig() {
	//Cubemap
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	//positions
	glCreateBuffers(1, &vboCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glNamedBufferStorage(vboCube, sizeof(skyboxVertices), skyboxVertices, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("OpenGL - Maxwell, the reflective Cat");

	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(motion);
	glutSpecialFunc (specialFunc);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	CY_GL_REGISTER_DEBUG_CALLBACK;
	
	//scene start
	cube.loadImageFilesCubeMap(
		"cubemap/cubemap_posx.png",
		"cubemap/cubemap_negx.png",
		"cubemap/cubemap_posy.png",
		"cubemap/cubemap_negy.png",
		"cubemap/cubemap_posz.png",
		"cubemap/cubemap_negz.png"
	);

	cam.updatePosition(angleX, angleY);
	cam.setMVP(windowHeight, windowHeight);
	
	object3D.loadFromFile("teapot.obj");

	setObject();
	setPlane();

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/reflecting_surface.frag", GL_FRAGMENT_SHADER);
	//fragmentS.CompileFile("Shaders/Blinn_shading.frag", GL_FRAGMENT_SHADER);
	
	program.CreateProgram();
	program.AttachShader(fragmentS);
	program.AttachShader(vertexS);
	program.Link();

	//glUseProgram(program.GetID());
	setObjectTexture(windowWidth, windowHeight, object3D.m_image);
	setCubemapTexConfig();
	setUniformVariables(program.GetID());
	setCubemapConfig();


	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	skyboxProgram.CreateProgram();
	skyboxProgram.AttachShader(fragmentS);
	skyboxProgram.AttachShader(vertexS);
	skyboxProgram.Link();

	setUniformVariables(skyboxProgram.GetID());
	//scene end

	glutDisplayFunc(myDisplayTeapot);
	glutMainLoop();

	return 0;
}
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
#include "src/plane.h"

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
Plane plane;

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

void myDisplay(){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, texWidth, texHeight);
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
	glViewport(0, 0, texWidth, texHeight);
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
	object3D.set();
	plane.set();

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	//fragmentS.CompileFile("Shaders/reflecting_surface.frag", GL_FRAGMENT_SHADER);
	fragmentS.CompileFile("Shaders/Blinn_shading.frag", GL_FRAGMENT_SHADER);	
	
	program.CreateProgram();
	program.AttachShader(fragmentS);
	program.AttachShader(vertexS);
	program.Link();

	object3D.setTexture(windowWidth, windowHeight);
	
	cube.set();

	setUniformVariables(program.GetID());

	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);

	skyboxProgram.CreateProgram();
	skyboxProgram.AttachShader(fragmentS);
	skyboxProgram.AttachShader(vertexS);
	skyboxProgram.Link();

	setUniformVariables(skyboxProgram.GetID());
	//scene end

	glutDisplayFunc(myDisplay);
	glutMainLoop();

	return 0;
}
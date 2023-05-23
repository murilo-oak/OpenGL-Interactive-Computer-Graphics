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
#include "src/mouseinput.h"

#include "Scenes/scene1.h"

Scene1 scene1{};

GLuint frameBuffer{};
GLuint renderedTexture{};
int texID{};

int windowWidth{ 500 };
int windowHeight{ 500 };

MouseInput mouse{};

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
void updateLightCamUniforms(GLuint programID) {
	glUseProgram(programID);

	GLint uniformTransLoc = glGetUniformLocation(programID, "cameraPos");
	glUniform3fv(uniformTransLoc, 1, &scene1.cam.m_position[0]);

	GLint uniformLightDir = glGetUniformLocation(programID, "lightDir");
	glUniform3fv(uniformLightDir, 1, &scene1.lightDir[0]);
}
void updateUniformVariables(GLuint programID) {
	glUseProgram(programID);

	if (programID == scene1.skyboxProgram.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(scene1.cam.m_projection * glm::mat4(glm::mat3(scene1.cam.m_view))));
	}

	if (programID == scene1.program.GetID()) {
		GLint uniformLoc = glGetUniformLocation(programID, "mvp");
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(scene1.cam.m_mvp));
	}

	GLint uniformLocmv = glGetUniformLocation(programID, "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(scene1.cam.m_mv));

	GLint uniformLocmv4 = glGetUniformLocation(programID, "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(scene1.cam.m_mv4));

	uniformLocmv4 = glGetUniformLocation(programID, "invMv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(scene1.cam.m_invMv4));

	updateLightCamUniforms(programID);
}
void onLeftButton(int x, int y) {
	mouse.update(x, y);

	scene1.onLeftButton(mouse);

	updateUniformVariables(scene1.program.GetID());
	updateUniformVariables(scene1.skyboxProgram.GetID());

	glutPostRedisplay();
}
void onLeftButton2(int x, int y) {

	mouse.update(x, y);
	scene1.onLeftButton2(mouse);
	
	updateLightCamUniforms(scene1.program.GetID());

	glutPostRedisplay();
}
void onRightButton(int x, int y) {
	mouse.update(x, y);

	scene1.onRightButton(mouse);

	updateUniformVariables(scene1.program.GetID());
	updateUniformVariables(scene1.skyboxProgram.GetID());

	glutPostRedisplay();
}
bool controlIsPressed = false;
void myMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			mouse.update(x, y);
			glutMotionFunc(onLeftButton);
		}
		else if (button == GLUT_RIGHT_BUTTON && !controlIsPressed) {
			mouse.update(x, y);
			glutMotionFunc(onRightButton);
		}	
	}
}
void specialFunc(int key, int x, int y) {
	if (key == GLUT_KEY_CTRL_L) {
		mouse.update(x, y);
		glutMotionFunc(onLeftButton2);
	}
}
void myDisplay(){
	scene1.update();
	scene1.render();
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
	glutSpecialFunc (specialFunc);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	CY_GL_REGISTER_DEBUG_CALLBACK;

	scene1.setup(windowHeight, windowWidth);

	glutDisplayFunc(myDisplay);
	glutMainLoop();

	return 0;
}
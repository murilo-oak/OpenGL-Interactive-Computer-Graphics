#include <iostream>
#include <vector>
#include <algorithm>

#define GLEW_STATIC
#include "ThirdParty/include/GL/glew.h"
#include "ThirdParty/include/GL/freeglut.h"

#include "ThirdParty/include/cyCodeBase/cyTriMesh.h"
#include "ThirdParty/include/cyCodeBase/cyGL.h"

#include "ThirdParty/include/glm/glm.hpp"
#include "ThirdParty/include/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/include/glm/gtc/type_ptr.hpp"

#include "ThirdParty/include/lodepng/lodepng.h"
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
		case 'r':
			std::cout << "recompile" << std::endl;
			scene1.recompileShaders();
			break;
		case 'R':
			std::cout << "recompile" << std::endl;
			scene1.recompileShaders();
			break;
	}
	glutPostRedisplay();
}

void onLeftButton(int x, int y) {
	
	mouse.update(x, y);
	scene1.onLeftButton(mouse);

	glutPostRedisplay();
}
void onLeftButton2(int x, int y) {
	
	mouse.update(x, y);
	scene1.onLeftButton2(mouse);

	glutPostRedisplay();
}
void onRightButton(int x, int y) {
	mouse.update(x, y);
	scene1.onRightButton(mouse);

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
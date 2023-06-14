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
MouseInput mouse{};

int windowWidth{ 500 };
int windowHeight{ 500 };

void myKeyboard(unsigned char, int, int);
void onLeftButton(int, int );
void onLeftButton2(int, int);
void onRightButton(int, int);
void myMouse(int, int, int, int);
void specialFunc(int, int, int);
void myDisplay();
void reshapeWindow(int, int);

int main(int argc, char** argv) 
{

	glutInit(&argc, argv);
	
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("OpenGL - The Reflective Teapot");

	// Setup callback functions
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutSpecialFunc (specialFunc);
	glutReshapeFunc(reshapeWindow);

	GLenum res = glewInit();
	// Check if glew initializes
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	CY_GL_REGISTER_DEBUG_CALLBACK;

	// Setup scene
	scene1.setup(windowHeight, windowWidth);

	glutDisplayFunc(myDisplay);
	glutMainLoop();

	return 0;
}

void myKeyboard(unsigned char key, int x, int y) 
{
	switch (key) {
	case 27:
		glutLeaveMainLoop(); // Exit the main loop when 'Esc' is pressed
		break;
	case 'r':
		std::cout << "recompile" << std::endl;
		scene1.recompileShaders(); // Recompile shaders when 'r' key is pressed
		break;
	case 'R':
		std::cout << "recompile" << std::endl; // Recompile shaders when 'R' key is pressed
		scene1.recompileShaders();
		break;
	}
	glutPostRedisplay();
}

// Callback function for mouse input
void myMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			mouse.update(x, y);
			glutMotionFunc(onLeftButton);
		}
		else if (button == GLUT_RIGHT_BUTTON) {
			mouse.update(x, y);
			glutMotionFunc(onRightButton);
		}
	}
}

// Callback function for right mouse button input
void onLeftButton(int x, int y) {

	mouse.update(x, y);
	scene1.onLeftButton(mouse);

	glutPostRedisplay();
}

// Callback function for right mouse button input
void onRightButton(int x, int y)
{
	mouse.update(x, y);
	scene1.onRightButton(mouse);

	glutPostRedisplay();
}

// Callback function for special key input
void specialFunc(int key, int x, int y) 
{
	if (key == GLUT_KEY_CTRL_L) { // If left control pressed
		mouse.update(x, y);
		glutMotionFunc(onLeftButton2);
	}
}

// This callback function is called after the left mouse button is pressed, followed by the left control key.
void onLeftButton2(int x, int y)
{

	mouse.update(x, y);
	scene1.onLeftButton2(mouse);

	glutPostRedisplay();
}

// Callback function for display
void myDisplay() 
{
	scene1.update();
	scene1.render();
}
// Callback function for window reshape
void reshapeWindow(int windowWidth, int windowHeight) 
{
	scene1.reshapeWindow(windowWidth, windowHeight);
	glutPostRedisplay();
}
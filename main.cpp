#include <iostream>
#include <vector>
#define GLEW_STATIC
#include "glew.h"
#include <GL/freeglut.h>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyGL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



float timeD;
GLuint VAO{};
GLuint vao{};
GLuint shaderProgram;
std::vector<cy::Vec3f> geometryMesh;

int preMouseX;
int preMouseY;

int windowWidth{ 400 };
int windowHeight{ 400 };

float angleX = 0.0f;
float angleY = 0.0f;

struct vertex {
	float x, y, z;
	float r, g, b;
};

std::vector<vertex> vertices{
		{-1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f }, // Vertex 1
		{ 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f }, // Vertex 2
		{ 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f }  // Vertex 3
};

glm::mat4 rotX = glm::mat4(1.0f);
glm::mat4 rotY = glm::mat4(1.0f);

glm::mat4 transformCamera;
glm::mat4 projection;
glm::mat4 view;

glm::mat4 model;
glm::mat4 mvp;
glm::mat4 transform;

glm::vec3 translation(0.0f, 0.0f, 0.0f);

std::vector<vertex> teapot{};
cy::GLSLProgram program;


void myIdle() {
	glutPostRedisplay();
}

void myDisplay() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	
	//myIdle();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			glutLeaveMainLoop();
			break;
		case 'a':

			break;
		
	}
	glutPostRedisplay();
}
void updateMouse(int x, int y) {
	preMouseX = x;
	preMouseY = y;

}

void onLeftButton(int x, int y) {
	angleY = (x - preMouseX)/40.0f;
	angleX = (y - preMouseY)/40.0f;

	rotX = glm::rotate(rotX, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	rotY = glm::rotate(rotY, angleY, glm::vec3(0.0f, 1.0f, 0.0f));

	view = glm::lookAt(
		glm::vec3(-1.0f, 0.0f, 0.5f) + translation,
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);


	mvp = projection * model * rotX * rotY * transform * view;

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glutPostRedisplay();

	updateMouse(x, y);
}

void onRightButton(int x, int y) {
	translation.z += (y - preMouseY) / 400.0f;


	view = glm::lookAt(
		glm::vec3(-1.0f, 0.0f, 0.5f) + translation,
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	mvp = projection * model * rotX * rotY * transform * view;

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glutPostRedisplay();

	updateMouse(x, y);
}

void myMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_RIGHT_BUTTON) {
			updateMouse(x, y);
			glutMotionFunc(onRightButton);
		}
		else if (button == GLUT_LEFT_BUTTON) {
			updateMouse(x, y);
			glutMotionFunc(onLeftButton);
		}
			
	}
	
	if (state == GLUT_DOWN || state == GLUT_UP)
	{
		//angleX = 0.1f;
		
		
	}
}
void motion(int x, int y) {
}

void myDisplayTeapot(){

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, (GLsizei)teapot.size());

	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);


	glutCreateWindow("Windows");
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(motion);

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	cy::TriMesh mesh;
	mesh.LoadFromFileObj("teapot.obj");
	//vector of vertices
	int n = mesh.NV();
	for (int i = 0; i < n; i++) {
		teapot.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 1, 1 ,1 });
		//std::cout << teapot[i].x <<" "<< teapot[i].y << " " << teapot[i].z << " " << teapot[i].r << " " << teapot[i].g << " " << teapot[i].b << "\n";
	}
	
	rotX = glm::rotate(rotX, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	rotY = glm::rotate(rotX, angleX, glm::vec3(0.0f, 1.0f, 0.0f));
	//transformCamera = rotX;

	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 4.0f);
	//projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

	view = glm::lookAt(
		glm::vec3(-1.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	model = glm::mat4(1.0f);
	mvp = projection * model * rotX * view;

	transform = glm::translate(glm::mat4(1.0f), translation);

	GLuint vbo{};
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, teapot.size() * sizeof(vertex), teapot.data(), 0);

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/fragment.frag", GL_FRAGMENT_SHADER);
	
	program.CreateProgram();
	

	program.AttachShader(fragmentS);
	program.AttachShader(vertexS);
	program.Link();


	shaderProgram = program.GetID();
	
	glCreateVertexArrays(1, &vao);

	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(vertex));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, x));
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, r));
	
	glVertexArrayBindingDivisor(vao, 0, 0);
	
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);

	glUseProgram(shaderProgram);

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformTransLoc = glGetUniformLocation(program.GetID(), "tranform");
	glUniformMatrix4fv(uniformTransLoc, 1, GL_FALSE, glm::value_ptr(transformCamera));

	if (uniformLoc != -1) {
		std::cout << "certo";
	}
	else {
		std::cout << "o uniform não foi encontrado no programa shader";
	}

	glutDisplayFunc(myDisplayTeapot);

	glutMainLoop();

	return 0;
}
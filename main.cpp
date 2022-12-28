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

int windowWidth{ 400 };
int windowHeight{ 400 };

struct vertex {
	float x, y, z;
	float r, g, b;
};

std::vector<vertex> vertices{
		{-1.0f, -1.0f, 1.0f,  1.0f, 0.0f, 0.0f }, // Vertex 1
		{ 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f }, // Vertex 2
		{ 1.0f, -1.0f, 1.0f,  0.0f, 0.0f, 1.0f }  // Vertex 3
};


void myIdle() {
	glutPostRedisplay();
}

void myDisplay() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
	
	//myIdle();
}

void myDisplayTeapot(){

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

	glutSwapBuffers();
}

int main(int argc, char** argv) {

	cy::TriMesh mesh;
	mesh.LoadFromFileObj("teapot.obj");

	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);


	glutCreateWindow("Windows");

	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}


	//vector of vertices
	
	int n = mesh.NV();

	for (int i = 0; i < n; i++) {
		geometryMesh.push_back(mesh.V(i));
	}
	// Criar a matriz de projeção ortográfica
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	// Criar a matriz de visualização
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	// Criar a matriz de modelo
	glm::mat4 model = glm::mat4(1.0f);

	// Criar a matriz MVP
	//glm::mat4x4 mvp = projection * view * model;

	glm::mat4 mvp = projection * model * view;

	GLuint vbo{};
	glCreateBuffers(1, &vbo);


	glNamedBufferStorage(vbo, vertices.size() * sizeof(vertex), vertices.data(), 0);

	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);


	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/fragment.frag", GL_FRAGMENT_SHADER);
	
	cy::GLSLProgram program;
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

	//float test = 10.0f;

	//GLint uniformLoc = glGetUniformLocation(program.GetID(), "test");
	//glUniform1f(uniformLoc, test);
	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

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
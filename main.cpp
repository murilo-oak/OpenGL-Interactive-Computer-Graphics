#include <iostream>
#include <vector>
#include <algorithm>
#define GLEW_STATIC
#include "glew.h"
#include <GL/freeglut.h>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyGL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

float timeD;
GLuint vao{};
GLuint vbo{};
GLuint vboNormals{};
GLuint ebo{};
GLuint shaderProgram;

std::vector<cy::Vec3f> geometryMesh;
std::vector<unsigned int> facesIndex;

int preMouseX;
int preMouseY;

int windowWidth{500};
int windowHeight{500};

float angleX = 0.0f;
float angleY = 0.0f;

struct vertex {
	float x, y, z;
	float r, g, b;
};

struct normal {
	float x, y, z;
};

struct vertexIndices{
	int vertex;
	int normal;

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

glm::mat3 mv;

glm::vec3 translation(0.0f, 0.0f, 0.0f);

std::vector<vertex> teapot{};
std::vector<normal> normals{};
cy::GLSLProgram program;


void myIdle() {
	glutPostRedisplay();
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
	mv = model * rotX * rotY * transform * view;

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glutPostRedisplay();

	updateMouse(x, y);
}
void onRightButton(int x, int y) {
	translation.y += (y - preMouseY) / 400.0f;


	view = glm::lookAt(
		glm::vec3(-1.0f, 0.0f, 0.5f) + translation,
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	mvp = projection * model * rotX * rotY * transform * view;
	mv = model * rotX * rotY * transform * view;

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
}
void motion(int x, int y) {
}

void myDisplayTeapot(){
	glDepthRange(0.0, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glClearDepth(0.5);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, windowWidth, windowHeight);
	
	//glDrawElements(GL_TRIANGLES, facesIndex.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, (GLsizei)teapot.size());
	glDrawElements(GL_TRIANGLES, facesIndex.size(), GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
}

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	// Imprima a mensagem de depuração
	std::cout << "Debug message: " << message << std::endl;
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("Windows");

	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouse);
	glutMotionFunc(motion);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	CY_GL_REGISTER_DEBUG_CALLBACK;

	cy::TriMesh mesh;
	mesh.LoadFromFileObj("teapot.obj");
	//std::cout << mesh.NF() << "\n";

	//vector of vertices
	int n = mesh.NV();
	for (int i = 0; i < n; i++) {
		teapot.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 0.5f, 0.5f , 0.5f});
		//std::cout << teapot[i].x <<" "<< teapot[i].y << " " << teapot[i].z << " " << teapot[i].r << " " << teapot[i].g << " " << teapot[i].b << "\n";
	}

	int nf = mesh.NF();
	int counter{};
	std::vector<vertexIndices> list{};
	std::vector<bool> checkVertex(n, false);
	
	for (int i = 0; i < nf-1; i++) {
		for (int k = 0; k < 3; k++) {
			
			if (mesh.F(i).v[k]>= n || checkVertex[mesh.F(i).v[k]]) {
				continue;
			}

			for (int j = i; j < nf; j++) {
				if (mesh.F(i).v[k] == mesh.F(j).v[k] && mesh.FN(i).v[k] != mesh.FN(j).v[k]) { //works!
					
					if (list.empty()) {
						int vertexI = mesh.F(i).v[k];
						int normalI = mesh.FN(i).v[k];
						vertexIndices vertex = { vertexI, normalI };
						list.push_back(vertex);
					}
					
					int vertexI = mesh.F(j).v[k];
					int normalI = mesh.FN(j).v[k];
					
					vertexIndices vertex = { vertexI, normalI };
					auto it = std::find_if(list.begin(), list.end(), [&vertex](const vertexIndices& e) { 
						return e.vertex == vertex.vertex && e.normal == vertex.normal; 
					});
					
					bool notFoundVertex = it == list.end();
					
					if (notFoundVertex) {
						list.push_back(vertex);
					}
				}
			}
			
		}
		//se achou vértice com normal distinta
		if (!list.empty()) {
			
			//atualiza o valor no vetor de vértice que ele ja foi checado
			checkVertex[list[0].vertex] = true;
			
			//adiciona ao vbo de vértices no final o vértices que precisa duplicar
			
			std::for_each(list.begin() + 1, list.end(), [&mesh, &nf](vertexIndices x) {
				
				int tSize = teapot.size();
				//adiciona vertice
				teapot.push_back({ mesh.V(x.vertex).x, mesh.V(x.vertex).y, mesh.V(x.vertex).z, 0.5f, 0.5f, 0.5f });
				
				//atualiza o indices dos vértices duplicados
				for (int f = 0; f < nf; f++) {
					for (int k = 0; k < 3; k++) {
						if (x.vertex == mesh.F(f).v[k] && x.normal == mesh.FN(f).v[k]) {
							mesh.F(f).v[k] = tSize;
						}
					}
				}
			});

			list.clear();
		}
		
	}

	std::cout << teapot.size() << "  <-\n";
	std::cout << mesh.NVN() << "  <-\n";

	for (int i = 0; i < nf; i++) {
		facesIndex.push_back({ mesh.F(i).v[0] });
		facesIndex.push_back({ mesh.F(i).v[1] });
		facesIndex.push_back({ mesh.F(i).v[2] });
	}

	int nNormals = mesh.NVN();
	int normalIndex{};

	normal zero = { 0.5f,0.5f,0.5f };
	for (int f = 0; f < nf; f++) {
		for (int k = 0; k < 3; k++) {
			if (mesh.F(f).v[k] == normalIndex) {
				normalIndex++;
				normal nwqe = { mesh.VN( mesh.FN(f).v[k]).x, mesh.VN(mesh.FN(f).v[k]).y, mesh.VN(mesh.FN(f).v[k]).z };
				normal zero = { 1.0f,0.0f,0.0f};
				normals.push_back(nwqe);
			}
		}
	}

	std::cout << normals.size() << "\n";
	std::cout << teapot.size() << "\n";

	rotX = glm::rotate(rotX, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	rotY = glm::rotate(rotX, angleX, glm::vec3(0.0f, 1.0f, 0.0f));

	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 10.0f);
	//projection = glm::perspective(45.0f, 1.0f, 0.0f, 100.0f);

	view = glm::lookAt(
		glm::vec3(-1.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	model = glm::mat4(1.0f);
	mvp = projection * model * view;

	mv = (model * view);
	mv = glm::inverse(mv);
	mv = glm::transpose(mv);
	
	transform = glm::translate(glm::mat4(1.0f), translation);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//positions
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glNamedBufferStorage(vbo, teapot.size() * sizeof(vertex), teapot.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, r));
	
	glEnableVertexAttribArray(0);//antes de renderizar
	glEnableVertexAttribArray(1);//antes de renderizar

	//normals
	glCreateBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glNamedBufferStorage(vboNormals, normals.size() * sizeof(normal), normals.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(normal), 0);
	
	glEnableVertexAttribArray(2);//antes de renderizar

	//indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesIndex.size() * sizeof(unsigned int), facesIndex.data(), GL_STATIC_DRAW);

	//vertex and fragment shader compilation
	cy::GLSLShader vertexS;
	vertexS.CompileFile("Shaders/vertex.vert", GL_VERTEX_SHADER);

	cy::GLSLShader fragmentS;
	fragmentS.CompileFile("Shaders/Blinn_shading.frag", GL_FRAGMENT_SHADER);
	
	program.CreateProgram();

	program.AttachShader(fragmentS);
	program.AttachShader(vertexS);
	program.Link();

	shaderProgram = program.GetID();

	glUseProgram(shaderProgram);

	//Uniform variable initialization
	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

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
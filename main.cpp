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
#include "lodepng/lodepng.h"

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
glm::mat4 mv4;

glm::vec3 translation(0.0f, 0.0f, 0.0f);
glm::vec3 lightDir(0.0f, 1.0f, 0.0f);

std::vector<vertex> teapot{};
std::vector<normal> normals{};
std::vector<glm::vec2> texCoords{};

cy::GLSLProgram program;


void myIdle() {
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
			glutLeaveMainLoop();
			break;
		case GLUT_ACTIVE_CTRL:
			std::cout << "aaa\n";
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
		glm::vec3(-2.0f, 0.0f, 0.5f) + translation,
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);


	mvp = projection * model * rotX * rotY * transform * view;
	mv4 = model * rotX * rotY * transform * view;

	mv = mv4;
	mv = glm::inverse(mv);
	mv = glm::transpose(mv);

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));


	GLint uniformLocmv4 = glGetUniformLocation(program.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));
	glutPostRedisplay();

	updateMouse(x, y);
}

void onLeftButton2(int x, int y) {
	angleY = (x - preMouseX) / 40.0f;
	angleX = (y - preMouseY) / 40.0f;

	glm::vec3 vector = glm::vec3(lightDir);
	glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec4 rotatedVector = rotationMatrixX * rotationMatrixY * glm::vec4(vector, 1.0f);

	lightDir = rotatedVector;

	GLint uniformLightDir = glGetUniformLocation(program.GetID(), "lightDir");
	glUniform3fv(uniformLightDir, 1, &lightDir[0]);

	glutPostRedisplay();

	updateMouse(x, y);
}
void onRightButton(int x, int y) {
	translation.y += (y - preMouseY) / 400.0f;


	view = glm::lookAt(
		glm::vec3(-2.0f, 0.0f, 0.5f) + translation,
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	mvp = projection * model * rotX * rotY * transform * view;
	mv4 = model * rotX * rotY * transform * view;

	mv = mv4;
	mv = glm::inverse(mv);
	mv = glm::transpose(mv);

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	
	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

	GLint uniformLocmv4 = glGetUniformLocation(program.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));


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
	glDepthRange(0.0, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(0.5);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, windowWidth, windowHeight);

	glDrawElements(GL_TRIANGLES, facesIndex.size(), GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
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
	glutSpecialFunc (specialFunc);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	CY_GL_REGISTER_DEBUG_CALLBACK;

	cy::TriMesh mesh;
	mesh.LoadFromFileObj("teapot.obj");
	//std::cout << mesh.GetTexCoord(0, cy::Vec3f(0,1,0)).elem[0] <<" | " << mesh.GetTexCoord(0, cy::Vec3f(0, 1, 0)).elem[1] << " <- M \n";
	
	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;
	lodepng::State state; //optionally customize this one

	unsigned error = lodepng::load_file(png, "brick.png"); //load the image file with given filename
	if (!error) error = lodepng::decode(image, width, height, state, png);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//vector of vertices
	int n = mesh.NV();
	for (int i = 0; i < n; i++) {
		teapot.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 1.0f, 0.0f , 0.3f});
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
		//se achou v�rtice com normal distinta
		if (!list.empty()) {
			
			//atualiza o valor no vetor de v�rtice que ele ja foi checado
			checkVertex[list[0].vertex] = true;
			
			//adiciona ao vbo de v�rtices no final o v�rtices que precisa duplicar
			
			std::for_each(list.begin() + 1, list.end(), [&mesh, &nf](vertexIndices x) {
				
				int tSize = teapot.size();
				//adiciona vertice
				teapot.push_back({ mesh.V(x.vertex).x, mesh.V(x.vertex).y, mesh.V(x.vertex).z, 0.5f, 0.5f, 0.5f });
				
				//atualiza os indices dos v�rtices duplicados
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
				//glm::vec2 texCoord = { mesh.VN(mesh.FN(f).v[k]).x, mesh.VN(mesh.FN(f).v[k]).x};
				normal zero = { 1.0f,0.0f,0.0f};
				normals.push_back(nwqe);
			}
		}
	}

	rotX = glm::rotate(rotX, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	rotY = glm::rotate(rotX, angleX, glm::vec3(0.0f, 1.0f, 0.0f));

	projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -5.0f, 100.0f);
	//projection = glm::perspective(45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 1.0f, 1500.0f);

	view = glm::lookAt(
		glm::vec3(-2.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 0.5f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);

	model = glm::mat4(1.0f);
	mvp = projection * model * view;
	mvp = projection * model * rotX * rotY * view;

	mv4 = (model * view);
	mv = mv4;
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

	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

	GLint uniformLocmv4 = glGetUniformLocation(program.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));

	GLint uniformTransLoc = glGetUniformLocation(program.GetID(), "tranform");
	glUniformMatrix4fv(uniformTransLoc, 1, GL_FALSE, glm::value_ptr(transformCamera));

	GLint uniformLightDir = glGetUniformLocation(program.GetID(), "lightDir");
	glUniform3fv(uniformLightDir, 1, &lightDir[0]);


	if (uniformLoc != -1) {
		std::cout << "certo";
	}
	else {
		std::cout << "o uniform n�o foi encontrado no programa shader";
	}

	glutDisplayFunc(myDisplayTeapot);
	glutMainLoop();

	return 0;
}
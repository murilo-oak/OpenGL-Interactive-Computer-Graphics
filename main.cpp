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
#include "cubemap/Cubemap.h"

float timeD;
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

int texWidth{ 500 }, texHeight{ 500};

float angleX = 0.90f;
float angleY = 0.90f;

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
	int texCoord;
};

std::vector<vertex> vertices{
	// positions          // colors           
	{13.5f,  13.5f, 0.0f,   1.0f, 0.0f, 0.0f},   // top right
	{13.5f, -13.5f, 0.0f,   0.0f, 1.0f, 0.0f },   // bottom right
	{-13.5f, -13.5f, 0.0f,   0.0f, 0.0f, 1.0f},   // bottom left
	{-13.5f,  13.5f, 0.0f,   1.0f, 1.0f, 0.0f}   // top left 
};

std::vector<normal> normalsPlane{
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

glm::mat4 transformCamera;
glm::mat4 projection;
glm::mat4 view;

glm::mat4 viewCube;


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
std::vector<glm::vec2> texCoords2{};

cy::GLSLProgram program;
cy::GLSLProgram skyboxProgram;

glm::vec3 tr = glm::vec3(3.0f * sin(angleY) * sin(angleX), 0.5f + 3.0f * cos(angleY), 3.0f * sin(angleY) * cos(angleX));
float r = 3;

Cubemap cube;

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
	//angleY += (x - preMouseX)/40.0f;

	r += ((y - preMouseY) / 40.0f);
	tr = glm::vec3(r * sin(angleY) * sin(angleX), 0.5f + r * cos(angleY), r * sin(angleY) * cos(angleX));
	view = glm::lookAt(
		tr,
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);


	mvp = projection * model * rotX * rotY * transform * view;
	mv4 = model * rotX * rotY * transform * view;

	mv = mv4;
	mv = glm::inverse(mv);
	mv = glm::transpose(mv);

	glUseProgram(program.GetID());

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

	GLint uniformLocmv4 = glGetUniformLocation(program.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));

	glUseProgram(skyboxProgram.GetID());

	glm::mat4 viewCube = glm::mat4(glm::mat3(view));
	mvp = projection * viewCube;

	uniformLoc = glGetUniformLocation(skyboxProgram.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	uniformLocmv = glGetUniformLocation(skyboxProgram.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

	uniformLocmv4 = glGetUniformLocation(skyboxProgram.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));
	glutPostRedisplay();

	updateMouse(x, y);
}

void onLeftButton2(int x, int y) {
	angleY = (x - preMouseX) / 40.0f;
	angleX = (y - preMouseY) / 40.0f;

	translation = +glm::vec4(angleX, 0, 0, 0);
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
	angleY += (y - preMouseY) / 400.0f;
	angleX += (x - preMouseX) / 400.0f;
	
	tr = glm::vec3(r * sin(angleY) * sin(angleX), 0.5f + r * cos(angleY), r * sin(angleY) * cos(angleX));

	view = glm::lookAt(
		tr,
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	
	mvp = projection * model * rotX * rotY * view;
	mv4 = model * rotX * rotY * transform * view;

	mv = mv4;
	mv = glm::inverse(mv);
	mv = glm::transpose(mv);


	glUseProgram(program.GetID());

	GLint uniformLoc = glGetUniformLocation(program.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformLocmv = glGetUniformLocation(program.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmv, 1, GL_FALSE, glm::value_ptr(mv));

	GLint uniformLocmv4 = glGetUniformLocation(program.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4, 1, GL_FALSE, glm::value_ptr(mv4));

	glUseProgram(skyboxProgram.GetID());

	glm::mat4 viewCube = glm::mat4(glm::mat3(view));
	mvp = projection * viewCube;

	uniformLoc = glGetUniformLocation(skyboxProgram.GetID(), "mvp");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mvp));


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
	//glDepthRange(0.0, 1);
	


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, texWidth, texHeight);
	glUseProgram(skyboxProgram.GetID());
	
	glDepthMask(GL_FALSE);
	glBindVertexArray(vaoCube);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	
	//glClearDepth(0.5);
	glEnable(GL_DEPTH_TEST);


	glUseProgram(program.GetID());


	glBindVertexArray(vao);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, texWidth, texHeight);
	//glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	
	glDrawElements(GL_TRIANGLES, facesIndex.size(), GL_UNSIGNED_INT, 0);

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


	//glDrawArrays(GL_POINTS, 0, teapot.size());
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutCreateWindow("OpenGL - Maxwell, the Cat");

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
	cube.loadCubeMap();
	//cubeMap.loadCubeMap();

	cy::TriMesh mesh;
	mesh.LoadFromFileObj("teapot.obj");
	//std::cout << mesh.GetTexCoord(0, cy::Vec3f(0,1,0)).elem[0] <<" | " << mesh.GetTexCoord(0, cy::Vec3f(0, 1, 0)).elem[1] << " <- M \n";

	std::vector<unsigned char> png;
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;
	lodepng::State state; //optionally customize this one

	std::cout<< "Materials: " << mesh.NM() << "\n";
	std::cout << "MAP: " << mesh.M(0).map_Kd << "\n";

	unsigned error = lodepng::load_file(png, std::string(mesh.M(0).map_Kd).c_str()); //load the image file with given filename
	if (!error) error = lodepng::decode(image, width, height, state, png);

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	//vector of vertices
	int n = mesh.NV();
	for (int i = 0; i < n; i++) {
		teapot.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 1.0f, 0.0f , 0.3f});
		//std::cout << mesh.VT(mesh.FT(i).v[0]).x << ", " << mesh.VT(mesh.FT(i).v[0]).y << " \n";
	}

	int nf = mesh.NF();
	int counter{};
	std::vector<vertexIndices> list{};
	std::vector<bool> checkVertex(n, false);
	
	for (int i = 0; i < nf; i++) {
		for (int k = 0; k < 3; k++) {
			
			if (mesh.F(i).v[k]>= n || checkVertex[mesh.F(i).v[k]]) {
				continue;
			}
	
			for (int j = i; j < nf; j++) {
				if (mesh.F(i).v[k] == mesh.F(j).v[k] && (mesh.FN(i).v[k] != mesh.FN(j).v[k])) { //works!
					
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
				
				//atualiza os indices dos vértices duplicados
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

	int nNormals = mesh.NVN();
	int normalIndex{};

	for (int i = 0; i < nf; i++) {
		for (int k = 0; k < 3; k++) {

			if (mesh.F(i).v[k] >= n || checkVertex[mesh.F(i).v[k]]) {
				continue;
			}

			for (int j = i; j < nf; j++) {
				if (mesh.F(i).v[k] == mesh.F(j).v[k] && mesh.FT(i).v[k] != mesh.FT(j).v[k]) { //works!
					if (list.empty()) {
						int vertexI = mesh.F(i).v[k];
						int normalI = mesh.FN(i).v[k];
						int texI = mesh.FT(i).v[k];
						vertexIndices vertex = { vertexI, normalI, texI };
						list.push_back(vertex);
					}

					int vertexI = mesh.F(j).v[k];
					int normalI = mesh.FN(j).v[k];
					int texI = mesh.FT(i).v[k];
					
					vertexIndices vertex = { vertexI, normalI, texI };
					auto it = std::find_if(list.begin(), list.end(), [&vertex](const vertexIndices& e) {
						return e.vertex == vertex.vertex && e.normal == vertex.normal && e.texCoord == vertex.texCoord;
						});

					bool notFoundVertex = it == list.end();

					if (notFoundVertex) {
						list.push_back(vertex);
					}
				}
			}

		}
		//se achou vértice com normal distinta
		if (list.size() > 0) {

			//atualiza o valor no vetor de vértice que ele ja foi checado
			checkVertex[list[0].vertex] = true;

			//adiciona ao vbo de vértices no final o vértices que precisa duplicar
			std::for_each(list.begin() + 1, list.end(), [&mesh, &nf](vertexIndices x) {
				
				int tSize = teapot.size();
				//adiciona vertice
				teapot.push_back({ mesh.V(x.vertex).x, mesh.V(x.vertex).y, mesh.V(x.vertex).z, 0.5f, 0.5f, 0.5f });
				//normals.push_back({ mesh.VN(x.normal).x, mesh.VN(x.normal).y, mesh.VN(x.normal).z});


				for (int f = 0; f < nf; f++) {
					for (int k = 0; k < 3; k++) {
						//mesh.VN(mesh.FN(f).v[k]).x
						if (x.vertex == mesh.F(f).v[k] && x.normal == mesh.FN(f).v[k] && x.texCoord == mesh.FT(f).v[k]) {
							mesh.F(f).v[k] = tSize;
							mesh.FN(f).v[k] = x.normal;
						}
					}
				}
				});
		}

		list.clear();
	}
			
	int nTex = teapot.size();
	int texIndex{};

	for (int f = 0; f < mesh.NF(); f++) {
		for (int k = 0; k < 3; k++) {
			//std::cout << mesh.F(f).v[k] << " | " << mesh.FN(f).v[k] << " | " << mesh.GetMaterialFirstFace(f) << " \n";
			for (int j = 0; j < nf; j++) {
				if (mesh.F(j).v[k] == texIndex) {
					texIndex++;
					texCoords.push_back({ mesh.VT(mesh.FT(j).v[k]).x, mesh.VT(mesh.FT(j).v[k]).y });
					normals.push_back({ mesh.VN(mesh.FN(j).v[k]).x, mesh.VN(mesh.FN(j).v[k]).y, mesh.VN(mesh.FN(j).v[k]).z });
					//continue;
				}
			}
		}
	}

	for (int i = 0; i < nf; i++) {
		facesIndex.push_back({ mesh.F(i).v[0] });
		facesIndex.push_back({ mesh.F(i).v[1] });
		facesIndex.push_back({ mesh.F(i).v[2] });
	}

	std::cout << teapot.size() << "  <- Vertices" << std::endl;
	std::cout << normals.size() << "  <- Normals" << std::endl;
	std::cout << texCoords.size() << "  <- Tex" << std::endl;
	std::cout << mesh.NF() << "  <- Faces" << std::endl;
	std::cout << mesh.NV() << "  <- Original Vertex number" << std::endl;
	std::cout << mesh.NVN() << "  <- Original Normal number" << std::endl;
	std::cout << mesh.NVT() << "  <- Original Tex number" << std::endl;

	//rotX = glm::rotate(rotX, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
	//rotY = glm::rotate(rotX, angleX, glm::vec3(0.0f, 1.0f, 0.0f));

	//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 200000.0f);
	projection = glm::perspective(glm::radians(55.0f), (GLfloat)windowHeight/ (GLfloat)windowWidth, 0.5f, 100.0f);
	//tr = glm::vec3(r * sin(angleY) * sin(angleX), 0.5f + r * cos(angleY), r * sin(angleY) * cos(angleX));
	
	view = glm::lookAt(
		tr,
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	model = glm::mat4(1.0f);
	//mvp = projection * model * rotX * rotY * view;
	//mvp = projection * model * view;
	mvp = projection * model * view;


	mv4 = model * view;
	mv = glm::mat4(glm::mat3(mv4));
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

	//texcoord
	glCreateBuffers(1, &vboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glNamedBufferStorage(vboTexCoords, texCoords.size() * sizeof(glm::vec2), texCoords.data(), 0);
	glVertexAttribPointer(3, 2, GL_FLOAT,GL_FALSE, sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(3);

	glGenVertexArrays(1, &vaoPlane);
	glBindVertexArray(vaoPlane);

	//indices
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesIndex.size() * sizeof(unsigned int), facesIndex.data(), GL_STATIC_DRAW);

	//positions
	glCreateBuffers(1, &vboPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboPlane);
	glNamedBufferStorage(vboPlane, 4 * sizeof(vertex), vertices.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, r));

	glEnableVertexAttribArray(0);//antes de renderizar
	glEnableVertexAttribArray(1);//antes de renderizar

	//normals
	glCreateBuffers(1, &vboNormalsPlane);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormalsPlane);
	glNamedBufferStorage(vboNormalsPlane, normalsPlane.size() * sizeof(normal), normalsPlane.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(normal), 0);

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


	glEnableVertexAttribArray(3);

	//texture
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	glGenerateMipmap(GL_TEXTURE_2D);
	
	//filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Tiling
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//vertex and fragment shader compilation

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
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0}; 

	//if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER != GL_FRAMEBUFFER_COMPLETE)) {
	//	return false;
	//}


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
	
	GLint sampler = glGetUniformLocation(program.GetID(), "tex");
	glUniform1i(sampler, 0);

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


	//Cubemap
	glGenVertexArrays(1, &vaoCube);
	glBindVertexArray(vaoCube);

	//positions
	glCreateBuffers(1, &vboCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glNamedBufferStorage(vboCube,  sizeof(skyboxVertices), skyboxVertices, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

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


	vertexS.CompileFile("Shaders/vertexcube.vert", GL_VERTEX_SHADER);
	fragmentS.CompileFile("Shaders/fragmentcube.frag", GL_FRAGMENT_SHADER);


	skyboxProgram.CreateProgram();

	skyboxProgram.AttachShader(fragmentS);
	skyboxProgram.AttachShader(vertexS);
	skyboxProgram.Link();


	glUseProgram(skyboxProgram.GetID());

	sampler = glGetUniformLocation(skyboxProgram.GetID(), "skybox");
	glUniform1i(sampler, 0);


	glm::mat4 viewCube = glm::mat4(glm::mat3(view));
	mv = viewCube;
	mvp = projection * viewCube;

	//Uniform variable initialization
	GLint uniformLocS = glGetUniformLocation(skyboxProgram.GetID(), "mvp");
	glUniformMatrix4fv(uniformLocS, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint uniformLocmvS = glGetUniformLocation(skyboxProgram.GetID(), "mv3");
	glUniformMatrix3fv(uniformLocmvS, 1, GL_FALSE, glm::value_ptr(mv));

	GLint uniformLocmv4S = glGetUniformLocation(skyboxProgram.GetID(), "mv4");
	glUniformMatrix4fv(uniformLocmv4S, 1, GL_FALSE, glm::value_ptr(mv4));

	GLint uniformTransLocS = glGetUniformLocation(skyboxProgram.GetID(), "tranform");
	glUniformMatrix4fv(uniformTransLocS, 1, GL_FALSE, glm::value_ptr(transformCamera));

	GLint uniformLightDirS = glGetUniformLocation(skyboxProgram.GetID(), "lightDir");
	glUniform3fv(uniformLightDirS, 1, &lightDir[0]);


	if (uniformLocS != -1) {
		std::cout << "certo";
	}
	else {
		std::cout << "o uniform não foi encontrado no programa shader";
	}

	glutDisplayFunc(myDisplayTeapot);
	glutMainLoop();

	return 0;
}
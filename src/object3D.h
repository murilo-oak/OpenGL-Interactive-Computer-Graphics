#pragma once

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <list>

#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include "../ThirdParty/include/lodepng/lodepng.h"
#include "../ThirdParty/include/cyCodeBase/cyTriMesh.h"
#include "../ThirdParty/include/glm/glm.hpp"

class Object3D
{
public:
	struct vertex {
		float x, y, z;
		float r, g, b;
	};

	struct vertexIndices {
		int vertex;
		int normal;
		int texCoord;
	};

	std::vector<vertex>       m_vertices{};
	std::vector<glm::vec3>    m_normals{};
	std::vector<glm::vec2>    m_texCoords{};
	std::vector<unsigned int> m_facesIndex{};

	GLuint m_texID{};
	unsigned int m_texWidth{500}, m_texHeight{500};

	lodepng::State m_state{};
	std::vector<unsigned char> m_png{};
	std::vector<unsigned char> m_image{};

	GLuint m_vao{};

	GLuint m_vbo{};
	GLuint m_vboNormals{};
	GLuint m_vboTexCoords{};
	GLuint m_ebo{};

	GLuint m_frameBuffer{};
	GLuint m_renderedTexture{};

	Object3D(const char* filename);
	Object3D();
	~Object3D();

	void set(const char* filename);
	void setTexture(unsigned texSizeWidth, unsigned texSizeHeight);

private:
	void setVboPositions();
	void setVboNormals();
	void setVboTexCoords();
	void setEbo(GLenum drawMode);
	void loadFromFile(const char* filename);
	void generateBuffers(cy::TriMesh mesh);
};
#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
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

	void set(const char* filename) {
		loadFromFile(filename);
		
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		setVboPositions();
		setVboNormals();
		setVboTexCoords();
		setEbo(GL_STATIC_DRAW);
	}
	void setTexture(unsigned texSizeWidth, unsigned texSizeHeight) {
		m_texWidth = texSizeWidth;
		m_texHeight = texSizeHeight;

		glGenTextures(1, &m_texID);
		//texture
		glBindTexture(GL_TEXTURE_2D, m_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeWidth, texSizeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.data());

		glGenerateMipmap(GL_TEXTURE_2D);

		//filter
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Tiling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	}

private:
	void setVboPositions() 
	{
		glCreateBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glNamedBufferStorage(m_vbo, m_vertices.size() * sizeof(vertex), m_vertices.data(), 0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, x));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, r));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	void setVboNormals()
	{
		glCreateBuffers(1, &m_vboNormals);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
		glNamedBufferStorage(m_vboNormals, m_normals.size() * sizeof(glm::vec3), m_normals.data(), 0);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

		glEnableVertexAttribArray(2);
	}
	void setVboTexCoords() 
	{
		//texcoord
		glCreateBuffers(1, &m_vboTexCoords);
		glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
		glNamedBufferStorage(m_vboTexCoords, m_texCoords.size() * sizeof(glm::vec2), m_texCoords.data(), 0);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

		glEnableVertexAttribArray(3);
	}
	void setEbo(GLenum drawMode)
	{
		glGenBuffers(1, &m_ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_facesIndex.size() * sizeof(unsigned int), m_facesIndex.data(), drawMode);
	}
	void loadFromFile(const char* filename) {
		cy::TriMesh mesh;
		mesh.LoadFromFileObj(filename);

		std::cout << "Materials: " << mesh.NM() << std::endl;

		if (mesh.NM() > 0) {
			std::cout << "MAP: " << mesh.M(0).map_Kd << std::endl;

			unsigned error = lodepng::load_file(m_png, std::string(mesh.M(0).map_Kd).c_str());

			if (!error) error = lodepng::decode(m_image, m_texWidth, m_texHeight, m_state, m_png);
			if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		}

		//generateBuffers(mesh);
		generateBuffers(mesh);
	}
	
	struct IntPairHash {
		std::size_t operator()(const std::pair<uint32_t, uint32_t>& p) const {
			assert(sizeof(std::size_t) >= 8);  //Ensure that std::size_t, the type of the hash, is large enough
			//Shift first integer over to make room for the second integer. The two are
			//then packed side by side.
			return (((uint64_t)p.first) << 32) | ((uint64_t)p.second);
		}
	};

	struct somethub {
		int normalIndex;
		int texCoordIndex;
	};

	void generateBuffers(cy::TriMesh mesh) {
		std::unordered_multimap<int, std::tuple<int, int, int>> hashmapVertexIndices;
		std::unordered_map<int, std::tuple<int, int, int>> hashmapUniqueVertexIndices;
		std::unordered_map<int, int> hashmapFacesIndices;

		//vector of vertices
		int n_vertices = mesh.NV();

		int nf = mesh.NF();
		int indexFace = 0;
		for (int i = 0; i < nf; i++) {
			for (int k = 0; k < 3; k++) {
				int vertexIndex = mesh.F(i).v[k];
				int normalIndex = mesh.FN(i).v[k];
				int textureIndex = mesh.FT(i).v[k];

				hashmapVertexIndices.insert({ vertexIndex, std::make_tuple(normalIndex , textureIndex, indexFace) });
				indexFace++;
			}
		}
		int newindexDuplicatedVertex = mesh.NV();
		for (int keyVertexIndice = 0; keyVertexIndice < n_vertices; ++keyVertexIndice) {
			auto range = hashmapVertexIndices.equal_range(keyVertexIndice);
			std::unordered_map<std::pair<uint32_t, uint32_t>, std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>*, IntPairHash> uniqueVertices;
			
			for (auto it = range.first; it != range.second; ++it) {
				std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>* list;
				auto inserted = uniqueVertices.emplace(std::make_pair(std::get<0>(it->second), (std::get<1>(it->second))), list);
				
				if (std::get<1>(inserted)) {
					std::get<0>(inserted)->second = new std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>();
				}

				std::get<0>(inserted)->second->push_back(it);

			}
			
			if (uniqueVertices.size() > 0) {
				/*(Vunique, N, T)
				hashmapUniqueVertexIndices
				
				(I, Vunique)
				hashmapFacesIndices
				*/

				auto itUniqueVertices = uniqueVertices.begin();

				for (auto listIt = itUniqueVertices->second->begin(); listIt != itUniqueVertices->second->end(); ++listIt) {
					std::unordered_map<int, std::tuple<int, int, int>>::iterator elemento = *listIt;


					hashmapUniqueVertexIndices.emplace(elemento->first, std::make_tuple(std::get<0>(elemento->second), std::get<1>(elemento->second), elemento->first));
					hashmapFacesIndices.emplace(std::get<2>(elemento->second), elemento->first);
				}

				itUniqueVertices++;
				for (; itUniqueVertices != uniqueVertices.end(); ++itUniqueVertices) {
					//std::cout << std::get<0>(itUniqueVertices->first) << " | " << std::get<1>(itUniqueVertices->first) << " | " << itUniqueVertices->second->size() << std::endl;
					
					auto listVertexBegin = itUniqueVertices->second->begin();

					for (auto listIt = listVertexBegin; listIt != itUniqueVertices->second->end(); ++listIt) {
						std::unordered_map<int, std::tuple<int, int, int>>::iterator elemento = *listIt;
						
						int duplicatedVertexIndex = newindexDuplicatedVertex;
						
						hashmapUniqueVertexIndices.emplace(duplicatedVertexIndex, std::make_tuple(std::get<0>(elemento->second), std::get<1>(elemento->second), elemento->first));
						hashmapFacesIndices.emplace(std::get<2>(elemento->second), duplicatedVertexIndex);
						
						newindexDuplicatedVertex++;
					}
				}
			}

		}
		
		std::cout << "VERTICES: " << hashmapUniqueVertexIndices.size() << std::endl;
		std::cout << "Faces Indices: " << hashmapFacesIndices.size() << std::endl;
		std::cout << "Faces Before Indices: " << hashmapVertexIndices.size() << std::endl;
		int counter{};
		std::vector<vertexIndices> list{};
		std::vector<bool> checkVertex(n_vertices, false);

		for (int i = 0; i < hashmapUniqueVertexIndices.size(); i++) {
			int v = std::get<2>(hashmapUniqueVertexIndices.find(i)->second);
			int n = std::get<0>(hashmapUniqueVertexIndices.find(i)->second);
			int t = std::get<1>(hashmapUniqueVertexIndices.find(i)->second);
			
			m_vertices.push_back({ mesh.V(v).x, mesh.V(v).y, mesh.V(v).z, 1.0f, 0.0f , 0.3f });
			m_normals.push_back({  mesh.VN(n).x, mesh.VN(n).y, mesh.VN(n).z  });
			m_texCoords.push_back({ mesh.VT(t).x, mesh.VT(t).y });
		}

		for (int i = 0; i < hashmapFacesIndices.size(); i++) {
			m_facesIndex.push_back(hashmapFacesIndices.find(i)->second);
		}

		std::cout << m_vertices.size() << "  <- Vertices" << std::endl;
		std::cout << m_normals.size() << "  <- Normals" << std::endl;
		std::cout << m_texCoords.size() << "  <- Tex" << std::endl;

		std::cout << mesh.NF() << "  <- Faces" << std::endl;
		std::cout << mesh.NV() << "  <- Original Vertex number" << std::endl;
		std::cout << mesh.NVN() << "  <- Original Normal number" << std::endl;
		std::cout << mesh.NVT() << "  <- Original Tex number" << std::endl;
	}
};
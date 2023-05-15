#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "../lodepng/lodepng.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include "../cyCodeBase/cyTriMesh.h"
#include "../glm/glm.hpp"

class Object3D
{
public:
	struct vertex {
		float x, y, z;
		float r, g, b;
	};

	struct normal {
		float x, y, z;
	};

	struct vertexIndices {
		int vertex;
		int normal;
		int texCoord;
	};

	std::vector<unsigned int> m_facesIndex{};

	std::vector<vertex> m_vertices{};
	std::vector<normal> m_normals{};
	std::vector<glm::vec2> m_texCoords{};


	lodepng::State m_state{};

	std::vector<unsigned char> m_png{};
	std::vector<unsigned char> m_image{};

	GLuint m_vao{};

	GLuint m_vbo{};
	GLuint m_vboNormals{};
	GLuint m_vboTexCoords{};

	GLuint m_ebo{};

	int m_texID{};
	unsigned int m_height{}, m_width{};

	void loadFromFile(const char * filename) {
		cy::TriMesh mesh;
		mesh.LoadFromFileObj(filename);

		std::cout << "Materials: " << mesh.NM() << std::endl;

		if (mesh.NM() > 0) {
			std::cout << "MAP: " << mesh.M(0).map_Kd << std::endl;

			unsigned error = lodepng::load_file(m_png, std::string(mesh.M(0).map_Kd).c_str());

			if (!error) error = lodepng::decode(m_image, m_width, m_height, m_state, m_png);
			if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		}

		generateBuffers(mesh);
	}

	void generateBuffers(cy::TriMesh mesh) {
		//vector of vertices
		int n = mesh.NV();
		for (int i = 0; i < n; i++) {
			m_vertices.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 1.0f, 0.0f , 0.3f });
		}

		int nf = mesh.NF();
		int counter{};
		std::vector<vertexIndices> list{};
		std::vector<bool> checkVertex(n, false);

		for (int i = 0; i < nf; i++) {
			for (int k = 0; k < 3; k++) {

				if (mesh.F(i).v[k] >= n || checkVertex[mesh.F(i).v[k]]) {
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
			//se achou v�rtice com normal distinta
			if (!list.empty()) {

				//atualiza o valor no vetor de v�rtice que ele ja foi checado
				checkVertex[list[0].vertex] = true;

				//adiciona ao vbo de v�rtices no final o v�rtices que precisa duplicar

				for (int m = 1; m < list.size(); m++) {

					int tSize = m_vertices.size();
					//adiciona vertice
					m_vertices.push_back({ mesh.V(list.at(m).vertex).x, mesh.V(list.at(m).vertex).y, mesh.V(list.at(m).vertex).z, 0.5f, 0.5f, 0.5f });

					//atualiza os indices dos v�rtices duplicados
					for (int f = 0; f < nf; f++) {
						for (int k = 0; k < 3; k++) {
							if (list.at(m).vertex == mesh.F(f).v[k] && list.at(m).normal == mesh.FN(f).v[k]) {
								mesh.F(f).v[k] = tSize;
							}
						}
					}
				};

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
			//se achou v�rtice com normal distinta
			if (list.size() > 0) {

				//atualiza o valor no vetor de v�rtice que ele ja foi checado
				checkVertex[list[0].vertex] = true;

				//adiciona ao vbo de v�rtices no final o v�rtices que precisa duplicar
				for (int m = 1; m < list.size(); m++) {

					int tSize = m_vertices.size();
					//adiciona vertice
					m_vertices.push_back({ mesh.V(list.at(m).vertex).x, mesh.V(list.at(m).vertex).y, mesh.V(list.at(m).vertex).z, 0.5f, 0.5f, 0.5f });
					//normals.push_back({ mesh.VN(x.normal).x, mesh.VN(x.normal).y, mesh.VN(x.normal).z});


					for (int f = 0; f < nf; f++) {
						for (int k = 0; k < 3; k++) {
							//mesh.VN(mesh.FN(f).v[k]).x
							if (list.at(m).vertex == mesh.F(f).v[k] && list.at(m).normal == mesh.FN(f).v[k] && list.at(m).texCoord == mesh.FT(f).v[k]) {
								mesh.F(f).v[k] = tSize;
								mesh.FN(f).v[k] = list.at(m).normal;
							}
						}
					}
				};
			}

			list.clear();
		}

		int nTex = m_vertices.size();
		int texIndex{};

		for (int f = 0; f < mesh.NF(); f++) {
			for (int k = 0; k < 3; k++) {
				//std::cout << mesh.F(f).v[k] << " | " << mesh.FN(f).v[k] << " | " << mesh.GetMaterialFirstFace(f) << std::endl;
				for (int j = 0; j < nf; j++) {
					if (mesh.F(j).v[k] == texIndex) {
						texIndex++;
						m_texCoords.push_back({ mesh.VT(mesh.FT(j).v[k]).x, mesh.VT(mesh.FT(j).v[k]).y });
						m_normals.push_back({ mesh.VN(mesh.FN(j).v[k]).x, mesh.VN(mesh.FN(j).v[k]).y, mesh.VN(mesh.FN(j).v[k]).z });
						//continue;
					}
				}
			}
		}

		for (int i = 0; i < nf; i++) {
			m_facesIndex.push_back({ mesh.F(i).v[0] });
			m_facesIndex.push_back({ mesh.F(i).v[1] });
			m_facesIndex.push_back({ mesh.F(i).v[2] });
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

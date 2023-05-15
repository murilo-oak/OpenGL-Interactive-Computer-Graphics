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

	//std::vector<unsigned int> facesIndex{};

	//std::vector<vertex> objectVertices{};
	//std::vector<normal> objectNormals{};
	//std::vector<glm::vec2> objectTexCoords{};

	unsigned int height{}, width{};

	lodepng::State state{};

	std::vector<unsigned char> png{};
	std::vector<unsigned char> image{};

	GLuint vao{};

	GLuint vbo{};
	GLuint vboNormals{};
	GLuint vboTexCoords{};

	GLuint ebo{};

	int texID{};

	void loadObject(cy::TriMesh mesh, std::vector<vertex>& objectVertices, std::vector<normal>& objectNormals, std::vector<glm::vec2>& objectTexCoords, std::vector<unsigned int>& facesIndex) {
		//vector of vertices
		int n = mesh.NV();
		for (int i = 0; i < n; i++) {
			objectVertices.push_back({ mesh.V(i).x, mesh.V(i).y, mesh.V(i).z, 1.0f, 0.0f , 0.3f });
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
			//se achou vértice com normal distinta
			if (!list.empty()) {

				//atualiza o valor no vetor de vértice que ele ja foi checado
				checkVertex[list[0].vertex] = true;

				//adiciona ao vbo de vértices no final o vértices que precisa duplicar

				std::for_each(list.begin() + 1, list.end(), [&mesh, &nf, &objectVertices](vertexIndices x) {

					int tSize = objectVertices.size();
					//adiciona vertice
					objectVertices.push_back({ mesh.V(x.vertex).x, mesh.V(x.vertex).y, mesh.V(x.vertex).z, 0.5f, 0.5f, 0.5f });

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
				std::for_each(list.begin() + 1, list.end(), [&mesh, &nf, &objectVertices](vertexIndices x) {

					int tSize = objectVertices.size();
					//adiciona vertice
					objectVertices.push_back({ mesh.V(x.vertex).x, mesh.V(x.vertex).y, mesh.V(x.vertex).z, 0.5f, 0.5f, 0.5f });
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

		int nTex = objectVertices.size();
		int texIndex{};

		for (int f = 0; f < mesh.NF(); f++) {
			for (int k = 0; k < 3; k++) {
				//std::cout << mesh.F(f).v[k] << " | " << mesh.FN(f).v[k] << " | " << mesh.GetMaterialFirstFace(f) << std::endl;
				for (int j = 0; j < nf; j++) {
					if (mesh.F(j).v[k] == texIndex) {
						texIndex++;
						objectTexCoords.push_back({ mesh.VT(mesh.FT(j).v[k]).x, mesh.VT(mesh.FT(j).v[k]).y });
						objectNormals.push_back({ mesh.VN(mesh.FN(j).v[k]).x, mesh.VN(mesh.FN(j).v[k]).y, mesh.VN(mesh.FN(j).v[k]).z });
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

		std::cout << objectVertices.size() << "  <- Vertices" << std::endl;
		std::cout << objectNormals.size() << "  <- Normals" << std::endl;
		std::cout << objectTexCoords.size() << "  <- Tex" << std::endl;

		std::cout << mesh.NF() << "  <- Faces" << std::endl;
		std::cout << mesh.NV() << "  <- Original Vertex number" << std::endl;
		std::cout << mesh.NVN() << "  <- Original Normal number" << std::endl;
		std::cout << mesh.NVT() << "  <- Original Tex number" << std::endl;
	}

};


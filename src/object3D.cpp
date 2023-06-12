#include "object3D.h"

Object3D::Object3D(const char* filename) {
	set(filename);
}

Object3D::Object3D() {}

Object3D::~Object3D()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_vboNormals);
	glDeleteBuffers(1, &m_vboTexCoords);
	glDeleteBuffers(1, &m_ebo);
	glDeleteTextures(1, &m_texID);
	glDeleteVertexArrays(1, &m_vao);

	m_vertices.clear();
	m_normals.clear();
	m_texCoords.clear();
	m_facesIndex.clear();
	m_png.clear();
	m_image.clear();
}

void Object3D::set(const char* filename) {
	loadFromFile(filename);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	setVboPositions();
	setVboNormals();
	setVboTexCoords();
	setEbo(GL_STATIC_DRAW);
}

void Object3D::setTexture(unsigned texSizeWidth = 500, unsigned texSizeHeight = 500) {
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

void Object3D::setVboPositions()
{
	glCreateBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glNamedBufferStorage(m_vbo, m_vertices.size() * sizeof(vertex), m_vertices.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, r));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Object3D::setVboNormals()
{
	glCreateBuffers(1, &m_vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
	glNamedBufferStorage(m_vboNormals, m_normals.size() * sizeof(glm::vec3), m_normals.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glEnableVertexAttribArray(2);
}

void Object3D::setVboTexCoords()
{
	glCreateBuffers(1, &m_vboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
	glNamedBufferStorage(m_vboTexCoords, m_texCoords.size() * sizeof(glm::vec2), m_texCoords.data(), 0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(3);
}

void Object3D::setEbo(GLenum drawMode)
{
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_facesIndex.size() * sizeof(unsigned int), m_facesIndex.data(), drawMode);
}

void Object3D::loadFromFile(const char* filename) {
	cy::TriMesh mesh;

	mesh.LoadFromFileObj(filename);

	m_vertices.clear();
	m_normals.clear();
	m_texCoords.clear();
	m_facesIndex.clear();

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

void Object3D::generateBuffers(cy::TriMesh mesh) {
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

		delete itUniqueVertices->second;
		itUniqueVertices++;

		for (; itUniqueVertices != uniqueVertices.end(); ++itUniqueVertices) {

			auto listVertexBegin = itUniqueVertices->second->begin();

			for (auto listIt = listVertexBegin; listIt != itUniqueVertices->second->end(); ++listIt) {
				std::unordered_map<int, std::tuple<int, int, int>>::iterator elemento = *listIt;

				int duplicatedVertexIndex = newindexDuplicatedVertex;

				hashmapUniqueVertexIndices.emplace(duplicatedVertexIndex, std::make_tuple(std::get<0>(elemento->second), std::get<1>(elemento->second), elemento->first));
				hashmapFacesIndices.emplace(std::get<2>(elemento->second), duplicatedVertexIndex);

				newindexDuplicatedVertex++;

			}
			delete itUniqueVertices->second;
		}

	}

	for (int i = 0; i < hashmapUniqueVertexIndices.size(); i++) {
		int v = std::get<2>(hashmapUniqueVertexIndices.find(i)->second);
		int n = std::get<0>(hashmapUniqueVertexIndices.find(i)->second);
		int t = std::get<1>(hashmapUniqueVertexIndices.find(i)->second);

		m_vertices.push_back({ mesh.V(v).x, mesh.V(v).y, mesh.V(v).z, 1.0f, 0.0f , 0.3f });
		m_normals.push_back({ mesh.VN(n).x, mesh.VN(n).y, mesh.VN(n).z });
		m_texCoords.push_back({ mesh.VT(t).x, mesh.VT(t).y });
	}

	for (int i = 0; i < hashmapFacesIndices.size(); i++) {
		m_facesIndex.push_back(hashmapFacesIndices.find(i)->second);
	}

	mesh.Clear();
	hashmapVertexIndices.clear();
	hashmapUniqueVertexIndices.clear();
	hashmapFacesIndices.clear();
}
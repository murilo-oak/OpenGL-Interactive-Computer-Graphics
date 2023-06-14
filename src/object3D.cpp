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

void Object3D::set(const char* filename)
{
	loadFromFile(filename);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	setVboPositions();
	setVboNormals();
	setVboTexCoords();
	setEbo(GL_STATIC_DRAW);
}

void Object3D::setTexture(unsigned texSizeWidth = 500, unsigned texSizeHeight = 500) 
{
	m_texWidth = texSizeWidth;
	m_texHeight = texSizeHeight;

	// Texture
	glGenTextures(1, &m_texID);
	glBindTexture(GL_TEXTURE_2D, m_texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSizeWidth, texSizeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_image.data());

	glGenerateMipmap(GL_TEXTURE_2D);

	// Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Tiling
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

void Object3D::loadFromFile(const char* filename) 
{
	// Load mesh
	cy::TriMesh mesh;

	mesh.LoadFromFileObj(filename);

	// Clear to ensure that won't have garbage into vbos and ebo
	m_vertices.clear();
	m_normals.clear();
	m_texCoords.clear();
	m_facesIndex.clear();

	generateBuffers(mesh);
}

struct IntPairHash 
{
	std::size_t operator()(const std::pair<uint32_t, uint32_t>& p) const {
		assert(sizeof(std::size_t) >= 8);  //Ensure that std::size_t, the type of the hash, is large enough
		//Shift first integer over to make room for the second integer. The two are
		//then packed side by side.
		return (((uint64_t)p.first) << 32) | ((uint64_t)p.second);
	}
};

void Object3D::generateBuffers(cy::TriMesh mesh) 
{
	// Map to store vertex indices and corresponding attributes: (Vertex index) || (normal index, texture index, face index)
	std::unordered_multimap<int, std::tuple<int, int, int>> vertexIndexMap;
	
	// Map to store unique vertex indices within each face: (Vertex within face ID) || (Vertex unique index)
	std::unordered_map<int, int> faceIndexMap;

	// Map to store unique vertex indices and their attributes: (Vertex unique index) || (normal index, texture index)
	std::unordered_map<int, std::tuple<int, int, int>> uniqueVertexIndexMap;
	

	// Number of vertices in the mesh
	int n_vertices = mesh.NV();

	// Total number of faces in the 3D object
	int nf = mesh.NF();
	
	// ID of the vertex within each face
	int vertexWithinFaceId = 0;

	// Load vertex index map (Vertex index || (normal index, texture index, face index))
	for (int i = 0; i < nf; i++) {
		for (int k = 0; k < 3; k++) {
			int vertexIndex = mesh.F(i).v[k];
			int normalIndex = mesh.FN(i).v[k];
			int textureIndex = mesh.FT(i).v[k];

			vertexIndexMap.insert({ vertexIndex, std::make_tuple(normalIndex , textureIndex, vertexWithinFaceId) });
			vertexWithinFaceId++;
		}
	}

	// Receive the last vertex index from the mesh, which allows creating new unique duplicated vertices indices
	int newindexDuplicatedVertex = mesh.NV();

	// For each vertex index from the mesh
	for (int keyVertexIndice = 0; keyVertexIndice < n_vertices; ++keyVertexIndice) {
		// Find all repeated vertex indices
		auto range = vertexIndexMap.equal_range(keyVertexIndice);
		
		// Map to store unique vertices: (normal Index, texture Index) || (list of references to occurrences of vertex inside vertexIndexMap)
		std::unordered_map<std::pair<uint32_t, uint32_t>, std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>*, IntPairHash> uniqueVerticesMap;

		// For each vertex index, construct a list of references with duplicated vertices
		for (auto itRange = range.first; itRange != range.second; ++itRange) {
			std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>* list;
			auto inserted = uniqueVerticesMap.emplace(std::make_pair(std::get<0>(itRange->second), (std::get<1>(itRange->second))), list);

			if (std::get<1>(inserted)) {
				std::get<0>(inserted)->second = new std::list<std::unordered_map<int, std::tuple<int, int, int>>::iterator>();
			}

			std::get<0>(inserted)->second->push_back(itRange);

		}

		auto itUniqueVertices = uniqueVerticesMap.begin();

		// For each list of vertices with the same first vertex, copy their content to the uniqueVertexIndexMap and faceIndexMap.
		for (auto listIt = itUniqueVertices->second->begin(); listIt != itUniqueVertices->second->end(); ++listIt) {
			std::unordered_map<int, std::tuple<int, int, int>>::iterator element = *listIt;

			// Add the vertex, along with its normal and texture indices, to the uniqueVertexIndexMap
			uniqueVertexIndexMap.emplace(
				element->first, 
				std::make_tuple(std::get<0>(element->second),
				std::get<1>(element->second), element->first)
			);

			// Add the face index associated with the vertex to the faceIndexMap
			faceIndexMap.emplace(std::get<2>(element->second), element->first);

		}

		delete itUniqueVertices->second;
		itUniqueVertices++;

		// For the remaining lists of vertices, duplicate each vertex with a new vertex index
		for (; itUniqueVertices != uniqueVerticesMap.end(); ++itUniqueVertices) {

			auto listVertexBegin = itUniqueVertices->second->begin();

			for (auto listIt = listVertexBegin; listIt != itUniqueVertices->second->end(); ++listIt) {
				std::unordered_map<int, std::tuple<int, int, int>>::iterator elemento = *listIt;

				int duplicatedVertexIndex = newindexDuplicatedVertex;

				// Add the duplicated vertex, along with its normal and texture indices, to the uniqueVertexIndexMap
				uniqueVertexIndexMap.emplace(
					duplicatedVertexIndex, 
					std::make_tuple(std::get<0>(elemento->second), 
					std::get<1>(elemento->second), elemento->first)
				);
				
				// Add the face index associated with the duplicated vertex to the faceIndexMap
				faceIndexMap.emplace(std::get<2>(elemento->second), duplicatedVertexIndex);

				//update next new index
				newindexDuplicatedVertex++;

			}
			delete itUniqueVertices->second;
		}

	}

	// Populate vectors with all indices
	for (int i = 0; i < uniqueVertexIndexMap.size(); i++) {
		int v = std::get<2>(uniqueVertexIndexMap.find(i)->second);
		int n = std::get<0>(uniqueVertexIndexMap.find(i)->second);
		int t = std::get<1>(uniqueVertexIndexMap.find(i)->second);

		m_vertices.push_back({ mesh.V(v).x, mesh.V(v).y, mesh.V(v).z, 1.0f, 0.0f , 0.3f });
		m_normals.push_back({ mesh.VN(n).x, mesh.VN(n).y, mesh.VN(n).z });
		m_texCoords.push_back({ mesh.VT(t).x, mesh.VT(t).y });
	}

	for (int i = 0; i < faceIndexMap.size(); i++) {
		m_facesIndex.push_back(faceIndexMap.find(i)->second);
	}

	// Clear mesh and maps
	mesh.Clear();
	vertexIndexMap.clear();
	uniqueVertexIndexMap.clear();
	faceIndexMap.clear();
}
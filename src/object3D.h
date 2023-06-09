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

	/**
	 * @brief Constructor for the Object3D class that takes a filename.
	 *
	 * This constructor creates an Object3D object and initializes it by calling the `set` function
	 * with the provided filename. The `set` function loads the object data from the file, generates
	 * buffers, and performs other necessary setup operations.
	 *
	 * @param filename The name of the file from which to load the object data.
	 */
	Object3D(const char* filename);

	/**
	 * @brief Default constructor for the Object3D class.
	 *
	 * This constructor creates an empty Object3D object with no initial data. It can be used
	 * when you want to create an Object3D instance without immediately setting its data.
	 * Use the `set` function to load object data later.
	 */
	Object3D();

	/**
	 * @brief Destructor for the Object3D class.
	 *
	 * This destructor is responsible for cleaning up any resources allocated by the Object3D object.
	 * It releases the allocated buffers, textures, and vertex arrays using the corresponding OpenGL
	 * delete functions. It also clears the internal data structures used by the Object3D object.
	 */
	~Object3D();

	/**
	 * @brief Set the Object3D data and initialize the necessary buffers.
	 *
	 * This function sets the Object3D data by loading it from the specified file using
	 * the loadFromFile function. It then generates a vertex array object (VAO) and binds it.
	 * The function also calls other helper functions to set up the vertex buffer objects (VBOs)
	 * for positions, normals, and texture coordinates. Finally, it sets up the element buffer
	 * object (EBO) with the specified draw mode.
	 *
	 * @param filename The name of the file to load the Object3D data from.
	 */
	void set(const char* filename);

	/**
	 * @brief Set the texture parameters and create a texture for the Object3D.
	 *
	 * This function sets the texture size, generates a texture ID, and creates a texture
	 * for the Object3D with the specified width and height. The texture data is taken from
	 * the m_image member variable. It also sets the texture filtering, tiling, and wrapping
	 * parameters.
	 *
	 * @param texSizeWidth The width of the texture.
	 * @param texSizeHeight The height of the texture.
	 */
	void setTexture(unsigned texSizeWidth, unsigned texSizeHeight);

private:
	/**
	 * @brief Set up the vertex buffer object (VBO) for positions of the Object3D.
	 *
	 * This function creates a vertex buffer object (VBO) using glCreateBuffers and binds it
	 * using glBindBuffer. It then allocates storage for the VBO using glNamedBufferStorage,
	 * specifying the size of the data as the product of the number of vertices and the size
	 * of the vertex struct. The vertex data is provided by m_vertices. The function also sets
	 * up vertex attribute pointers using glVertexAttribPointer for the position and color
	 * attributes of the vertex. Finally, it enables the vertex attribute arrays using
	 * glEnableVertexAttribArray.
	 */
	void setVboPositions();

	/**
	 * @brief Set up the vertex buffer object (VBO) for normals of the Object3D.
	 *
	 * This function creates a vertex buffer object (VBO) for normals using glCreateBuffers
	 * and binds it using glBindBuffer. It then allocates storage for the VBO using
	 * glNamedBufferStorage, specifying the size of the data as the product of the number of
	 * normals and the size of glm::vec3. The normal data is provided by m_normals. The function
	 * sets up a vertex attribute pointer using glVertexAttribPointer for the normal attribute
	 * of the vertex, specifying the attribute index, size, type, stride, and offset. Finally,
	 * it enables the vertex attribute array using glEnableVertexAttribArray.
	 */
	void setVboNormals();
	
	/**
	 * @brief Set up the vertex buffer object (VBO) for texture coordinates of the Object3D.
	 *
	 * This function creates a vertex buffer object (VBO) for texture coordinates using
	 * glCreateBuffers and binds it using glBindBuffer. It then allocates storage for the VBO
	 * using glNamedBufferStorage, specifying the size of the data as the product of the number
	 * of texture coordinates and the size of glm::vec2. The texture coordinate data is provided
	 * by m_texCoords. The function sets up a vertex attribute pointer using glVertexAttribPointer
	 * for the texture coordinate attribute of the vertex, specifying the attribute index, size,
	 * type, stride, and offset. Finally, it enables the vertex attribute array using
	 * glEnableVertexAttribArray.
	 */
	void setVboTexCoords();
	
	/**
	 * @brief Set up the element buffer object (EBO) for the Object3D.
	 *
	 * This function generates a buffer for the element buffer object (EBO) using glGenBuffers
	 * and binds it using glBindBuffer. It then fills the buffer with data using glBufferData,
	 * specifying the target as GL_ELEMENT_ARRAY_BUFFER, the size of the data as the product of
	 * the number of faces indices and the size of unsigned int, the data provided by m_facesIndex,
	 * and the specified draw mode. The draw mode determines how the data will be interpreted
	 * during rendering.
	 */
	void setEbo(GLenum drawMode);
	
	/**
	 * @brief Load a 3D object from an OBJ file.
	 *
	 * This function loads a 3D object from an OBJ file specified by the filename.
	 * The object is loaded into the internal 'mesh' variable of the Object3D class,
	 * and then buffers are generated for vertices, normals, texture coordinates, and face indices.
	 *
	 * @param filename The name of the OBJ file to load.
	 */
	void loadFromFile(const char* filename);

	/**
	 * @brief Generates buffers for a given TriMesh object.
	 *
	 * This function generates buffers for vertices, normals, texture coordinates, and face indices
	 * based on the provided TriMesh object. The generated buffers are then stored in the respective
	 * member variables of the Object3D class.
	 *
	 * @param mesh The TriMesh object from which to generate the buffers.
	 */
	void generateBuffers(cy::TriMesh mesh);
};
#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <utility>
#include "../ThirdParty/include/lodepng/lodepng.h"
#include "../ThirdParty/include/GL/glew.h"	

class Cubemap
{
public:
	unsigned int m_height{}, m_width{};

	std::vector<unsigned char> m_images[6];
	lodepng::State m_states[6];

	GLuint m_vao{};
	GLuint m_vbo{};
	GLuint m_texCubeID{};

	float m_vertices[108] = {
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

	/**
	 * @brief Default constructor for the Cubemap class.
	 *
	 * This constructor initializes a new instance of the Cubemap class with default values.
	 */
	Cubemap();

	/**
	 * @brief Destructor for the Cubemap class.
	 *
	 * This destructor releases the OpenGL resources associated with the Cubemap object, including
	 * the vertex buffer, texture, and vertex array objects. It also clears the image data stored in
	 * the member variable `m_images`.
	 */
	~Cubemap();

	/**
	 * @brief Sets up the cubemap by configuring the buffer and texture.
	 *
	 * This function calls the `setBuffer` and `setTexture` functions to configure the buffer
	 * and texture for the cubemap, respectively.
	 */
	void set();

	/**
	 * @brief Sets up the buffers for rendering the cubemap.
	 *
	 * This function generates and binds the vertex array object (VAO) for the cubemap.
	 * It creates and initializes the vertex buffer object (VBO) for storing the vertex positions.
	 * The vertex positions are stored in the `m_vertices` array.
	 * The vertex attribute pointer is configured to interpret the data correctly.
	 * Finally, vertex attribute array 0 is enabled to use the vertex positions during rendering.
	 */
	void setBuffer();

	/**
	 * @brief Configures the texture for the cubemap using the image data in the member variable.
	 *
	 * Generates and binds a texture object for the cubemap using the image data stored in the member variable
	 * `m_images`. Each face of the cubemap is specified using a separate call to `glTexImage2D`, with the format
	 * set to GL_RGBA and the data type set to GL_UNSIGNED_BYTE. Texture parameters such as filtering, wrapping modes,
	 * and mipmapping are configured using `glTexParameteri` and `glGenerateMipmap`.
	 */
	void setTexture();

	/**
	 * @brief Loads multiple image files to create a cubemap texture.
	 *
	 * This function loads six image files corresponding to the six faces of a cubemap texture:
	 * positive X, negative X, positive Y, negative Y, positive Z, and negative Z.
	 * The image files are loaded in parallel using separate threads to improve performance.
	 * Each image is decoded into RGBA pixel data and stored in the respective element of the 'image' vector.
	 * The LodePNG state object for each image is stored in the respective element of the 'state' vector.
	 *
	 * @param posx The file path of the positive X face image.
	 * @param negx The file path of the negative X face image.
	 * @param posy The file path of the positive Y face image.
	 * @param negy The file path of the negative Y face image.
	 * @param posz The file path of the positive Z face image.
	 * @param negz The file path of the negative Z face image.
	 */
	void loadImageFilesCubeMap(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz);

private:
	/**
	 * @brief Loads an image from the specified file path and decodes it into RGBA pixel data.
	 *
	 * This function loads an image from the provided file path using the LodePNG library.
	 * It decodes the image into RGBA pixel data and stores it in the 'image' vector.
	 * The width and height of the loaded image are also returned through the 'width' and 'height' parameters.
	 * If any error occurs during the decoding process, an error message is printed to the console.
	 *
	 * @param imagePath The file path of the image to load.
	 * @param image The vector to store the RGBA pixel data of the loaded image.
	 * @param width The width of the loaded image (output parameter).
	 * @param height The height of the loaded image (output parameter).
	 * @param state The LodePNG state object used for decoding the image.
	 */
	void loadImage(const char* imagePath, std::vector<unsigned char>& image, unsigned int& width, unsigned int& height, lodepng::State state);
};
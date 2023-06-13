#pragma once
#include "object3D.h"

class Plane : public Object3D
{
public:
	Plane();
	~Plane();

	/**
	 * @brief Generates the vertex, normal, texture coordinate, and face index buffers for the Plane object.
	 *
	 * This function populates the member variables of the Plane object with the vertex, normal, texture coordinate,
	 * and face index data required to render the plane. It defines the positions, normals, texture coordinates, and
	 * face indices for a simple plane with four vertices.
	 */
	void generateBuffers();
	
	/**
	 * @brief Sets up the Plane object with the specified texture size.
	 *
	 * This function initializes the Plane object by generating and binding vertex array object (VAO),
	 * setting up vertex buffers for positions, normals, and texture coordinates, and setting up the
	 * element buffer object (EBO) for indices. It also calls the setFrameBuffer() function to set up
	 * the frame buffer for rendering.
	 *
	 * @param texSizeWidth The width of the texture.
	 * @param texSizeHeight The height of the texture.
	 */
	void set(unsigned texSizeWidth, unsigned texSizeHeight);
	
	/**
	 * @brief Sets up the frame buffer for rendering the Plane object.
	 *
	 * This function sets up the frame buffer used for rendering the Plane object. It creates a frame buffer object (FBO),
	 * a texture for rendering, and a depth buffer. It then configures the frame buffer attachments and sets the appropriate
	 * parameters.
	 */
	void setFrameBuffer();
	
	/**
	 * @brief Resizes the frame buffer used by the Plane object.
	 *
	 * This function resizes the frame buffer used by the Plane object to match the specified window width and height.
	 * It first deletes the existing frame buffer and then sets the Plane object with the new dimensions.
	 *
	 * @param windowWidth The new width of the frame buffer.
	 * @param windowHeight The new height of the frame buffer.
	 */
	void resizeFrameBuffer(unsigned int windowWidth, unsigned int windowHeight);
};


#include "plane.h"

Plane::Plane() {
	generateBuffers();
}

Plane::~Plane() {
	Object3D::~Object3D();
}

void Plane::generateBuffers() {
	m_vertices = {
		// Positions           Colors           
		{ 20.0f,  0,  20.0f,   1.0f, 0.0f, 0.0f},
		{ 20.0f,  0, -20.0f,   0.0f, 1.0f, 0.0f},
		{-20.0f,  0, -20.0f,   0.0f, 0.0f, 1.0f},
		{-20.0f,  0,  20.0f,   1.0f, 1.0f, 0.0f}
	};

	m_normals = {
		// Positions                     
		{0.0f, 1.0f, 0.0f},  // Top right
		{0.0f, 1.0f, 0.0f},  // Bottom right
		{0.0f, 1.0f, 0.0f},  // Bottom left
		{0.0f, 1.0f, 0.0f}   // Top left 
	};

	m_texCoords = {
		{0.0f, 0.0f},  // bottom left
		{0.0f, 1.0f},  // bottom right 
		{1.0f, 1.0f},  // top right
		{1.0f, 0.0f},  // top left
	};

	m_facesIndex = {
		2, 1, 0,
		2, 0, 3
	};
}

void Plane::set(unsigned texSizeWidth, unsigned texSizeHeight) 
{
	m_texWidth = texSizeWidth;
	m_texHeight = texSizeHeight;
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Positions
	glCreateBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glNamedBufferStorage(m_vbo, 4 * sizeof(Object3D::vertex), m_vertices.data(), 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, x));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object3D::vertex), (void*)offsetof(Object3D::vertex, r));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Normals
	glCreateBuffers(1, &m_vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboNormals);
	glNamedBufferStorage(m_vboNormals, m_normals.size() * sizeof(glm::vec3), m_normals.data(), 0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glEnableVertexAttribArray(2);

	// Texcoord
	glCreateBuffers(1, &m_vboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
	glNamedBufferStorage(m_vboTexCoords, m_texCoords.size() * sizeof(glm::vec2), m_texCoords.data(), 0);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);

	glEnableVertexAttribArray(3);

	// Ebo
	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), m_facesIndex.data(), GL_STATIC_DRAW);

	setFrameBuffer();
}

void Plane::setFrameBuffer() 
{
	// Framebuffer
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBuffer);

	// RenderedTexture
	glGenTextures(1, &m_renderedTexture);
	glBindTexture(GL_TEXTURE_2D, m_renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Depth buffer
	GLuint depthBuffer{};
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_texWidth, m_texHeight);

	// Configure framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderedTexture, 0);
	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
}

void Plane::resizeFrameBuffer(unsigned int windowWidth, unsigned int windowHeight) 
{
	glDeleteFramebuffers(1, &m_frameBuffer);

	m_texWidth = windowWidth;
	m_texHeight = windowHeight;
	set(windowWidth, windowHeight);
}

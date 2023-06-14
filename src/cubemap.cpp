#include "cubemap.h"

Cubemap::Cubemap() {};

Cubemap::~Cubemap()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteTextures(1, &m_texCubeID);
	glDeleteVertexArrays(1, &m_vao);

	// Delete all images
	for (auto& img : m_images) {
		img.clear();
	}
}

void Cubemap::set() 
{
	setBuffer();
	setTexture();
}

void Cubemap::setBuffer() 
{
	// Cubemap
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// Positions
	glCreateBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glNamedBufferStorage(m_vbo, sizeof(m_vertices), m_vertices, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
}


void Cubemap::setTexture() 
{
	glGenTextures(1, &m_texCubeID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texCubeID);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_images[i].data());
	}

	// Cube filter parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void Cubemap::loadImageFilesCubeMap(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz) 
{

	std::vector<const char*> imgPaths {
		posx,
		negx,
		posy,
		negy,
		posz,
		negz
	};

	std::vector<std::thread> threads;

	for (int i = 0; i < 6; i++) 
	{
		// Start a thread for each cubemap image
		threads.emplace_back([&, i] {
			std::vector<unsigned char> localImage;
			lodepng::State localState;

			loadImage(imgPaths[i], localImage, m_width, m_height, localState);


			m_images[i].swap(localImage);
			m_states[i] = localState;

			localImage.clear();
		});
	}

	// Wait threads to load all images
	for (auto& thread : threads) {
		thread.join();
	}

}

void Cubemap::loadImage(const char* imagePath, std::vector<unsigned char>& image, unsigned int& width, unsigned int& height, lodepng::State state)
{
	std::vector<unsigned char> png;
	
	unsigned error = lodepng::load_file(png, imagePath);
	if (!error) error = lodepng::decode(image, width, height, state, png);
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	png.clear();
}
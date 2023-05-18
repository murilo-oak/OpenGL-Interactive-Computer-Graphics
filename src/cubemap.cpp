#include "cubemap.h"

void Cubemap::loadImageFilesCubeMap(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz){

	std::vector<const char*> imgPaths {
		posx,
		negx,
		posy,
		negy,
		posz,
		negz
	};

	for (int i = 0; i < 6; i++) {
		unsigned error = lodepng::load_file(png[i], imgPaths[i]); //load the image file with given filename
		
		if (!error) error = lodepng::decode(image[i], width, height, state[i], png[i]);
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	}

}

void Cubemap::setBuffer() {
	//Cubemap
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//positions
	glCreateBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glNamedBufferStorage(m_vbo, sizeof(m_vertices), m_vertices, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
}

void Cubemap::setTexture() {
	glGenTextures(1, &texCubeID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCubeID);

	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[i].data());
	}

	//cube filter parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::set() {
	setBuffer();
	setTexture();
}

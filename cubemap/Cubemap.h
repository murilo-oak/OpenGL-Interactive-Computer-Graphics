#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <iostream>
#include <vector>
#include "D:\OpenglProjects\Projeto 4\Project1\lodepng\lodepng.h"

class Cubemap
{
public:
	unsigned int height{}, width{};

	lodepng::State state[6];

	std::vector<unsigned char> png[6];
	std::vector<unsigned char> image[6];

	void loadCubeMap();
};

#endif

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

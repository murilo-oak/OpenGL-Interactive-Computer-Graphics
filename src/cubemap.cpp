#include "cubemap.h"

void Cubemap::loadCubeMap() {

	std::vector<std::string> imgPaths {
		"cubemap/cubemap_posx.png",
		"cubemap/cubemap_negx.png",
		"cubemap/cubemap_posy.png",
		"cubemap/cubemap_negy.png",
		"cubemap/cubemap_posz.png",
		"cubemap/cubemap_negz.png"
	};

		for (int i = 0; i < 6; i++) {
			unsigned error = lodepng::load_file(png[i], imgPaths[i].c_str()); //load the image file with given filename
			if (!error) error = lodepng::decode(image[i], width, height, state[i], png[i]);
			if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		}

		////face positive x
		//unsigned error = lodepng::load_file(png[0], "cubemap/cubemap_posx.png"); //load the image file with given filename
		//if (!error) error = lodepng::decode(image[0], width, height, state[0], png[0]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		////face negative x
		//error = lodepng::load_file(png[1], "cubemap/cubemap_negx.png");
		//if (!error) error = lodepng::decode(image[1], width, height, state[1], png[1]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		////face positive y
		//error = lodepng::load_file(png[2], "cubemap/cubemap_posy.png"); 
		//if (!error) error = lodepng::decode(image[2], width, height, state[2], png[2]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

		////face negative y
		//error = lodepng::load_file(png[3], "cubemap/cubemap_negy.png");
		//if (!error) error = lodepng::decode(image[3], width, height, state[3], png[3]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;


		////face positive z
		//error = lodepng::load_file(png[4], "cubemap/cubemap_posz.png"); 
		//if (!error) error = lodepng::decode(image[4], width, height, state[4], png[4]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;


		////face negative z
		//error = lodepng::load_file(png[5], "cubemap/cubemap_negz.png");
		//if (!error) error = lodepng::decode(image[5], width, height, state[5], png[5]);
		//if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

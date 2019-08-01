#include "Image.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace sandbox {

void Image::update() {
    if (!imageLoaded && path != "") {
        loadImage(path);
    }
}

void Image::loadImage(const std::string& path) {
	if (imageLoaded) {
		delete[] image;
	}

	unsigned char *data = stbi_load(path.c_str(), &width, &height, &components, 0);
	std::cout << width << " " << height << " " << components << std::endl;
	image = new unsigned char[width*height*components];
	std::copy(data, data+width*height*components, image);

	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	stbi_image_free(data);
	imageLoaded = true;
}

}
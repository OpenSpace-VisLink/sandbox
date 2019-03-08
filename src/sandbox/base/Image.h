#ifndef SANDBOX_BASE_IMAGE_H_
#define SANDBOX_BASE_IMAGE_H_

#include "sandbox/SceneComponent.h"
#include <string>
#include <iostream>

namespace sandbox {

class Image : public SceneComponent {
public:
	Image() : imageLoaded(false), image(0), path("") {
		addType<Image>();
	}

	Image(int width, int height, int components) : imageLoaded(true), image(0), width(width), height(height), components(components), path("") {
		addType<Image>();
		image = new unsigned char[width*height*components];
	}

	Image(const std::string& path) : imageLoaded(false), path(path) {
		addType<Image>();
		//loadImage(path);
	}

	~Image() {
		if (imageLoaded) {
			delete[] image;
		}
	}

	void updateModel();

    void loadImage(const std::string& path);

    const unsigned char* getData() const {
    	return image;
    }

    unsigned char* getData() {
    	return image;
    }

    const unsigned char* getPixel(int x, int y) {
		return &image[(x + width*y)*components];
	}

	void setPixelValue(int x, int y, int component, unsigned char value) {
		image[(x + width*y)*components + component] = value;
		updateVersion();
	}

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getComponents() const { return components; }

private:
	bool imageLoaded;
	unsigned char* image;
	int width, height, components;
	const std::string path;
};

}

#endif /* APP_SRC_IMAGE_H_ */

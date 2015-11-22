#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include "lodepng.h"
#include <iostream>

class imageIO {

private:
	unsigned int imageWidth, imageHeight, size, imageBytes;

public:
	int loadPNG(const char * filename, std::vector<unsigned char>& imageBuffer);
	int savePNG(const char * filename, std::vector<unsigned char>& imageBuffer);
	
	unsigned char * readBMP(char * filename);
	void saveBMP(char * filename, int WIDTH, int HEIGHT, unsigned char dataArray[]);
	
	unsigned char * openImage(char * filename);
	void saveImage(char * filename, unsigned char dataArray[], int size);
	
	unsigned int getImageBytes();
	unsigned int getImageWidth();
	unsigned int getImageHeight();
	unsigned int getSize();

	void setImageBytes(int x);
	
	unsigned int getIntLE(const unsigned char *p);

};


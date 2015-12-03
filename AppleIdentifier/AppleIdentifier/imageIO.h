#pragma once
#define _CRT_SECURE_NO_DEPRECATE
#include "lodepng.h"
#include <iostream>
#include <algorithm>

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

	int isPowerOfTwo(unsigned int x);
	unsigned char* squareImage(unsigned char origImage[], int imageWidth, int imageHeight, int imageBytes);
	
	unsigned int getImageBytes();
	unsigned int getImageWidth();
	unsigned int getImageHeight();
	unsigned int getSize();

	void setImageBytes(int x);

	void setImageWidth(int width);

	void setImageHeight(int height);
	
	unsigned int getIntLE(const unsigned char *p);

};


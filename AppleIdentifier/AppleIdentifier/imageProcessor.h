#pragma once
#include <iostream>

class imageProcessor {

private:
	int redBucket[4];
	int blueBucket[4];
	int greenBucket[4];

	int redBucketTotal;
	int greenBucketTotal;
	int blueBucketTotal;
public:

	float sobelFilterX[9] = {	-1, 0, 1,
								-2, 0, 2,
								-1, 0, 1	};

	float sobelFilterY[9] = {	-1, -2, -1,
								0, 0, 0,
								1, 2, 1		};

	float gaussianFilter[25] = {	1, 4, 7, 4, 1,
									4, 16, 26, 16, 4,
									7, 26, 41, 26, 7,
									4, 16, 26, 16, 4,
									1, 4, 7, 4, 1		};


	unsigned char * RGBtoGreyscale(unsigned char image[], int imageWidth, int imageHeight, int imageBytes);

	void calculateImageSample(unsigned char image[], int index, int imageBytes, int imageWidth, float outData[], int kernelWidth, int kernelHeight);

	int applyFilter(float imageSample[], float kernel[], int kernelWidth, int kernelHeight);

	unsigned char* padOutImage(unsigned char image[], int imageWidth, int imageHeight, int imageBytes);

	void colourHistogram(unsigned char image[], int imageWidth, int imageHeight, int imageBytes);

	unsigned char * NonMaxSuppress(unsigned char * sobelX, unsigned char * sobelY, unsigned char * combinedSobel, int imageWidth, int imageHeight, int imageBytes);

};
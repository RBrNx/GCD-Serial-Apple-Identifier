#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class imageProcessor {

private:
	int redBucket;
	int blueBucket;
	int greenBucket;

	int lowThresh = 50;
	int highThresh = 150;
	int strongEdge = 255;
	int weakEdge = 100;
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

	void colourHistogram(unsigned char image[], int imageWidth, int imageHeight, int imageBytes, unsigned char mask[], float redHist[], float greenHist[], float blueHist[]);

	void colourHistogram(unsigned char image[], int imageWidth, int imageHeight, int imageBytes, float redHist[], float greenHist[], float blueHist[]);

	unsigned char * NonMaxSuppress(unsigned char * sobelX, unsigned char * sobelY, unsigned char * combinedSobel, int imageWidth, int imageHeight, int imageBytes);

	unsigned char * doubleThresholding(unsigned char data[], int imageWidth, int imageHeight, int imageBytes);

	unsigned char * hystTracking(unsigned char image[], int imageWidth, int imageHeight, int imageBytes);

	unsigned char * fillFromEdges(unsigned char image[], int imageWidth, int imageHeight, int imageBytes);

	void loadHistogram(char * filename, float redHist[], float greenHist[], float blueHist[]);

	void saveHistogram(char * filename, float redHist[], float greenHist[], float blueHist[]);

	std::string compareHistogram(float redHist[], float greenHist[], float blueHist[], std::string imageArray[]);

	void normaliseColourHistogram(float redHist[], float greenHist[], float blueHist[]);

};
#include <cmath>
#include "imageIO.h"
#include "imageProcessor.h"

imageIO imageLoader;
imageProcessor imagePro;

int main() {
	unsigned char* rawData = imageLoader.openImage("apple.png");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageWidth * imageHeight * imageBytes;

	//////////////////////
	// Colour Histogram //
	//////////////////////
	imagePro.colourHistogram(rawData, imageWidth, imageHeight, imageBytes);

	//////////////////////
	// RGB to Greyscale //
	//////////////////////
	unsigned char* greyscaleData = imagePro.RGBtoGreyscale(rawData, imageWidth, imageHeight, imageBytes);
	imageLoader.setImageBytes(1);
	imageBytes = imageLoader.getImageBytes();
	size = imageWidth * imageHeight * imageBytes;
	delete[] rawData;

	///////////////////////////
	// Apply Gaussian Filter //
	///////////////////////////
	printf("%s \n", "Applying Gaussian Filter");
	unsigned char* gaussianData = new unsigned char[size];

	for (int i = 0; i < 25; i++) {
		imagePro.gaussianFilter[i] *= 1 / 273.0f;
	}

	float gaussImageSample[25];

	for (int y = 5 + 1; y < imageHeight - (5 + 1); y++) {
		for (int x = 5 + 1; x < imageWidth - (5 + 1); x++) {

			int index = (y * imageWidth) + (x);

			imagePro.calculateImageSample(greyscaleData, index, imageBytes, imageWidth, gaussImageSample, 5, 5);

			int result = imagePro.applyFilter(gaussImageSample, imagePro.gaussianFilter, 5, 5);

			result = result > 255 ? 255 : result;

			gaussianData[index] = result;
		}
	}

	////////////////////////
	// Apply Sobel Filter //
	////////////////////////
	printf("%s \n", "Applying Sobel Filter");
	unsigned char* sobelData = new unsigned char[size];
	unsigned char* sobelX = new unsigned char[size];
	unsigned char* sobelY = new unsigned char[size];
	delete[] greyscaleData;

	float imageSample[9];

	for (int y = 1; y < imageHeight - 1; y++) {
		for (int x = 1; x < imageWidth - 1; x++) {

			int index = (y * imageWidth) + (x);

			imagePro.calculateImageSample(gaussianData, index, imageBytes, imageWidth, imageSample, 3, 3);

			int resultX = imagePro.applyFilter(imageSample, imagePro.sobelFilterX, 3, 3);
			int resultY = imagePro.applyFilter(imageSample, imagePro.sobelFilterY, 3, 3);

			int final = sqrt((resultX*resultX * 3) + (resultY*resultY * 3));

			final = final > 255 ? 255 : final;

			unsigned char finalChar = final;

			sobelData[index] = finalChar;
		}
	}
	
	///////////////////////////////////
	// Apply Non-Maximum Suppression //
	///////////////////////////////////
	sobelData = imagePro.NonMaxSuppress(sobelX, sobelY, sobelData, imageWidth, imageHeight, imageBytes);

	delete[] sobelX;
	delete[] sobelY;

	///////////////////////////////
	// Apply Double-Thresholding //
	///////////////////////////////
	sobelData = imagePro.doubleThresholding(sobelData, imageWidth, imageHeight, imageBytes);

	/////////////////////////
	// Apply Hyst Tracking //
	/////////////////////////
	sobelData = imagePro.hystTracking(sobelData, imageWidth, imageHeight, imageBytes);

	/////////////////////
	// Fill from Edges //
	/////////////////////
	sobelData = imagePro.fillFromEdges(sobelData, imageWidth, imageHeight, imageBytes);

	////////////////////////
	// Pad Sides of Image //
	////////////////////////
	imageLoader.setImageBytes(4);
	imageBytes = imageLoader.getImageBytes();
	size = imageHeight * imageWidth * imageBytes;
	unsigned char* paddedImage = imagePro.padOutImage(sobelData, imageWidth, imageHeight, imageBytes);

	imageLoader.saveImage("test-nms-dthresh-hyst-fill.png", paddedImage, size);

	delete[] gaussianData;
	delete[] sobelData;
	delete[] paddedImage;
	
	return 0;
}



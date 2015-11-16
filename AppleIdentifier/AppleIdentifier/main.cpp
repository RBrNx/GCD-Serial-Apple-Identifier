#include <math.h>
#include <cstdio>
#include <fstream>
#include "imageIO.h"

float sobelKernelY[9] = { 1, 2, 1, 
						0, 0, 0, 
					  -1, -2, -1 };

float sobelKernelX[9] = { -1, 0, 1,
						-2, 0, 2, 
						-1, 0, 1 };

float gaussianKernel[25] = { 2, 4, 5, 4, 2,
						   4, 9, 12, 9, 4,
						  5, 12, 15, 12, 5,
						   4, 9, 12, 9, 4,
						   2, 4, 5, 4, 2 };

unsigned char* RGBtoGrayscale(int width, int height, int imageBytes, unsigned char dataArray[]) {
	unsigned char* grayData = new unsigned char[height * width];

	double rc, gc, bc, gray;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			
			int index = (x + width * y) * imageBytes;

			bc = (double)dataArray[index]; //Blue, Green, Red since data is flipped
			gc = (double)dataArray[index + 1];
			rc = (double)dataArray[index + 2];
			gray = 0.2126*rc + 0.7152*gc + 0.0722*bc;

			grayData[y * width + x] = (unsigned char)gray;

			if (x == 1576 && y == 928) {
				int m = 3;;
			}
		}
	}

	return grayData;
}

float applyFilter(int imageSample[], int kernelWidth, int kernelHeight, float kernel[]) {

	float result = 0;

	for (int x = 0; x < kernelHeight; x++) {
		for (int y = 0; y < kernelWidth; y++) {
			int index = x * kernelWidth + y;

			float imageIndex = imageSample[index];
			float kernelIndex = kernel[index];
			result += imageIndex * kernelIndex;
		}
	}

	delete[] imageSample;
	return result;
}

int* calculateImageSample(unsigned char dataArray[], int imageIndex, int kernelWidth) {
	int* imageSample = new int[kernelWidth*kernelWidth];
	int kernelSides = (int)floor(kernelWidth / 2.0f);

	for (int x = -kernelSides, i = 0; x <= kernelSides; x++, i++) {
		for (int y = -kernelSides, j = 0; y <= kernelSides; y++, j++) {
			int kernelIndex = (x + kernelSides) * kernelWidth + (y + kernelSides);

			imageSample[i * kernelWidth + j] = dataArray[kernelIndex + imageIndex];
		}
	}

	return imageSample;
}

int main() {
	imageIO imageLoader;
	unsigned char* data = imageLoader.openImage("images.png");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageLoader.getSize();
	
	printf("%s \n", "Converting Colour to Grayscale");
	unsigned char* grayData = RGBtoGrayscale(imageWidth, imageHeight, imageBytes, data);
	delete[] data;

  	//unsigned char* gaussianData = new unsigned char[imageWidth * imageHeight];
	//float returnedGauss;

	//printf("%s \n", "Applying Gaussian Filter");
	///////////////////////////
	// Apply Gaussian Filter //
	///////////////////////////
	/*float gaussianMultiply = 1 / 159.0f;
	for (int i = 0; i < 25; i++) {
		gaussianKernel[i] *= gaussianMultiply;
	}

	for (int x = 2; x < imageHeight - 2; x++) {
		for (int y = 2; y < imageWidth - 2; y++) {

			int index = x * imageWidth + y;
			
			returnedGauss = applyFilter( calculateImageSample(grayData, index, 5), 5, 5, gaussianKernel);
				
			gaussianData[index] = returnedGauss;

			returnedGauss = 0;
		}
	}*/

	
	//delete[] grayData;
	unsigned char* sobelData = new unsigned char[imageWidth * imageHeight];
	int returnedX, returnedY, squaredResult;
	printf("%s \n", "Applying Sobel Filter");
	////////////////////////
	// Apply Sobel Filter //
	////////////////////////
	for (int x = 0; x < imageHeight; x++) {
		for (int y = 0; y < imageWidth; y++) {

			int index = x * imageWidth + y;

			if (x == 1576 && y == 928) {
				int m = 3;
			}
			returnedX = applyFilter( calculateImageSample(grayData , index, 3), 3, 3, sobelKernelX);
			returnedY = applyFilter( calculateImageSample(grayData, index, 3), 3, 3, sobelKernelY);

			squaredResult = sqrt((returnedX * returnedX) + (returnedY * returnedY));
			
			sobelData[index] = (unsigned char)squaredResult;

			returnedX = returnedY = squaredResult = 0;
		}
	}

	imageLoader.saveImage("sobeltest.png", sobelData, size);

	delete[] sobelData;
	//delete[] gaussianData;

	return 0;

}


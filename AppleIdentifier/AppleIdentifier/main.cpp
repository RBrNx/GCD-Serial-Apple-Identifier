#include <math.h>
#include <cstdio>
#include <fstream>
#include "imageIO.h"

/*float sobelKernelY[9] = { 1, 2, 1, 
						0, 0, 0, 
					  -1, -2, -1 };

float sobelKernelX[9] = { -1, 0, 1,
						-2, 0, 2, 
						-1, 0, 1 };

float gaussianKernel[25] = { 2, 4, 5, 4, 2,
						   4, 9, 12, 9, 4,
						  5, 12, 15, 12, 5,
						   4, 9, 12, 9, 4,
						   2, 4, 5, 4, 2 };*/

float sobelKernelX[] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
float sobelKernelY[] = { 1, 0, -1, 2, 0, -2, 1, 0, -1 };

unsigned char* RGBtoGrayscale(int width, int height, int imageBytes, unsigned char dataArray[]) {
	unsigned char* grayData = new unsigned char[height * width];

	double rc, gc, bc, gray;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			
			int index = i * (width * imageBytes) + (j * imageBytes);

			bc = (double)dataArray[index]; //Blue, Green, Red since data is flipped
			gc = (double)dataArray[index + 1];
			rc = (double)dataArray[index + 2];
			gray = 0.2126*rc + 0.7152*gc + 0.0722*bc;

			grayData[i * width + j] = (unsigned char)gray;
		}
	}

	return grayData;
}

/*float applyFilter(int imageSample[], int kernelWidth, int kernelHeight, float kernel[]) {

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
}*/

void applyFilter(unsigned char imageSample[], unsigned char returnedFilter[], int col, int row, int imageWidth, int imageHeight, int kernelWidth, int kernelHeight, float kernel[]) {

	int kernelWidthExtent = (int)floor(kernelWidth / 2.0f);
	bool validIndex = (row >= kernelWidthExtent && row < imageHeight - kernelWidthExtent && col >= kernelWidthExtent && col < imageWidth - kernelWidthExtent);
	float result = 0;

	if (validIndex) {
		for (int x = -kernelWidthExtent; x <= kernelWidthExtent; x++) {
			for (int y = -kernelWidthExtent; y <= kernelWidthExtent; y++) {
				int imageIndex = (x + col) + (y + row) * imageWidth;
				int kernelIndex = (x + kernelWidthExtent) + (y + kernelWidthExtent) * kernelWidth;

				result += (float)imageSample[imageIndex] * kernel[kernelIndex];
				
			}
		}
	}
	else {
		result = (float)imageSample[col + row*imageWidth];
	}
	returnedFilter[col + row*imageWidth] = (unsigned char)result;

	//delete[] imageSample;
}

void applyKernal(unsigned char * inData, int rowStride, int row, int col, int maxRows, int maxCols, unsigned char * outData, float * kernal, int kernalWidth, int kernalHeight)
{
	//assert(kernalWidth % 2 != 0);
	//assert(kernalHeight % 2 != 0);

	int kernalWidthExtent = (int)floor(kernalWidth / 2.0f);
	int kernalHeightExtent = (int)floor(kernalHeight / 2.0f);

	bool validData = (inData && outData && kernal);
	bool validIndex = (row >= kernalHeightExtent && row < maxRows - kernalHeightExtent && col >= kernalWidthExtent && col < maxCols - kernalWidthExtent);
	if (validData)
	{
		float result = 0;
		if (validIndex)
		{
			for (int x = -kernalWidthExtent; x <= kernalWidthExtent; ++x)
			{
				for (int y = -kernalHeightExtent; y <= kernalHeightExtent; ++y)
				{
					int dataIndex = (col + x) + (row + y)*rowStride;
					int kernalIndex = (x + kernalWidthExtent) + (y + kernalHeightExtent) * kernalWidth;
					result += inData[dataIndex] * kernal[kernalIndex];
				}
			}
		}
		else
		{
			result = (float)inData[col + row*rowStride];
		}
		outData[col + row*rowStride] = (int)result;
	}
	else
	{
		std::cerr << "Invalid data provided" << std::endl;
	}
}

/*int* calculateImageSample(unsigned char dataArray[], int imageIndex, int kernelWidth) {
	int* imageSample = new int[kernelWidth*kernelWidth];
	int kernelSides = (int)floor(kernelWidth / 2.0f);

	for (int x = -kernelSides, i = 0; x <= kernelSides; x++, i++) {
		for (int y = -kernelSides, j = 0; y <= kernelSides; y++, j++) {
			int kernelIndex = (x + kernelSides) * kernelWidth + (y + kernelSides);

			imageSample[i * kernelWidth + j] = dataArray[kernelIndex + imageIndex];
		}
	}

	return imageSample;
}*/

int main() {
	imageIO imageLoader;
	unsigned char* data = imageLoader.openImage("images.bmp");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageLoader.getSize();
	
	printf("%s \n", "Converting Colour to Grayscale");
	unsigned char* grayData = RGBtoGrayscale(imageWidth, imageHeight, imageBytes, data);
	unsigned char* sobelX = new unsigned char[imageWidth * imageHeight];
	unsigned char* sobelY = new unsigned char[imageWidth * imageHeight];
	delete[] data;

  	unsigned char* gaussianData = new unsigned char[imageWidth * imageHeight];
	float returnedGauss;

	printf("%s \n", "Applying Gaussian Filter");
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
	//unsigned char* sobelData = new unsigned char[imageWidth * imageHeight];
	//int* sobelData = new int[imageWidth * imageHeight];
	unsigned char* sobelData = new unsigned char[imageWidth * imageHeight];
	int returnedX, returnedY, squaredResult;
	printf("%s \n", "Applying Sobel Filter");
	////////////////////////
	// Apply Sobel Filter //
	////////////////////////
	for (int x = 1; x < imageHeight-1; x++) {
		for (int y = 1; y < imageWidth-1; y++) {

			int index = x * imageWidth + y;
			//int index = y*imageWidth + x;

			if (x == 1576 && y == 928) {
				int m = 3;
			}
			//returnedX = applyFilter( calculateImageSample(grayData , index, 3), 3, 3, sobelKernelX);
			//returnedY = applyFilter( calculateImageSample(grayData, index, 3), 3, 3, sobelKernelY);
			applyFilter(grayData, sobelX, y, x, imageWidth, imageHeight, 3, 3, sobelKernelX);
			applyFilter(grayData, sobelY, y, x, imageWidth, imageHeight, 3, 3, sobelKernelY);
			//squaredResult = sqrt((returnedX * returnedX) + (returnedY * returnedY));

			int squared = sqrt(sobelX[index] * sobelX[index] + sobelY[index] * sobelY[index]);
			//squared = (squared * 255) / 360;
			if (index == 5000000) {
				int m = 3;
			}
			
			//sobelData[index] = (int)sqrt(sobelX[index] * sobelX[index] + sobelY[index] * sobelY[index]);
			sobelData[index] = (unsigned char)squared;

			returnedX = returnedY = squaredResult = 0;
		}
	}


	/*for (int row = 0; row<imageHeight; ++row)
	{
		for (int col = 0; col<imageWidth; ++col)
		{
			applyKernal(grayData, imageWidth, row, col, imageHeight, imageWidth, sobelX, sobelKernelX, 3, 3);
			applyKernal(grayData, imageWidth, row, col, imageHeight, imageWidth, sobelY, sobelKernelY, 3, 3);
			int index = col + row*imageWidth;
			sobelData[index] = (unsigned char)sqrt(sobelX[index] * sobelX[index] + sobelY[index] * sobelY[index]);
		}
	}*/
		for (int i = 5000000; i < 5000010; i++) {
			printf("%d \n", sobelData[i]);
		}

	std::cin.get();

	imageLoader.saveImage("test2.bmp", sobelData, size);

	delete[] sobelX;
	delete[] sobelY;
	delete[] sobelData;
	delete[] gaussianData;

	std::cin.get();
	//return 0;

}


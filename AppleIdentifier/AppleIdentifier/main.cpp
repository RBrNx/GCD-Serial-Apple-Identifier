#include <cmath>
#include "imageIO.h"

int sobelFilterX[9] = { -1, 0, 1,
						-2, 0, 2,
						-1, 0, 1 };

int sobelFilterY[9] = { -1, -2, -1,
						 0, 0, 0,
						 1, 2, 1 };

imageIO imageLoader;

unsigned char* RGBtoGreyscale(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	
	int size = imageWidth * imageHeight * imageBytes;
	unsigned char* greyscaleData = new unsigned char[size];

	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {
			
			int index = ( (y * imageWidth * imageBytes) + (x * imageBytes) );
			float greyscale = 0;
			greyscale += image[index] * 0.21;
			greyscale += image[index + 1] * 0.71;
			greyscale += image[index + 2] * 0.07;
			greyscaleData[index] = greyscaleData[index + 1] = greyscaleData[index + 2] = greyscale;
		}
	}

	return greyscaleData;
}

void calculateImageSample(unsigned char image[], int index, int imageBytes, int imageWidth, int outData[], int kernelWidth, int kernelHeight) {

	for (int x = 0; x < kernelWidth; x++) {
		for (int y = 0; y < kernelHeight; y++) {
			int sampleIndex = x * kernelHeight + y;

			int offsetX = x - (kernelWidth / 2);
			int offsetY = y - (kernelHeight / 2);
			int imageIndex = index + (offsetY*imageWidth*imageBytes + (offsetX*imageBytes));
			outData[sampleIndex] = image[imageIndex];
		}
	}
}

int applyFilter(int imageSample[], int kernel[], int kernelWidth, int kernelHeight) {

	int filteredValue = 0;

	for (int x = 0; x < kernelWidth; x++) {
		for (int y = 0; y < kernelHeight; y++) {
			
			int index = x * kernelHeight + y;
			filteredValue += imageSample[index] * kernel[index];
		}
	}

	return filteredValue;

}

void padOutImage(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {

	for (int x = 0; x < imageWidth; x += 4) {
		image[x] = 0;
		image[x + 1] = 0;
		image[x + 2] = 0;
		if(imageBytes == 4) image[x + 3] = 255;

		image[(imageHeight - 1) + x] = 0;
		image[(imageHeight - 1) + x + 1] = 0;
		image[(imageHeight - 1) + x + 2] = 0;
		if (imageBytes == 4) image[(imageHeight - 1) + x + 3] = 255;
	}

	for (int y = 0; y < imageHeight; y++) {
		image[0 + y * (imageWidth * imageBytes)] = 0;
		image[1 + y * (imageWidth * imageBytes)] = 0;
		image[2 + y * (imageWidth * imageBytes)] = 0;
		if (imageBytes == 4) image[3 + y * (imageWidth * imageBytes)] = 255;

		image[((imageWidth - 1) * imageBytes) + (y * (imageWidth * imageBytes))] = 0;
		image[(((imageWidth - 1) * imageBytes) + 1) + (y * (imageWidth * imageBytes))] = 0;
		image[(((imageWidth - 1) * imageBytes) + 2) + (y * (imageWidth * imageBytes))] = 0;
		if (imageBytes == 4) image[(((imageWidth - 1) * imageBytes) + 3) + (y * (imageWidth * imageBytes))] = 255;
	}
}

int main() {
	unsigned char* rawData = imageLoader.openImage("images.bmp");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageWidth * imageHeight * imageBytes;

	unsigned char* greyscaleData = RGBtoGreyscale(rawData, imageWidth, imageHeight, imageBytes);

	////////////////////////
	// Apply Sobel Filter //
	////////////////////////
	unsigned char* sobelData = new unsigned char[size];

	int imageSample[9];

	for (int y = 1; y < imageHeight - 1; y++) {
		for (int x = 1; x < imageWidth - 1; x++) {

			int index = ((y * imageWidth * imageBytes) + (x * imageBytes));

			calculateImageSample(greyscaleData, index, imageBytes, imageWidth, imageSample, 3, 3);

			int resultX = applyFilter(imageSample, sobelFilterX, 3, 3);
			int resultY = applyFilter(imageSample, sobelFilterY, 3, 3);

			int final = sqrt((resultX*resultX * 3) + (resultY*resultY * 3));
			final = final > 255 ? 255 : final;

			unsigned char finalChar = final;

			sobelData[index] = finalChar;
			sobelData[index + 1] = finalChar;
			sobelData[index + 2] = finalChar;
			if (imageBytes == 4) {
				sobelData[index + 3] = 255;
			}
		}
	}
	
	////////////////////////
	// Pad Sides of Image //
	////////////////////////
	padOutImage(rawData, imageWidth, imageHeight, imageBytes);

	imageLoader.saveImage("test.bmp", rawData, size);
}



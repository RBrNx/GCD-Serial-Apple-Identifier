#include <cmath>
#include "imageIO.h"

float sobelFilterX[9] = { -1, 0, 1,
						-2, 0, 2,
						-1, 0, 1 };

float sobelFilterY[9] = { -1, -2, -1,
						 0, 0, 0,
						 1, 2, 1 };

float gaussianFilter[25] = { 1, 4, 7, 4, 1,
						4, 16, 26, 16, 4,
						7, 26, 41, 26, 7,
						4, 16, 26, 16, 4,
						1, 4, 7, 4, 1 };

float gaussianFilter2[25] = { 2, 4, 5, 4, 2,
							 4, 9, 12, 9, 4,
                            5, 12, 15, 12, 5,
							 4, 9, 12, 9, 4,
							 2, 4, 5, 4, 2 };

int redBucket[4];
int blueBucket[4];
int greenBucket[4];

int redBucketTotal;
int greenBucketTotal;
int blueBucketTotal;


imageIO imageLoader;

unsigned char* RGBtoGreyscale(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	printf("%s \n", "Converting RGB to Greyscale");

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

void calculateImageSample(unsigned char image[], int index, int imageBytes, int imageWidth, float outData[], int kernelWidth, int kernelHeight) {

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

int applyFilter(float imageSample[], float kernel[], int kernelWidth, int kernelHeight) {

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
	printf("%s \n", "Padding edges of image");

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
	unsigned char* rawData = imageLoader.openImage("apple.png");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageWidth * imageHeight * imageBytes;

	//////////////////////
	// Colour Histogram //
	//////////////////////
	printf("%s \n", "Performing Colour Histogram");
	for (int y = 0; y < imageHeight; y+=4) {
		for (int x = 0; x < imageWidth; x+=4) {
			int index = y * imageHeight + x;

			if (rawData[index + 3] != 0) {

				//std::cout << rawData[index] << rawData[index + 1] << rawData[index + 2];

				if (rawData[index] < 251 || rawData[index + 1] < 251 || rawData[index + 2] < 251) {

					if (rawData[index] > 0 && rawData[index] < 64) redBucket[1] += 1;
					if (rawData[index] > 63 && rawData[index] < 128) redBucket[2] += 1;
					if (rawData[index] > 127 && rawData[index] < 192) redBucket[3] += 1;
					if (rawData[index] > 191 && rawData[index] < 256) redBucket[4] += 1;

					if (rawData[index + 1] > 0 && rawData[index + 1] < 64) greenBucket[1] += 1;
					if (rawData[index + 1] > 63 && rawData[index + 1] < 128) greenBucket[2] += 1;
					if (rawData[index + 1] > 127 && rawData[index + 1] < 192) greenBucket[3] += 1;
					if (rawData[index + 1] > 191 && rawData[index + 1] < 256) greenBucket[4] += 1;

					if (rawData[index + 2] > 0 && rawData[index + 2] < 64) blueBucket[1] += 1;
					if (rawData[index + 2] > 63 && rawData[index + 2] < 128) blueBucket[2] += 1;
					if (rawData[index + 2] > 127 && rawData[index + 2] < 192) blueBucket[3] += 1;
					if (rawData[index + 2] > 191 && rawData[index + 2] < 256) blueBucket[4] += 1;
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		redBucketTotal += redBucket[i];
		greenBucketTotal += greenBucket[i];
		blueBucketTotal += blueBucket[i];
	}

	printf("%d \n", redBucketTotal/4);
	printf("%d \n", greenBucketTotal/4);
	printf("%d \n", blueBucketTotal/4);

	std::cin.get();

	unsigned char* greyscaleData = RGBtoGreyscale(rawData, imageWidth, imageHeight, imageBytes);
	delete[] rawData;

	///////////////////////////
	// Apply Gaussian Filter //
	///////////////////////////
	printf("%s \n", "Applying Gaussian Filter");
	unsigned char* gaussianData = new unsigned char[size];

	for (int i = 0; i < 25; i++) {
		gaussianFilter[i] *= 1 / 273.0f;
	}

	float gaussImageSample[25];

	for (int y = 5 + 1; y < imageHeight - (5 + 1); y++) {
		for (int x = 5 + 1; x < imageWidth - (5 + 1); x++) {

			int index = ((y * imageWidth * imageBytes) + (x * imageBytes));

			calculateImageSample(greyscaleData, index, imageBytes, imageWidth, gaussImageSample, 5, 5);

			int result = applyFilter(gaussImageSample, gaussianFilter, 5, 5);

			result = result > 255 ? 255 : result;

			gaussianData[index] = result;
			gaussianData[index + 1] = result;
			gaussianData[index + 2] = result;
			if (imageBytes == 4) {
				gaussianData[index + 3] = 255;
			}
		}
	}

	////////////////////////
	// Apply Sobel Filter //
	////////////////////////
	printf("%s \n", "Applying Sobel Filter");
	unsigned char* sobelData = new unsigned char[size];
	delete[] greyscaleData;

	float imageSample[9];

	for (int y = 1; y < imageHeight - 1; y++) {
		for (int x = 1; x < imageWidth - 1; x++) {

			int index = ((y * imageWidth * imageBytes) + (x * imageBytes));

			calculateImageSample(gaussianData, index, imageBytes, imageWidth, imageSample, 3, 3);

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
	padOutImage(sobelData, imageWidth, imageHeight, imageBytes);

	imageLoader.saveImage("sobel+gauss2.png", sobelData, size);

	delete[] gaussianData;
	delete[] sobelData;

	return 0;
}



#include "imageProcessor.h"

unsigned char* imageProcessor::RGBtoGreyscale(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	printf("%s \n", "Converting RGB to Greyscale");

	int greySize = imageWidth * imageHeight;
	unsigned char* greyscaleData = new unsigned char[greySize];

	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {

			int imageIndex = ((y * imageWidth * imageBytes) + (x * imageBytes));
			int greyIndex = ((y * imageWidth) + (x));
			float greyscale = 0;
			greyscale += image[imageIndex] * 0.21;
			greyscale += image[imageIndex + 1] * 0.71;
			greyscale += image[imageIndex + 2] * 0.07;
			greyscaleData[greyIndex] = greyscale;
			int m = 3;
		}
	}

	return greyscaleData;
}

void imageProcessor::calculateImageSample(unsigned char image[], int index, int imageBytes, int imageWidth, float outData[], int kernelWidth, int kernelHeight) {

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

int imageProcessor::applyFilter(float imageSample[], float kernel[], int kernelWidth, int kernelHeight) {

	int filteredValue = 0;

	for (int x = 0; x < kernelWidth; x++) {
		for (int y = 0; y < kernelHeight; y++) {

			int index = x * kernelHeight + y;
			filteredValue += imageSample[index] * kernel[index];
		}
	}

	return filteredValue;

}

unsigned char* imageProcessor::padOutImage(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	printf("%s \n", "Coverting image back to original size");
	int paddedSize = imageWidth * imageHeight * imageBytes;
	unsigned char* paddedImage = new unsigned char[paddedSize];

	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {
			int index = (y * imageWidth + x);
			int paddedIndex = ((y * imageWidth * imageBytes) + (x * imageBytes));

			paddedImage[paddedIndex] = image[index];
			paddedImage[paddedIndex + 1] = image[index];
			paddedImage[paddedIndex + 2] = image[index];
			paddedImage[paddedIndex + 3] = 255;

			int m = 3;
		}
	}
	
	printf("%s \n", "Padding edges of image");
	for (int x = 0; x < imageWidth; x += 4) {
		paddedImage[x] = 0;
		paddedImage[x + 1] = 0;
		paddedImage[x + 2] = 0;
		if (imageBytes == 4) paddedImage[x + 3] = 255;

		paddedImage[(imageHeight - 1) + x] = 0;
		paddedImage[(imageHeight - 1) + x + 1] = 0;
		paddedImage[(imageHeight - 1) + x + 2] = 0;
		if (imageBytes == 4) paddedImage[(imageHeight - 1) + x + 3] = 255;
	}

	for (int y = 0; y < imageHeight; y++) {
		paddedImage[0 + y * (imageWidth * imageBytes)] = 0;
		paddedImage[1 + y * (imageWidth * imageBytes)] = 0;
		paddedImage[2 + y * (imageWidth * imageBytes)] = 0;
		if (imageBytes == 4) paddedImage[3 + y * (imageWidth * imageBytes)] = 255;

		paddedImage[((imageWidth - 1) * imageBytes) + (y * (imageWidth * imageBytes))] = 0;
		paddedImage[(((imageWidth - 1) * imageBytes) + 1) + (y * (imageWidth * imageBytes))] = 0;
		paddedImage[(((imageWidth - 1) * imageBytes) + 2) + (y * (imageWidth * imageBytes))] = 0;
		if (imageBytes == 4) paddedImage[(((imageWidth - 1) * imageBytes) + 3) + (y * (imageWidth * imageBytes))] = 255;
	}

	return paddedImage;
}

void imageProcessor::colourHistogram(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	printf("%s \n", "Performing Colour Histogram");
	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {

			int index = ((y * imageWidth * imageBytes) + (x * imageBytes));

			if (image[index + 3] != 0) {

				if (image[index] < 251 || image[index + 1] < 251 || image[index + 2] < 251) {

					if (image[index] > 0 && image[index] < 64) redBucket[0] += 1;
					if (image[index] > 63 && image[index] < 128) redBucket[1] += 1;
					if (image[index] > 127 && image[index] < 192) redBucket[2] += 1;
					if (image[index] > 191 && image[index] < 256) redBucket[3] += 1;

					if (image[index + 1] > 0 && image[index + 1] < 64) greenBucket[0] += 1;
					if (image[index + 1] > 63 && image[index + 1] < 128) greenBucket[1] += 1;
					if (image[index + 1] > 127 && image[index + 1] < 192) greenBucket[2] += 1;
					if (image[index + 1] > 191 && image[index + 1] < 256) greenBucket[3] += 1;

					if (image[index + 2] > 0 && image[index + 2] < 64) blueBucket[0] += 1;
					if (image[index + 2] > 63 && image[index + 2] < 128) blueBucket[1] += 1;
					if (image[index + 2] > 127 && image[index + 2] < 192) blueBucket[2] += 1;
					if (image[index + 2] > 191 && image[index + 2] < 256) blueBucket[3] += 1;
				}
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		redBucketTotal += redBucket[i];
		greenBucketTotal += greenBucket[i];
		blueBucketTotal += blueBucket[i];
	}

	printf("%s", "Total Red Pixels: ");
	printf("%d \n", redBucketTotal);
	printf("%s", "Total Green Pixels: ");
	printf("%d \n", greenBucketTotal);
	printf("%s", "Total Blue Pixels: ");
	printf("%d \n", blueBucketTotal);
}

unsigned char* imageProcessor::NonMaxSuppress(unsigned char* sobelX, unsigned char* sobelY, unsigned char*  combinedSobel, int imageWidth, int imageHeight, int imageBytes) {
	//Calculate the Edge Strengths
	printf("%s \n", "NMS: Calculating Edge Strengths");
	int size = imageWidth * imageHeight * imageBytes;
	unsigned char* edgeStrength = new unsigned char[size];

	printf("%s \n", "NMS: Calculating Gradient Directions");
	for (int i = 0; i < size; i++) {
		//Calculate the gradient direction
		if (sobelX[i] == 0) {
			if (sobelY[i] == 0) {
				edgeStrength[i] = 0;
			}
			else {
				edgeStrength[i] = 90;
			}
		}
		else {
			edgeStrength[i] = (float)atan(abs(sobelY[i]) / (float)abs(sobelX[i]));
		}

		//Round the gradient direction to nearest 45 degree
		if (edgeStrength[i] >= 0.0f && edgeStrength[i] < 22.5f) { edgeStrength[i] = 0.0f; }
		else if (edgeStrength[i] >= 22.5f && edgeStrength[i] < 67.5f) { edgeStrength[i] = 45.0f; }
		else if (edgeStrength[i] >= 67.5f && edgeStrength[i] <= 112.5f) { edgeStrength[i] = 90.0f; }
		else if (edgeStrength[i] > 112.5f && edgeStrength[i] <= 157.5f) { edgeStrength[i] = 135.0f; }

	}

	printf("%s \n", "Performing NMS");
	//Use edge strengths in NMS
	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {

			int index = ((y * imageWidth * imageBytes) + (x * imageBytes));

			if (edgeStrength[index] == 0.0f) {
				int rightPixel = ((y * imageWidth * imageBytes) + (x + 1 * imageBytes));
				int leftPixel = ((y * imageWidth * imageBytes) + (x - 1 * imageBytes));

				if ((leftPixel > 0 && rightPixel < size) && (combinedSobel[index] < combinedSobel[leftPixel] || combinedSobel[index] < combinedSobel[rightPixel])) { combinedSobel[index] = 0; }
			}
			else if (edgeStrength[index] == 45.0f) {
				int diagUpRightPixel = ((y - 1 * imageWidth * imageBytes) + (x + 1 * imageBytes));
				int diagDownLeftPixel = ((y + 1 * imageWidth * imageBytes) + (x - 1 * imageBytes));

				if ((diagUpRightPixel > 0 && diagUpRightPixel < size && diagDownLeftPixel > 0 && diagDownLeftPixel < size) && (combinedSobel[index] < combinedSobel[diagDownLeftPixel] || combinedSobel[index] < combinedSobel[diagUpRightPixel])) { combinedSobel[index] = 0; }
			}
			else if (edgeStrength[index] == 90.0f) {
				int abovePixel = ((y - 1 * imageWidth * imageBytes) + (x * imageBytes));
				int belowPixel = ((y + 1 * imageWidth * imageBytes) + (x * imageBytes));

				if ((abovePixel > 0 && belowPixel < size) && (combinedSobel[index] < combinedSobel[belowPixel] || combinedSobel[index] < combinedSobel[abovePixel])) { combinedSobel[index] = 0; }
			}
			else if (edgeStrength[index] == 135.0f) {
				int diagUpLeftPixel = ((y - 1 * imageWidth * imageBytes) + (x - 1 * imageBytes));
				int diagDownRightPixel = ((y + 1 * imageWidth * imageBytes) + (x + 1 * imageBytes));

				if ((diagUpLeftPixel > 0 && diagUpLeftPixel < size && diagDownRightPixel > 0 && diagDownRightPixel < size) && (combinedSobel[index] < combinedSobel[diagDownRightPixel] || combinedSobel[index] < combinedSobel[diagUpLeftPixel])) { combinedSobel[index] = 0; }
			}

		}
	}

	return combinedSobel;
}

unsigned char* imageProcessor::doubleThresholding(unsigned char image[], int imageWidth, int imageHeight, int imageBytes) {
	int lowThresh = 75;
	int highThresh = 200;
	int strongEdge = 255;
	int weakEdge = 100;
	int size = imageWidth * imageHeight * imageBytes;

	printf("%s \n", "Performing Double-Thresholding");
	unsigned char* treshData = new unsigned char[size];

	for (int i = 0; i < size; i++) {
		if (image[i] < lowThresh) { treshData[i] = 0; }
		else if (image[i] > highThresh) { treshData[i] = strongEdge; }
		else if (image[i] >= lowThresh && image[i] <= highThresh) { treshData[i] = weakEdge; }
	}
	return treshData;
}
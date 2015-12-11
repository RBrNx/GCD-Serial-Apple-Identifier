#include <cmath>
#include "imageIO.h"
#include "imageProcessor.h"

float sobelFilterX[9] = { -1, 0, 1,
-2, 0, 2,
-1, 0, 1 };

float sobelFilterY[9] = { -1, -2, -1,
0, 0, 0,
1, 2, 1 };

imageIO imageLoader;
imageProcessor imagePro;

float redHist[256];
float greenHist[256];
float blueHist[256];
std::string imageArray[14] = { "Apples/Braeburn", "Apples/Cortland", "Apples/Fuji", "Apples/Gala", "Apples/Ginger-Gold",
								"Apples/Golden-Delicious", "Apples/Granny-Smith", "Apples/Honeycrisp", "Apples/Jonagold",
								"Apples/Jonathan", "Apples/McIntosh", "Apples/Pacific-Rose", "Apples/Paula-Red", "Apples/Red-Delicious" };


int main() {
	//Load the image into an unsigned char array and get the Width/Height/Bytes from the Class
	unsigned char* rawData = imageLoader.openImage("Apples/Granny-Smith.png");
	int imageWidth = imageLoader.getImageWidth();
	int imageHeight = imageLoader.getImageHeight();
	int imageBytes = imageLoader.getImageBytes();
	int size = imageWidth * imageHeight * imageBytes;

	//Increase the dimensions of the image to a power of two and get the Width/Height again
	unsigned char* squareImage = imageLoader.squareImage(rawData, imageWidth, imageHeight, imageBytes);
	imageWidth = imageLoader.getImageWidth();
	imageHeight = imageLoader.getImageHeight();
	size = imageWidth * imageHeight * imageBytes;
	delete[] rawData;

	//////////////////////
	// RGB to Greyscale //
	//////////////////////
	//Convert RGB Image to Greyscale
	unsigned char* greyscaleData = imagePro.RGBtoGreyscale(squareImage, imageWidth, imageHeight, imageBytes);
	imageLoader.setImageBytes(1); //Greyscale image is only 1 byte instead of 4
	imageBytes = imageLoader.getImageBytes();
	size = imageWidth * imageHeight * imageBytes;
	delete[] squareImage;

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

			resultX = resultX > 255 ? 255 : resultX; 
			resultX = resultX < 0 ? 0 : resultX;
			resultY = resultY > 255 ? 255 : resultY;
			resultY = resultY < 0 ? 0 : resultY;
			final = final > 255 ? 255 : final;

			unsigned char finalChar = final;
			unsigned char finalCharX = resultX;
			unsigned char finalCharY = resultY;

			sobelData[index] = finalChar;
			sobelX[index] = finalCharX;
			sobelY[index] = finalCharY;
		}
	}

	/*imageLoader.setImageBytes(4); //Saving code is only necessary if you would like to see the sobel output - if not then comment this section out
	imageBytes = imageLoader.getImageBytes();
	size = imageHeight * imageWidth * imageBytes;
	unsigned char* paddedImage = new unsigned char[size];
	paddedImage = imagePro.padOutImage(sobelData, imageWidth, imageHeight, imageBytes);
	imageLoader.saveImage("GrannySmith-Sobel.png", paddedImage, size);
	imageLoader.setImageBytes(1); //Image bytes is set back to 1 so that the rest of the program can continue
	imageBytes = imageLoader.getImageBytes();*/

	///////////////////////////////////
	// Apply Non-Maximum Suppression //
	///////////////////////////////////
	delete[] gaussianData;

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

	/////////////////////////////////
	// Pad Sides of Image and Save //
	/////////////////////////////////
	imageLoader.setImageBytes(4); //Saving code is only necessary if you would like to see the mask output
	imageBytes = imageLoader.getImageBytes();
	size = imageHeight * imageWidth * imageBytes;
	unsigned char* paddedImage = new unsigned char[size];
	paddedImage = imagePro.padOutImage(sobelData, imageWidth, imageHeight, imageBytes);
	imageLoader.saveImage("GrannySmith-Mask.png", paddedImage, size);

	//////////////////////
	// Colour Histogram //
	//////////////////////
	rawData = imageLoader.openImage("Apples/Granny-Smith.png");
	imageWidth = imageLoader.getImageWidth();
	imageHeight = imageLoader.getImageHeight();
	imageBytes = imageLoader.getImageBytes();
	size = imageWidth * imageHeight * imageBytes;
	imagePro.colourHistogram(rawData, imageWidth, imageHeight, imageBytes, sobelData, redHist, greenHist, blueHist);
		
	//imagePro.saveHistogram("Granny-Smith.txt", redHist, greenHist, blueHist); //Only Needed for Creating the Test Data Histograms

	//////////////////////////
	// Compare Loaded Apple //
	//////////////////////////
	std::string result = imagePro.compareHistogram(redHist, greenHist, blueHist, imageArray);

	delete[] sobelData;
	delete[] paddedImage;
	delete[] rawData;

	std::cin.get();

	return 0;
}



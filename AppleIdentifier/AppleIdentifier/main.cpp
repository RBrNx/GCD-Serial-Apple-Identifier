#include <cmath>
#include "imageIO.h"
#include "imageProcessor.h"

imageIO imageLoader;
imageProcessor imagePro;

unsigned char redHist[256];
unsigned char greenHist[256];
unsigned char blueHist[256];
std::string imageArray[14] = { "Apples/Courtland", "Apples/Braeburn", "Apples/Fuji", "Apples/Gala", "Apples/Ginger-Gold",
								"Apples/Golden-Delicious", "Apples/Granny-Smith", "Apples/Honeycrisp", "Apples/Jonagold",
								"Apples/Jonathan", "Apples/McIntosh", "Apples/Pacific-Rose", "Apples/Paula-Red", "Apples/Red-Delicious" };

int main() {
	for (int i = 0; i < 14; i++) {
		imageArray[i].append(".png");
		char* filename = new char[imageArray[i].length() + 1];
		strcpy(filename, imageArray[i].c_str());

		unsigned char* rawData = imageLoader.openImage(filename);
		int imageWidth = imageLoader.getImageWidth();
		int imageHeight = imageLoader.getImageHeight();
		int imageBytes = imageLoader.getImageBytes();
		int size = imageWidth * imageHeight * imageBytes;

		//imagePro.loadHistogram("SavedHist/test.txt", redHist, greenHist, blueHist);
		//imagePro.saveHistogram("SavedHist/test2.txt", redHist, greenHist, blueHist);
		//std::cin.get();

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

		//////////////////////
		// Colour Histogram //
		//////////////////////
		rawData = imageLoader.openImage(filename);
		imageWidth = imageLoader.getImageWidth();
		imageHeight = imageLoader.getImageHeight();
		imageBytes = imageLoader.getImageBytes();
		size = imageWidth * imageHeight * imageBytes;
		imagePro.colourHistogram(rawData, imageWidth, imageHeight, imageBytes, sobelData, redHist, greenHist, blueHist);
		imageArray[i].erase(imageArray[i].length() - 4);
		imageArray[i].append(".txt");
		filename = new char[imageArray[i].length() + 1];
		strcpy(filename, imageArray[i].c_str());
		imagePro.saveHistogram(filename, redHist, greenHist, blueHist);


		////////////////////////
		// Pad Sides of Image //
		////////////////////////
		imageLoader.setImageBytes(4);
		imageBytes = imageLoader.getImageBytes();
		size = imageHeight * imageWidth * imageBytes;
		unsigned char* paddedImage = imagePro.padOutImage(sobelData, imageWidth, imageHeight, imageBytes);

		imageArray[i].erase(imageArray[i].length() - 4);
		imageArray[i].append("-mask.png");
		filename = new char[imageArray[i].length() + 1];
		strcpy(filename, imageArray[i].c_str());
		imageLoader.saveImage(filename, paddedImage, size);

		delete[] sobelData;
		delete[] paddedImage;
		delete[] rawData;

		printf("%s \n", "");

		//std::cin.get();
	}

	return 0;
}



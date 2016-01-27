/********************************************************
*
*
*
*********************************************************/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "SLICsp.h"

using namespace cv;
using namespace std;

//define path and file name of image
#define FILE_NAME "d:\\lena.jpg"

//define input parameters
#define NUMBER_OF_SUPERPIXELS 4

//structure of cluster center
/*struct Cluster
{
	uchar l = 0;
	uchar a = 0;
	uchar b = 0;
	uint x = 0;
	uint y = 0;
};*/

int main()
{
	//k number of super pixels
	unsigned int k = NUMBER_OF_SUPERPIXELS;
	//original image
	Mat inputImage;
	//image in grayscale space
	Mat grayImage;
	//image in LAB space 
	Mat labImage;

	// Read the file
	inputImage = imread(FILE_NAME, IMREAD_COLOR); 

	// Check for invalid input
	if (!inputImage.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	SLICsp slic(inputImage, k);
	slic.initClusterCenter();

	for (unsigned int i = 0; i < slic.tabCluaster_->size(); i++)
	{
		circle(inputImage, Point(slic.tabCluaster_->at(i).x,
			slic.tabCluaster_->at(i).y), 5, Scalar(255, 0, 0));
	}

	// Create a window for display.
	namedWindow("Display window", WINDOW_AUTOSIZE); 
	// Show our image inside it.
	imshow("Display window", inputImage); 

	// Wait for a keystroke in the window
	waitKey(0); 
	return 0;
}
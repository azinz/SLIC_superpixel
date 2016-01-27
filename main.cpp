/********************************************************
*
*
*
*********************************************************/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include "SLICsp.h"

using namespace cv;
using namespace std;

//define path and file name of image
#define FILE_NAME "e:\\lena.jpg"

//define input parameters
#define NUMBER_OF_SUPERPIXELS 1200

//define number of iteration
#define NUMBER_OF_ITERATION 1

//define compacity
#define COMPACITY 35

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
	unsigned int m = COMPACITY;

	//original image
	Mat inputImage;
	//image in grayscale space
	Mat grayImage, grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y, grad;
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

	SLICsp slic(inputImage, k, m);
	slic.initClusterCenter();
	slic.moveClusterCenter();
	slic.initLabelDistancePixel();
	//slic.meanClaster();

	for (unsigned int i = 0; i < NUMBER_OF_ITERATION; i++)
	{
		slic.computeDistance();
	}

	
	for (unsigned int i = 0; i < slic.tabCluster_.size(); i++)
	{
		circle(inputImage, Point(slic.tabCluster_.at(i).x,
			slic.tabCluster_.at(i).y), 5, Scalar(255, 0, 255));
	}
	for (uint y = 0; y < slic.tabLabel_.rows; y++)
		for (uint x = 0; x < slic.tabLabel_.cols; x++)
		{
			if (slic.tabLabel_.at<int>(y, x) == -1)
			{
				circle(inputImage, Point(x,y), 0, Scalar(0, 0, 255));
			}
		
		}
	

	// Create a window for display.
	namedWindow("Display window", WINDOW_AUTOSIZE); 
	// Show our image inside it.
	imshow("Display window", inputImage); 

	// Wait for a keystroke in the window
	waitKey(0); 
	return 0;
}
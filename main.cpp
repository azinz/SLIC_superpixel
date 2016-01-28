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
#define FILE_NAME "d:\\alimentare.jpg"

//define input parameters
#define NUMBER_OF_SUPERPIXELS 1200

//define number of iteration
#define NUMBER_OF_ITERATION 5
//define compacity
#define COMPACITY 35

//#define SHOW

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

	for (unsigned int i = 0; i < NUMBER_OF_ITERATION; i++)
	{
		cout << "- COMPUTE " << i << endl;
		slic.computeDistance();
		cout << "- MEAN " << i << endl;
		slic.meanClaster();
		cout << "- END " << i << endl;
	}

	/*for (unsigned int i = 0; i < slic.tabCluster_.size(); i++)
	{
		circle(inputImage, Point(slic.tabCluster_.at(i).x,
			slic.tabCluster_.at(i).y), 5, Scalar(255, 0, 255));
	}*/
	/*for (int y = 0; y < slic.tabLabel_.rows; y++)
		for (int x = 0; x < slic.tabLabel_.cols; x++)
		{
			if (slic.tabLabel_.at<int>(y, x) == -1)
			{
				circle(inputImage, Point(x, y), 0, Scalar(slic.tabLabel_.at<int>(y, x)%255, 0, 255));
			}
		
		}*/

#ifdef SHOW
	for (int y = 0; y < slic.tabLabel_.rows; y++)
		for (int x = 0; x < slic.tabLabel_.cols; x++)
		{
	
		circle(inputImage, Point(x, y), 0, Scalar(255 / (slic.tabLabel_.at<int>(y, x) % 7 + 1), 0, 255 / (slic.tabLabel_.at<int>(y, x) % 20 + 1)));
		}
		
	cout << " TEST MAIN1" << endl;
	
#else
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	

	/// Draw contours
	Mat imgContour = Mat(slic.tabLabel_.size(), CV_8UC1);

	for (int y = 0; y < slic.tabLabel_.rows; y++)
		for (int x = 0; x < slic.tabLabel_.cols; x++)
		{
		if (slic.tabLabel_.at<int>(y, x) % 2 == 0)
			imgContour.at<uchar>(y, x) = 100;
		else
			imgContour.at<uchar>(y, x) = 255;
		}

	cout << " TEST MAIN2" << endl;
	/// Detect edges using canny
	Canny(imgContour, canny_output, 1, 1, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	cout << " TEST MAIN3" << endl;
	for (int i = 0; i< contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(inputImage, contours, i, Scalar(0, 0, 255), 1, 8, hierarchy, 0, Point());
	}
#endif	
	// Create a window for display.
	namedWindow("Display window", WINDOW_AUTOSIZE); 
	// Show our image inside it.
	imshow("Display window", inputImage); 

	// Wait for a keystroke in the window
	waitKey(0); 
	return 0;
}
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
#include "ClusterCenter.h"

using namespace cv;
using namespace std;

//define path and file name of image
#define FILE_NAME "e:\\lena.jpg"

//define input parameters
#define NUMBER_OF_SUPERPIXELS 1200

//structure of cluster center
/*struct Cluster
{
	uchar l = 0;
	uchar a = 0;
	uchar b = 0;
	uint x = 0;
	uint y = 0;
};*/

struct PixelSLIC
{
	int label = -1;
	uint distance = numeric_limits<uint>::max();
};

int main()
{
	//k number of super pixels
	uint k = NUMBER_OF_SUPERPIXELS;
	//original image
	Mat inputImage;
	//image in gray space
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

	//N number of pixel in image
	uint N = inputImage.total();

	//initialize array of labels and distances for each pixels
	vector<PixelSLIC> tabPixSLIC(N);

	//initialize array of cluaster centers
	vector<Cluster> tabCluaster(k);

	ClusterCenter claster(k);

	for (auto i : claster.tabCluaster_)
		cout << "CL " << i.a << endl;

	//calculate distance between cluster centers
	uint S = sqrt(N / k);

	cout << "PIXEL " << tabPixSLIC.size() << endl;
	cout << "Claster " << tabCluaster.size() << endl;
	cout << "SIZE IMAGE " << N << endl;
	cout << "S " << S << endl;

	// Create a window for display.
	namedWindow("Display window", WINDOW_AUTOSIZE); 
	// Show our image inside it.
	imshow("Display window", inputImage); 

	// Wait for a keystroke in the window
	waitKey(0); 
	return 0;
}
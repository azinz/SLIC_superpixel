/**
main.cpp

@brief     Implementation of SLIC algorithm

@details   
**************Warining******************
This code is writed under C++11 and OpenCV 3.0 lib

@author		Dumitru Loghin
			Azin Zareian

@date      25_01_2016
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

//including SLIC implemetation as class object
#include "SLICsp.h"

using namespace cv;
using namespace std;

/**************************************************************
 * Define section:
 * in order to configurate the SLIC algorithm
 *
 * MAIN configurations applied by defines:
 * 1. PARALLEL_COMPUTING // to allow to decrease time execution
 * 2. VIDEO_TREATING // to allow reading a video stream file
 * 3. FILE_NAME_PICTURE / FILE_NAME_VIDEO // Path file defines
 * 4. NUMBER_OF_SUPERPIXELS // number of super pixels
 * 5. NUMBER_OF_ITERATION // number of iteratations
 * 6. COMPACITY // compacity of color space disatance
 * ************************************************************/

//1.
//to enable/disable parallel computing
//please use the PARALLEL_COMPUTING define in defines.h
#include "defines.h"

//2.
//if VIDEO_TREATING directiver is defined then the main program will execute
//swich to reading the video file
//else  to reading the image file
//#define VIDEO_TREATING

//3.
//define path and file name of image
//if the VIDEO_TREATING	directive is undefined 
#define FILE_NAME_PICTURE "e:\\lena.jpg"

//define path and file name of video
//if the VIDEO_TREATING directive is defined 
#define FILE_NAME_VIDEO "e:\\MVI_9284.mov"

//4.
//define the numer of super pixels
#define NUMBER_OF_SUPERPIXELS 1200

//5.
//define the number of iteration
#define NUMBER_OF_ITERATION 5

//6.
//define compacity
#define COMPACITY 35


#ifdef VIDEO_TREATING
/**************************************************************
 * Main program:
 * Description: Shows the sequences of image frames from
 * the video file.
 * This segment of code is activated when the VIDEO_TREATING 
 * directive are defined.
 **************************************************************/
int main()
{
	//k number of super pixels
	unsigned int k = NUMBER_OF_SUPERPIXELS;
        //m compacity
	unsigned int m = COMPACITY;

        //using the opencv API lib in order to extact frames from vedio stream
        //with the sequences of frame from video file
	VideoCapture capture(FILE_NAME_VIDEO);
	Mat frame;

        // Check if the file is opened and ready to read
        // if an error occurred then exit
	if (!capture.isOpened())
	{
		cout << "Error when reading video file " << FILE_NAME_VIDEO << std::endl;
		return -1;
	}

	// Create a window for displaying
	namedWindow("Display window", WINDOW_AUTOSIZE);

        //main cycle of video streaming
	while (1)
	{
           //extraction the image frame from video file
		capture >> frame;

		cout << "*** START SLIC ALGORITHM ***" << endl;
		cout << "INTIALIZE CLUSTER CENTER ";

                //"std::chrono::steady_clock::now()"
                //is used to measure the time execution
                auto start = std::chrono::steady_clock::now();

                //instantiate the SLIC object algorithm 
                //by input paramters
                //frame image, k and m
		SLICsp slic(frame, k, m);

                //initialization cluaster center
		slic.initClusterCenter();

                //move the cluster center
		slic.moveClusterCenter();

                //initialize the distance tabel
                //and label tabel
		slic.initLabelDistancePixel();

		auto end = std::chrono::steady_clock::now();
		cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;

                //repeat the computing super pixel "NUMBER_OF_ITERATION" times
		for (unsigned int i = 0; i < NUMBER_OF_ITERATION; i++)
		{
			cout << "- COMPUTE DISTANCE " << i;
			start = std::chrono::steady_clock::now();

                        //compute the distance
			slic.computeDistance();

			end = std::chrono::steady_clock::now();
			cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
			cout << "- MEAN CLASTER " << i;
			start = std::chrono::steady_clock::now();

                        //calculate the cluster means
			slic.meanClaster();
			end = std::chrono::steady_clock::now();

			cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
			cout << "- END " << i << endl << endl;
		}

		cout << "*** DRAW THE CONTOUR ***";
		start = std::chrono::steady_clock::now();

                //extraction the image with the contour as a super pixel
		Mat img = slic.getSegmentation();

		end = std::chrono::steady_clock::now();
		cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
		cout << " --------- SHOW THE SUPER PIXELS --------- " << endl << endl;
		
                // Show the treated image inside the window
		imshow("Display window", img);

                //waiting for pressing 'esc' key for 1 ms. If 'esc' key is pressed, break loop 
		if (waitKey(1) == 27) 
		{
			break;
		}
	}
	// Waiting for a keystroke in the window 
	waitKey(0); 
	return 0;
}

#else//VIDEO_TREATING
/**************************************************************
 * Main program:
 * Description: Shows the image from the image file.
 * This segment of code is activated when the VIDEO_TREATING                                  
 * directive are not defined.
 **************************************************************/
int main()
{
	//k number of the super pixels
	unsigned int k = NUMBER_OF_SUPERPIXELS;
        //m compacity
	unsigned int m = COMPACITY;

	//original image
	Mat inputImage;

	// Read the file
	inputImage = imread(FILE_NAME_PICTURE, IMREAD_COLOR); 

        // Check if the file is opened and ready to read
        // if an error occurred then exit
	if (!inputImage.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	
	cout << "*** START SLIC ALGORITHM ***" << endl;
	cout << "INTIALIZE CLUSTER CENTER ";

        //"std::chrono::steady_clock::now()"
        //is used to measure the time execution
	auto start = std::chrono::steady_clock::now();

        //instantiate the SLIC object algorithm
        //by input paramters
        //frame image, k and m
	SLICsp slic(inputImage, k, m);

        //initialization cluaster center
	slic.initClusterCenter();

        //move the cluster center
	slic.moveClusterCenter();

        //initialize the distance tabel
        //and label tabel
	slic.initLabelDistancePixel();

	auto end = std::chrono::steady_clock::now();
	cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;

        //repeat the computing super pixel "NUMBER_OF_ITERATION" times
	for (unsigned int i = 0; i < NUMBER_OF_ITERATION; i++)
	{
		cout << "- COMPUTE DISTANCE " << i;
		start = std::chrono::steady_clock::now();

                //compute the distance
		slic.computeDistance();

		end = std::chrono::steady_clock::now();
		cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
		cout << "- MEAN CLASTER " << i;

		start = std::chrono::steady_clock::now();

                //calculate the cluster means
		slic.meanClaster();

		end = std::chrono::steady_clock::now();
		cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
		cout << "- END " << i << endl << endl;
	}

	cout << "*** DRAW THE CONTOUR ***";
	start = std::chrono::steady_clock::now();

        //extraction the image with the contour as a super pixel
	Mat img = slic.getSegmentation();

	end = std::chrono::steady_clock::now();
	cout << " t = " << std::chrono::duration <double, std::milli>(end - start).count() << " ms" << endl;
	cout << " --------- SHOW THE SUPER PIXELS --------- " << endl << endl;

	// Create a window for display.
	namedWindow("Display window", WINDOW_AUTOSIZE);
        // Show the treated image inside the window
	imshow("Display window", img);

	// waiting for a keystroke in the window 
	waitKey(0); 
	return 0;
}
#endif//VIDEO_TREATING


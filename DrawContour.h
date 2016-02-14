#ifndef DRAW_CONTOUR
#define DRAW_CONTOUR

/**
 DrawContour.h

 @brief     Draw the contour of super pixel which were soterd
            in label tabel

 @details   The algorithm of drawing contour is based on finding the difference
            between two adjacented pixels. The scaning process is being done
            verticaly and horizontaly both. This code is implemented to applay
            the parallel computing. Plsease refer to the file "defines.h"
            **************Warining******************
               This code is writed under C++11 and OpenCV 3.0 lib

 @author    Dumitru Loghin
            Azin Zareian

 @date      25_01_2016
*/

#include "defines.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <future>

#include <iostream>
using namespace std;
using namespace cv;

#ifdef PARALLEL_COMPUTING
/**************************************************************
 * This part of code is activated by PARALLEL_COMPUTING define
 *
 **************************************************************/

/**
  @brief Draw the contour by horizontal scaning
  @param label table, output image
  @return void
*/
void HorizontalDraw(Mat &tempImg_, Mat &drawImgOut)
{
	for (int y = 0; y < tempImg_.rows; y++)
	{
                //cycle on x until number of columns -1
                //to avoid the overboard checking the adjacent pixel
		for (int x = 0; x < tempImg_.cols - 1; x++)
		{
                        //label extraction from the position x,y
			int label = tempImg_.at<int>(y, x);
                        //check if the label is differnet from the next label X+1
                        //if yes then a pixel will be stored as a BLUE color in
                        //output image
                        if (tempImg_.at<int>(y, x + 1) != label)
			{
				drawImgOut.at<Vec3b>(y, x)[0] = 255;
				drawImgOut.at<Vec3b>(y, x)[1] = 0;
				drawImgOut.at<Vec3b>(y, x)[2] = 0;
			}
		}
	}
};

/**
  @brief Draw the contour by vertical scaning
  @param label table, output image
  @return void
*/
void VerticalDraw(Mat &tempImg_, Mat &drawImgOut)
{
        //cycle on x until number of rows -1
        //to avoid the overboard checking the adjacent pixel
	for (int y = 0; y < tempImg_.rows - 1; y++)
	{
		for (int x = 0; x < tempImg_.cols; x++)
		{
                        //label extraction from the position x,y
			int label = tempImg_.at<int>(y, x);
                        //check if the label is differnet from the next label Y+1
                        //if yes then a pixel will be stored as a BLUE color in
                        //output image
                        if (tempImg_.at<int>(y + 1, x) != label)
			{
				drawImgOut.at<Vec3b>(y, x)[0] = 255;
				drawImgOut.at<Vec3b>(y, x)[1] = 0;
				drawImgOut.at<Vec3b>(y, x)[2] = 0;
			}
		}
	}
};

/**
  @brief    The class DrawContour provide
            the algorithm of contour draw by parallel mechanism
*/
class DrawContour
{
public:
    /**
      @brief Constructor
      @param label table, output image
    */
        DrawContour(Mat &labelImg, Mat &drawImgOut)
        {
            //instance the vector of the future tasks
                vector<future<void>> futures;
                //preparing the tasks which will be executed in parallel mode
                //Draw the contour task in horizontal way
                futures.push_back(async(HorizontalDraw, labelImg, drawImgOut));
                //Draw the contour task in vertical way
                futures.push_back(async(VerticalDraw, labelImg, drawImgOut));

                //mechanism of execution the task in parallel mode
                for (auto &task : futures){ task.get(); }
        }
};

#else//PARALLEL_COMPUTING
/**************************************************************
 * This part of code is activated when PARALLEL_COMPUTING
 * directive is not defined
 **************************************************************/

/**
  @brief    The class DrawContour provide
            the algorithm of contour draw in sequencely mode
*/
class DrawContour
{
public:
    /**
      @brief Constructor
      @param label table, output image
    */
	DrawContour(Mat &labelImg, Mat &drawImgOut)
	{		
            //storing the table of labels in temporal table
		tempImg_ = labelImg;
                //storing the size of table
		rows_ = tempImg_.rows;
		cols_ = tempImg_.cols;
		
                //contour drawing algorithme
                //Draw the contour by horizontal scaning
		for (int y = 0; y < rows_; y++)
		{
                    //cycle on x until number of columns -1
                    //to avoid the overboard checking the adjacent pixel
			for (int x = 0; x < cols_ - 1; x++)
			{
                            //label extraction from the position x,y
				int label = tempImg_.at<int>(y, x);
                                //check if the label is differnet from the next label X+1
                                //if yes then a pixel will be stored as a BLUE color in
                                //output image
                                if (tempImg_.at<int>(y, x + 1) != label)
				{
					drawImgOut.at<Vec3b>(y, x)[0] = 255;
					drawImgOut.at<Vec3b>(y, x)[1] = 0;
					drawImgOut.at<Vec3b>(y, x)[2] = 0;
				}
			}
		}

                //Draw the contour by vertical scaning
                //cycle on x until number of rows -1
                //to avoid the overboard checking the adjacent pixel
		for (int y = 0; y < rows_ - 1; y++)
		{
			for (int x = 0; x < cols_; x++)
			{
                            //label extraction from the position x,y
				int label = tempImg_.at<int>(y, x);
                                //check if the label is differnet from the next label Y+1
                                //if yes then a pixel will be stored as BLUE color in
                                //output image
                                if (tempImg_.at<int>(y + 1, x) != label)
				{
					drawImgOut.at<Vec3b>(y, x)[0] = 255;
					drawImgOut.at<Vec3b>(y, x)[1] = 0;
					drawImgOut.at<Vec3b>(y, x)[2] = 0;
				}
			}
		}
	}

private:
        //attributes
	Mat tempImg_;
	int cols_;
	int rows_;
};
#endif//PARALLEL_COMPUTING

#endif//DRAW_CONTOUR

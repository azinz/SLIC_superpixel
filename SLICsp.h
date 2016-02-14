/**
 SLICsp.h

 @brief     Segemtation the image by applying the simple linear iterative clustering (SLIC) algorithm

 @details   This code provide the class "SLICsp" which perfom the segemtation.
            Parallel computing is available by definding the PARALLEL_COMPUTING directive
            **************Warining******************
            This code is writed under C++11 and OpenCV 3.0 lib

 @ref       "SLIC Superpixels Compared to State-of-the-Art Superpixel Methods"
            Radhakrishna Achanta and al.
            IEEE TRANSACTIONS ON PATTERN ANALYSIS AND MACHINE INTELLIGENCE

 @author    Dumitru Loghin
            Azin Zareian

 @date      25_01_2016
*/

#ifndef SLICSP
#define SLICSP

#include "defines.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;


/**
  @brief The data structure of cluster center
         LAB color space
         XY coordinates
*/
struct LABxy
{
        //LAB color space
	int l = 0;
	int a = 0;
	int b = 0;
        //XY coordinates
	int x = 0;
	int y = 0;
	unsigned int count = 0;
};//LABxy

/**
  @brief    The Class which perform the segemtation of image by applying the
            simple linear iterative clustering (SLIC) algorithm
*/
class SLICsp
{
public:
        /**
          @brief constructor
          @param input original image, number of clusters and compacity
        */
	SLICsp(const Mat &inputImg, int k, int m);

        /**
          @brief destructor
        */
	~SLICsp();

        /**
          @brief initializing the cluster center
          @param void
          @return void
        */
	void initClusterCenter();

        /**
          @brief move the cluster to the lowest gradient
          @param void
          @return void
        */
	void moveClusterCenter();

        /**
          @brief initialize the tabels of distances and labels
          @param void
          @return void
        */
	void initLabelDistancePixel();

        /**
          @brief compute the distance and assigning
          @param void
          @return void
        */
	void computeDistance();

        /**
          @brief calculate the mean of the cluster region
          @param void
          @return void
        */
	void meanClaster();

        /**
          @brief draw the contour over the image
          @param void
          @return the copy of original image with the contour of segemtation
        */
	Mat getSegmentation();

        /**
          @brief get a distance of clusters
          @param void
          @return the distance between the clusters
        */
	int getS()const;

        /**
          @brief get a number of pixels in image
          @param void
          @return number of pixels in image
        */
	int getN()const;

        /**
          @brief get a required number of super pixel
          @param void
          @return number of super pixel
        */
	int getK()const;

        /**
          @brief get the compacity
          @param void
          @return compacity
        */
	int getM()const;

private:
        /**
          @brief place the cluster in the lowest gradient in 3x3 region
          @param input gradient image, input by alias the cluster center data structure 
          @return void
        */
	void findMinimalValue(const Mat &inputImage, LABxy &clusterCenter);

        /**
          @brief release the resource
          @param void
          @return void
        */
	void releaseResource();

        /**
          @brief clear the cluster table
          @param void
          @return void
        */
	void releaseTabCluster();

        /**
          @brief clear the distance and tables label
          @param void
          @return void
        */
	void releaseTabLabelDistance();

        //Attributes
        //table of clusters
        vector<LABxy> tabCluster_;
        //table of labels
	Mat tabLabel_;
        //table of distances
	Mat tabDistance_;
        //image in LAB color space
	Mat labSpaceImage_;
        //alias to original image
	const Mat &inputImage_;
        //number of super pixel
	int k_;
        //compacity
	int m_;
        //distance between clusters
	int S_;
};

#endif//SLICSP

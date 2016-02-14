/**
 GradientImg.h

 @brief     Calculate the gradient of an image by Sobel method

 @details   This code provide the gradient treatment an image.
            The class GradientImg is implemented as a function
            by a static method "calculate"
            **************Warining******************
            This code is writed under C++11 and OpenCV 3.0 lib

 @ref       http://docs.opencv.org/2.4/doc/tutorials/imgproc/imgtrans/sobel_derivatives/sobel_derivatives.html

 @author    Dumitru Loghin
            Azin Zareian

 @date      25_01_2016
*/

#ifndef GRADIENTIMG
#define GRADIENTIMG
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class GradientImg
{
public:
    /**
      @brief calculate the gradient image
      @param input original image, output gradient image
      @return void
    */
	static void calculate(const Mat &inputImage, Mat &outputImageGrad)
	{		
            //Decalaration of work variabels
		Mat blurImgare, grayImage, grad_x, grad_y, abs_grad_x, abs_grad_y;

                //Pass the input image through the Gaussian convolution by 3x3 kernel
		GaussianBlur(inputImage, blurImgare, Size(3, 3), 0, 0, BORDER_DEFAULT);

        //converting the blured image to image of grayscale color space
		cvtColor(blurImgare, grayImage, CV_BGR2GRAY);

                //treat the grayscale image by sobel derivative in horizontal direction
                Sobel(grayImage,    //input grayscale image
                      grad_x,       //output results
                      CV_16S,       //CV_16S depth of the output image
                      1,            //The order of the derivative in x direction
                      0,3,1,0,BORDER_DEFAULT//default values
                      );
                //convert the partial results to CV_8U depth
		convertScaleAbs(grad_x, abs_grad_x);

                //treat the grayscale image by sobel derivative in vertical direction
                Sobel(grayImage,    //input grayscale image
                      grad_y,       //output results
                      CV_16S,       //CV_16S depth of the output image
                      0,            //The order of the derivative in y direction
                      1,3,1,0,BORDER_DEFAULT//default values
                      );

                //convert the partial results to CV_8U depth
		convertScaleAbs(grad_y, abs_grad_y);

                //mixing the gradients to one output results
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, outputImageGrad);
	}
};

#endif//GRADIENTIMG

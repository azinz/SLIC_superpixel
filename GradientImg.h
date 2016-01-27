#ifndef GRADIENTIMG
#define GRADIENTIMG
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class GradientImg
{
public:
	static void calculate(Mat inputImage, Mat &outputImageGrad)
	{		
		Mat grayImage, grad_x, grad_y, abs_grad_x, abs_grad_y;

		GaussianBlur(inputImage, inputImage, Size(3, 3), 0, 0, BORDER_DEFAULT);
		
		cvtColor(inputImage, grayImage, CV_BGR2GRAY);
		Sobel(grayImage, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(grad_x, abs_grad_x);

		/// Gradient Y		
		Sobel(grayImage, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(grad_y, abs_grad_y);

		/// Total Gradient (approximate)
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, outputImageGrad);
	}
};

#endif
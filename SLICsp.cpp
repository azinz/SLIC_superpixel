#include "SLICsp.h"
#include "GradientImg.h"
#include <cmath>

SLICsp::SLICsp(const Mat &inputImg, uint k, uint m)
	: inputImage_(inputImg),
	k_(k),
	m_(m),
	S_((unsigned int)sqrt(getN() / getK()))
{
	cvtColor(inputImg, labSpaceImage_, CV_BGR2Lab);
}

SLICsp::~SLICsp()
{
	releaseResource();
}

void SLICsp::initClusterCenter()
{
	releaseTabCluster();

	unsigned int cX = getS() / 2;
	unsigned int cY = getS() / 2;

	while (1)
	{
		if ((int)cX >= inputImage_.cols)
		{
			cX = getS() / 2;
			cY += getS();
			if ((int)cY >= inputImage_.rows) 
				break;
		}
		
		LABxy cluster;
		cluster.x = cX;
		cluster.y = cY;
		tabCluster_.push_back(cluster);
		cX += getS();
	}
}

void SLICsp::moveClusterCenter()
{
	Mat gradImage;
	GradientImg::calculate(/*labSpaceImage_*/inputImage_, gradImage);
	for (uint i = 0; i < tabCluster_.size(); i++)
		findMinimalValue(gradImage, tabCluster_.at(i));
	// Create a window for display.
	//namedWindow("Display window1", WINDOW_AUTOSIZE);
	// Show our image inside it.
	//imshow("Display window1", gradImage);


}

void SLICsp::initLabelDistancePixel()
{
	releaseTabLabelDistance();
	tabLabel_ = Mat(inputImage_.rows, inputImage_.cols,
						CV_32S,
						Scalar(-1));

	tabDistance_ = Mat(inputImage_.rows, inputImage_.cols,
						CV_16U, 
						Scalar(numeric_limits<unsigned short>::max()));


/*	for (int i = 0; i < getN(); i++)
		cout << "L " << i << " : " << (int)tabLabel_.at<int>(i)
		<< " D " << (int)tabDistance_.at<unsigned short>(i) << endl;*/
}

void SLICsp::computeDistance()
{
	for (uint i = 0; i < tabCluster_.size(); i++)
	{
		uint x0 = (int)tabCluster_.at(i).x - (int)getS() < 0 ? 0 : tabCluster_.at(i).x - getS();
		uint y0 = (int)tabCluster_.at(i).y - (int)getS() < 0 ? 0 : tabCluster_.at(i).y - getS();

		uint x1 = (int)tabCluster_.at(i).x + getS() >= labSpaceImage_.cols ? labSpaceImage_.cols - 1 : tabCluster_.at(i).x + getS();
		uint y1 = (int)tabCluster_.at(i).y + getS() >= labSpaceImage_.rows ? labSpaceImage_.rows - 1 : tabCluster_.at(i).y + getS();

		for (uint y = y0; y < y1; y++)
			for (uint x = x0; x < x1; x++)
			{		
				LABxy pixel;
				pixel.x = x;
				pixel.y = y;
				
				pixel.l = labSpaceImage_.at<Vec3b>(y, x)[0];
				pixel.a = labSpaceImage_.at<Vec3b>(y, x)[1];
				pixel.b = labSpaceImage_.at<Vec3b>(y, x)[2];

				uint D = distance(tabCluster_[i], pixel, getM(), getS());

				if ((int)D < tabDistance_.at<ushort>(y, x))
				{					
					tabDistance_.at<ushort>(y, x) = (ushort)D;
					tabLabel_.at<int>(y, x) = i;
				}

				if (tabLabel_.at<int>(y, x) == -1)
				{
					cout << "X " << x << " y " << y << " D " << D << endl;
				}
			}
	}	
	/*for (uint y = 0; y < labSpaceImage_.rows; y++)
		for (uint x = 0; x < labSpaceImage_.cols; x++)
		{
			cout << x << " " << y << " " << tabLabel_.at<int>(y, x) << endl;
		}*/
}

void SLICsp::meanClaster()
{
/*	for (uint y = 0; y < labSpaceImage_.rows; y++)
		for (uint x = 0; x < labSpaceImage_.cols; x++)
		{
			cout << x << " " << y << " " << tabLabel_.at<int>(y, x) << endl;
		}
		*/
/*	for (uint i = 0; i < tabCluster_.size(); i++)
	{
		LABxy meanClaster;
		for (uint y = 0; y < labSpaceImage_.rows; y++)
			for (uint x = 0; x < labSpaceImage_.cols; x++)
			{
				if (tabLabel_.at<int>(y, x) == i)
				{
					meanClaster.x += x;
					meanClaster.y += y;
					meanClaster.l = labSpaceImage_.at<Vec3b>(y, x)[0];
					meanClaster.a = labSpaceImage_.at<Vec3b>(y, x)[1];
					meanClaster.b = labSpaceImage_.at<Vec3b>(y, x)[2];
				}
			}
	}*/
}

uint SLICsp::getS()const
{
	return S_;
}

uint SLICsp::getN()const
{
	return (uint)inputImage_.total();
}

uint SLICsp::getK()const
{
	return k_;
}

uint SLICsp::getM()const
{
	return m_;
}

void SLICsp::findMinimalValue(const Mat &inputImage, LABxy &clusterCenter)
{
	int cX = clusterCenter.x;
	int cY = clusterCenter.y;
	uint halfS = getS() / 2;
	uint x0 = (int)clusterCenter.x - 1 < 0 ? 0 : clusterCenter.x - 1;
	uint y0 = (int)clusterCenter.y - 1 < 0 ? 0 : clusterCenter.y - 1;

	uint x1 = (int)clusterCenter.x + 1 >= inputImage.cols ? inputImage.cols - 1 : clusterCenter.x + 1;
	uint y1 = (int)clusterCenter.y + 1 >= inputImage.rows ? inputImage.rows - 1 : clusterCenter.y + 1;

	uchar minValue = numeric_limits<uchar>::max();

	for (uint y = y0; y <= y1; y++)
		for (uint x = x0; x <= x1; x++)
			if (cX - halfS < x && x < cX + halfS &&
				cY - halfS < y && y < cY + halfS &&
				inputImage.at<uchar>(y, x) < minValue)
			{
				minValue = inputImage.at<uchar>(y, x);
				clusterCenter.x = x;
				clusterCenter.y = y;
			}
}

uint SLICsp::distance(const LABxy &clusterCenter, const LABxy &pixel, double m, uint S)
{
	double Dc = sqrt(
		pow(clusterCenter.l - pixel.l, 2)
		+ pow(clusterCenter.a - pixel.a, 2)
		+ pow(clusterCenter.b - pixel.b, 2));

	double Ds = sqrt(
		pow(clusterCenter.x - pixel.x, 2)
		+ pow(clusterCenter.y - pixel.y, 2));

	return (uint)sqrt(pow(Dc / m, 2) + pow(Ds / S, 2));
}

void SLICsp::releaseResource()
{
	releaseTabCluster();
	releaseTabLabelDistance();
}

void SLICsp::releaseTabCluster()
{
	tabCluster_.clear();
}

void SLICsp::releaseTabLabelDistance()
{
	tabLabel_.release();
	tabDistance_.release();
}
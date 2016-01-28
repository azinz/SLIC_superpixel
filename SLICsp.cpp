#include "SLICsp.h"
#include "GradientImg.h"
#include <cmath>

SLICsp::SLICsp(const Mat &inputImg, int k, int m)
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

	int cX = getS() / 2;
	int cY = getS() / 2;

	while (1)
	{
		if (cX >= inputImage_.cols)
		{
			cX = getS() / 2;
			cY += getS();
			if (cY >= inputImage_.rows) 
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
	for (int i = 0; i < (int)tabCluster_.size(); i++)
	{
		findMinimalValue(gradImage, tabCluster_.at(i));
		tabCluster_.at(i).l = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[0];
		tabCluster_.at(i).a = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[1];
		tabCluster_.at(i).b = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[2];
	}
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
						CV_32F,
						Scalar(numeric_limits<float>::max()));


/*	for (int i = 0; i < getN(); i++)
		cout << "L " << i << " : " << (int)tabLabel_.at<int>(i)
		<< " D " << (int)tabDistance_.at<unsigned short>(i) << endl;*/
}

void SLICsp::computeDistance()
{	
	for (int i = 0; i < (int)tabCluster_.size(); i++)
	{
		int x0 = tabCluster_.at(i).x - getS() < 0 ? 0 : tabCluster_.at(i).x - getS();
		int y0 = tabCluster_.at(i).y - getS() < 0 ? 0 : tabCluster_.at(i).y - getS();

		int x1 = tabCluster_.at(i).x + getS() >= labSpaceImage_.cols ? labSpaceImage_.cols - 1 : tabCluster_.at(i).x + getS();
		int y1 = tabCluster_.at(i).y + getS() >= labSpaceImage_.rows ? labSpaceImage_.rows - 1 : tabCluster_.at(i).y + getS();

		for (int y = y0; y <= y1; y++)
		{
			for (int x = x0; x <= x1; x++)
			{
				LABxy pixel;
				pixel.x = x;
				pixel.y = y;

				pixel.l = labSpaceImage_.at<Vec3b>(y, x)[0];
				pixel.a = labSpaceImage_.at<Vec3b>(y, x)[1];
				pixel.b = labSpaceImage_.at<Vec3b>(y, x)[2];

				float D = distance(tabCluster_[i], pixel, getM(), getS());

				if (D <= tabDistance_.at<float>(y, x))
				{
					tabDistance_.at<float>(y, x) = D;
					tabLabel_.at<int>(y, x) = i;
				}
			}
		}
	}	
	cout << "COMPUTE END" << endl;
}

void SLICsp::meanClaster()
{
	//need??? initlize
	cout << " mean test " << endl;
	for (int i = 0; i < tabCluster_.size(); i++)
		tabCluster_[i].count = 0;//LABxy();

	cout << " mean test 1" << endl;
	for (int y = 0; y < labSpaceImage_.rows; y++)
	{
		for (int x = 0; x < labSpaceImage_.cols; x++)
		{
			int i = tabLabel_.at<int>(y, x);
			tabCluster_[i].x += x;
			tabCluster_[i].y += y;
			tabCluster_[i].l += labSpaceImage_.at<Vec3b>(y, x)[0];
			tabCluster_[i].a += labSpaceImage_.at<Vec3b>(y, x)[1];
			tabCluster_[i].b += labSpaceImage_.at<Vec3b>(y, x)[2];
			tabCluster_[i].count++;
		}
	}

	cout << " mean test 2 -- size " << tabCluster_.size() << endl;
	
	for (int i = 0; i < tabCluster_.size(); i++)
	{
		if (tabCluster_[i].count > 0)
		{
			tabCluster_[i].x /= tabCluster_[i].count;
			tabCluster_[i].y /= tabCluster_[i].count;
			tabCluster_[i].l /= tabCluster_[i].count;
			tabCluster_[i].a /= tabCluster_[i].count;
			tabCluster_[i].b /= tabCluster_[i].count;
		}
	}

	cout << "mean end inside" << endl;
}

int SLICsp::getS()const
{
	return S_;
}

int SLICsp::getN()const
{
	return inputImage_.total();
}

int SLICsp::getK()const
{
	return k_;
}

int SLICsp::getM()const
{
	return m_;
}

void SLICsp::findMinimalValue(const Mat &inputImage, LABxy &clusterCenter)
{
	int cX = clusterCenter.x;
	int cY = clusterCenter.y;
	int halfS = getS() / 2;
	int x0 = clusterCenter.x - 1 < 0 ? 0 : clusterCenter.x - 1;
	int y0 = clusterCenter.y - 1 < 0 ? 0 : clusterCenter.y - 1;

	int x1 = clusterCenter.x + 1 >= inputImage.cols ? inputImage.cols - 1 : clusterCenter.x + 1;
	int y1 = clusterCenter.y + 1 >= inputImage.rows ? inputImage.rows - 1 : clusterCenter.y + 1;

	uchar minValue = numeric_limits<uchar>::max();

	for (int y = y0; y <= y1; y++)
		for (int x = x0; x <= x1; x++)
			if (cX - halfS < x && x < cX + halfS &&
				cY - halfS < y && y < cY + halfS &&
				inputImage.at<uchar>(y, x) < minValue)
			{
				minValue = inputImage.at<uchar>(y, x);
				clusterCenter.x = x;
				clusterCenter.y = y;
			}
}

float SLICsp::distance(const LABxy &clusterCenter, const LABxy &pixel, int m, int S)
{
	float Dc = sqrtf(
		powf(clusterCenter.l - pixel.l, 2)
		+ powf(clusterCenter.a - pixel.a, 2)
		+ powf(clusterCenter.b - pixel.b, 2));

	float Ds = sqrtf(
		powf(clusterCenter.x - pixel.x, 2)
		+ powf(clusterCenter.y - pixel.y, 2));

	return sqrtf(powf(Dc / m, 2) + powf(Ds / S, 2));
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
#include "SLICsp.h"
#include "GradientImg.h"
#include "DrawContour.h"
#include <cmath>
#include <future>

/**
	@brief calculate distance of euclidean space and LAB color space between two pixel
	@param pixel of cluster center, pixel from ROI, compacity (m) and cluster distance (S)
	@return the distance
*/
float distance(const LABxy &clusterCenter, const LABxy &pixel, int m, int S)
{
	//color distance
	float Dc = sqrtf(
		powf(clusterCenter.l - pixel.l, 2)
		+ powf(clusterCenter.a - pixel.a, 2)
		+ powf(clusterCenter.b - pixel.b, 2));

	//euclidean space distance
	float Ds = sqrtf(
		powf(clusterCenter.x - pixel.x, 2)
		+ powf(clusterCenter.y - pixel.y, 2));

	//combine both distance in one general
	return sqrtf(powf(Dc / m, 2) + powf(Ds / S, 2));
}

SLICsp::SLICsp(const Mat &inputImg, int k, int m)
	: 
	inputImage_(inputImg),
	k_(k),
	m_(m),
	S_((unsigned int)sqrt(getN() / getK()))
{
    //converting the BGR image to LAB image
	cvtColor(inputImg, labSpaceImage_, CV_BGR2Lab);
}

SLICsp::~SLICsp()
{
    //before destruction
    //release memmory
	releaseResource();
}

void SLICsp::initClusterCenter()
{
    //clear the table of clusters
	releaseTabCluster();

        //finding the initiale position of first cluster
	int cX = getS() / 2;
	int cY = getS() / 2;

        //fill up the table of cluster in concordance with the distance S
        //the cycle will break when the image will be entier covered by clusters
	while (1)
	{
            //check if the line was filled up with clusters
            //if yes then we pass to the next line
		if (cX >= inputImage_.cols)
		{
			cX = getS() / 2;
			cY += getS();
                        //check if all lines was filled up
                        //if yes then we exit from this while cycle
			if (cY >= inputImage_.rows) 
				break;
		}
		
                //fill up the tabel
                //by positioning the cluster according to the cX and cY position
		LABxy cluster;
		cluster.x = cX;
		cluster.y = cY;
                //push the cluster in tabel
		tabCluster_.push_back(cluster);
                //pass to next postion cluster
		cX += getS();
	}
}

void SLICsp::moveClusterCenter()
{
    //prepare a matrix image for future storing a gradient image
	Mat gradImage;

        //calculate gradient by Sobel method
	GradientImg::calculate(inputImage_, gradImage);

        //repositioning the cluaster to the lowest gradient in 3x3 region of the cluster center
	for (int i = 0; i < (int)tabCluster_.size(); i++)
	{
            //find the position by lowest gradient
		findMinimalValue(gradImage, tabCluster_.at(i));

                //attributing the LAB color from this new position
		tabCluster_.at(i).l = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[0];
		tabCluster_.at(i).a = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[1];
		tabCluster_.at(i).b = labSpaceImage_
			.at<Vec3b>(tabCluster_.at(i).y, tabCluster_.at(i).x)[2];
	}
}

void SLICsp::initLabelDistancePixel()
{
    //clear the tables of distances and labels
	releaseTabLabelDistance();
        //initialize the table of labels with -1
	tabLabel_ = Mat(inputImage_.rows, inputImage_.cols,
						CV_32S,
						Scalar(-1));

        //initialize the table of distance with max number of float data type(3.402823e+38)
	tabDistance_ = Mat(inputImage_.rows, inputImage_.cols,
						CV_32F,
						Scalar(numeric_limits<float>::max()));

}

#ifdef PARALLEL_COMPUTING

void asynchronTask(
	int i, LABxy &cluaster, 
	Mat &labSpaceImage_, 
	Mat &tabDistance_, 
	Mat &tabLabel_, 
	int S, int m, int k)
{
	int x0 = cluaster.x - S < 0 ? 0 : cluaster.x - S;
	int y0 = cluaster.y - S < 0 ? 0 : cluaster.y - S;

	int x1 = cluaster.x + S >= labSpaceImage_.cols ? labSpaceImage_.cols - 1 : cluaster.x + S;
	int y1 = cluaster.y + S >= labSpaceImage_.rows ? labSpaceImage_.rows - 1 : cluaster.y + S;
	

	for (int y = y0; y <= y1; y++)
	{
		float *ptr_dist = tabDistance_.ptr<float>(y);
		int *ptr_label = tabLabel_.ptr<int>(y);

		for (int x = x0; x <= x1; x++)
		{
			LABxy pixel;
			pixel.x = x;
			pixel.y = y;

			pixel.l = labSpaceImage_.at<Vec3b>(y, x)[0];
			pixel.a = labSpaceImage_.at<Vec3b>(y, x)[1];
			pixel.b = labSpaceImage_.at<Vec3b>(y, x)[2];

			float D = distance(cluaster, pixel, m, S);

			if (D <= ptr_dist[x])
			{
				ptr_dist[x] = D;
				ptr_label[x] = i;
			}
		}
	}
}

#endif

void SLICsp::computeDistance()
{
#ifdef PARALLEL_COMPUTING
	vector<future<void>> futures;
	for (int i = 0; i < (int)tabCluster_.size(); i++)
	{
		futures.push_back(async(asynchronTask, 
			i,
			tabCluster_.at(i),
			labSpaceImage_,
			tabDistance_,
			tabLabel_,
			getS(),
			getM(),
			getK()));
	}

	for (auto &task : futures) 
	{
		task.get();
	}
#else
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
#endif
	
}

void SLICsp::meanClaster()
{
	for (int i = 0; i < tabCluster_.size(); i++)
		tabCluster_[i] = LABxy();

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
}

Mat SLICsp::getSegmentation()
{
	Mat segImg = inputImage_;
	inputImage_.copyTo(segImg);

	DrawContour::DrawContour(tabLabel_, segImg);

	return segImg;
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
	{
		for (int x = x0; x <= x1; x++)
		{
			if (cX - halfS < x && x < cX + halfS &&
				cY - halfS < y && y < cY + halfS &&
				inputImage.at<uchar>(y, x) < minValue)
			{
				minValue = inputImage.at<uchar>(y, x);
				clusterCenter.x = x;
				clusterCenter.y = y;
			}
		}
	}
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

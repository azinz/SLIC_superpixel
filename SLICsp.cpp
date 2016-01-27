#include "SLICsp.h"

SLICsp::SLICsp(const Mat &inputImg, unsigned int k)
	: imputImage_(inputImg),
	k_(k),
	tabCluaster_(nullptr),
	S_((unsigned int)sqrt(getN() / getK()))
{

}

SLICsp::~SLICsp()
{
	releaseResource();
}

void SLICsp::initClusterCenter()
{
	releaseTabCluster();
	tabCluaster_ = new vector<Cluster>();

	unsigned int cX = getS() / 2;
	unsigned int cY = getS() / 2;

	while (1)
	{
		if (cX >= imputImage_.cols)
		{
			cX = getS() / 2;
			cY += getS();
			if (cY >= imputImage_.rows) 
				break;
		}
		
		Cluster cluster;
		cluster.x = cX;
		cluster.y = cY;
		tabCluaster_->push_back(cluster);
		cX += getS();
	}
}

void SLICsp::moveClusterCenter()
{

}

void SLICsp::initLabelDistancePixel()
{
	releaseTabLabelDistance();
	tabLabel_ = Mat(imputImage_.rows,imputImage_.cols,
						CV_32SC1,
						Scalar(-1));

	tabDistance_ = Mat(imputImage_.rows, imputImage_.cols,
						CV_16UC1, 
						Scalar(numeric_limits<unsigned short>::max()));


/*	for (int i = 0; i < getN(); i++)
		cout << "L " << i << " : " << (int)tabLabel_.at<int>(i)
		<< " D " << (int)tabDistance_.at<unsigned short>(i) << endl;
*/
}

unsigned int SLICsp::getS()const
{
	return S_;
}

unsigned int SLICsp::getN()const
{
	return imputImage_.total();
}

unsigned int SLICsp::getK()const
{
	return k_;
}

void SLICsp::releaseResource()
{
	releaseTabCluster();
	releaseTabLabelDistance();
}

void SLICsp::releaseTabCluster()
{
	if (tabCluaster_)
		delete tabCluaster_;
	tabCluaster_ = nullptr;
}

void SLICsp::releaseTabLabelDistance()
{
	tabLabel_.release();
	tabDistance_.release();
}
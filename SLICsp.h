#ifndef SLICSP
#define SLICSP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

struct Cluster
{
	unsigned char l = 0;
	unsigned char a = 0;
	unsigned char b = 0;
	unsigned int x = 0;
	unsigned int y = 0;
};

class SLICsp
{
public:
	SLICsp(const Mat &inputImg, unsigned int k);
	~SLICsp();
	void initClusterCenter();
	void moveClusterCenter();
	void initLabelDistancePixel();
	unsigned int getS()const;
	unsigned int getN()const;
	unsigned int getK()const;

//private:
	void releaseResource();
	void releaseTabCluster();
	void releaseTabLabelDistance();
	vector<Cluster> *tabCluaster_;
	Mat tabLabel_;
	Mat tabDistance_;
	const Mat &imputImage_;
	unsigned int k_;
	unsigned int S_;
};

#endif
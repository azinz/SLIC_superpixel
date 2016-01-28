#ifndef SLICSP
#define SLICSP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

struct LABxy
{
	int l = 0;
	int a = 0;
	int b = 0;
	int x = 0;
	int y = 0;
	unsigned int count = 0;
};

class SLICsp
{
public:
	SLICsp(const Mat &inputImg, int k, int m);
	~SLICsp();
	void initClusterCenter();
	void moveClusterCenter();
	void initLabelDistancePixel();
	void computeDistance();
	void meanClaster();

	int getS()const;
	int getN()const;
	int getK()const;
	int getM()const;
	////////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!
//private:
	void findMinimalValue(const Mat &inputImage, LABxy &clusterCenter);
	float distance(const LABxy &clusterCenter, const LABxy &pixel, int m, int S);
	void releaseResource();
	void releaseTabCluster();
	void releaseTabLabelDistance();
	vector<LABxy> tabCluster_;
	Mat tabLabel_;
	Mat tabDistance_;
	Mat labSpaceImage_;
	const Mat &inputImage_;
	int k_;
	int m_;
	int S_;
};


#endif
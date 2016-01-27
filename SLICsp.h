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
	uchar l = 0;
	uchar a = 0;
	uchar b = 0;
	uint x = 0;
	uint y = 0;
};

class SLICsp
{
public:
	SLICsp(const Mat &inputImg, unsigned int k, uint m);
	~SLICsp();
	void initClusterCenter();
	void moveClusterCenter();
	void initLabelDistancePixel();
	void computeDistance();
	void meanClaster();

	uint getS()const;
	uint getN()const;
	uint getK()const;
	uint getM()const;
	////////////////////////////////////////////!!!!!!!!!!!!!!!!!!!!
//private:
	void findMinimalValue(const Mat &inputImage, LABxy &clusterCenter);
	uint distance(const LABxy &clusterCenter, const LABxy &pixel, double m, uint S);
	void releaseResource();
	void releaseTabCluster();
	void releaseTabLabelDistance();
	vector<LABxy> tabCluster_;
	Mat tabLabel_;
	Mat tabDistance_;
	Mat labSpaceImage_;
	const Mat &inputImage_;
	uint k_;
	uint m_;
	uint S_;
};


#endif
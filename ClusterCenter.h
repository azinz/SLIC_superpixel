#ifndef CLUASTER_CENTER
#define CLUASTER_CENTER

#include <vector>

using namespace std;

struct Cluster
{
	unsigned char l = 0;
	unsigned char a = 0;
	unsigned char b = 0;
	unsigned int x = 0;
	unsigned int y = 0;
};

class ClusterCenter
{
public:
	ClusterCenter(unsigned int size);

	void clear();

private:
	vector<Cluster> tabCluaster_;
};


#endif
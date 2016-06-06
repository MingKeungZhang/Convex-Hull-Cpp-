#ifndef __kdtree__
#define __kdtree__
#include <string>
#include "LList.h"
#include <fstream>

class KDNode {
private:
	double lat;
	double lon;
	int depth;
	std::string name;
	KDNode *left;
	KDNode *right;

	double distance(double _la, double _lo);

public:
	KDNode(double la, double lo, int d, const std::string &desc);
	~KDNode();

	friend class KDTree;
};

class KDTree {
private:
	unsigned int size;
	KDNode *root;
	unsigned int count = 0;
	LinkedList set;
	LinkedList hull;

	void destroy(KDNode *p);
	KDNode *KDTree::insert(KDNode *t, double la, double lo, int d, const std::string &desc);

public:
	KDTree();
	~KDTree();

	void insert(double la, double lo, const std::string &desc);
	void rangeQuerie(KDNode *t, double la, double lo, double rad, const std::string &filter);
	void printNeighbors(double la, double lo, double rad, const std::string &filter);
	void printConvexHull(double la, double lo, double rad, const std::string &filter);
	unsigned int getSize();

};

#endif
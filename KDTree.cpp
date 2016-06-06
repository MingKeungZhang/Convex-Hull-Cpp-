#include "stdafx.h"
#include "KDTree.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <iostream>


KDNode::KDNode(double la, double lo, int d, const std::string &desc) {
	left = NULL;
	right = NULL;
	depth = d;
	name = desc;
	lat = la;
	lon = lo;
}

KDNode::~KDNode() {
}

double KDNode::distance(double _la, double _lo) {
	double param = M_PI / 180.0; // required for conversion from degrees to radians
	double rad = 3956.0;  // radius of earth in miles
	double d_lat = (_la - lat) * param;
	double d_lon = (_lo - lon) * param;
	double dist = sin(d_lat / 2) * sin(d_lat / 2) + cos(lat*param) * cos(_la*param) * sin(d_lon / 2) * sin(d_lon / 2);
	dist = 2.0 * atan2(sqrt(dist), sqrt(1 - dist));
	return rad * dist;
}

KDTree::KDTree() {
	root = NULL;
}

KDTree::~KDTree() {
	destroy(root);
}

void KDTree::destroy(KDNode *p) {
	if (p == NULL)
		return;
	destroy(p->left);
	destroy(p->right);
	delete p;
}

KDNode *KDTree::insert(KDNode *t, double la, double lo, int d, const std::string &desc) {
	if (t == NULL) {
		size += 1;
		return new KDNode(la, lo, d, desc);
	}

	if (t->depth % 2 == 0) {
		if (t->lat >= la) 
			t->left = insert(t->left, la, lo, t->depth + 1, desc);
		else if (t->lat < la) 
			t->right = insert(t->right, la, lo, t->depth + 1, desc);
		
	}
	else if (t->depth % 2 == 1) {
		if (t->lon >= lo) 
			t->left = insert(t->left, la, lo, t->depth + 1, desc);
		else if (t->lon < lo) 
			t->right = insert(t->right, la, lo, t->depth + 1, desc);
	}

	return t;
}

void KDTree::insert(double la, double lo, const std::string &desc) {
	root = insert(root, la, lo, 0, desc);
}

void KDTree::rangeQuerie(KDNode *t, double la, double lo, double rad, const std::string &filter) {
	if (t == NULL)
		return;
	if (t->distance(la, lo) < rad && t->name.find(filter) != std::string::npos) {
		std::ofstream myfile("data/markers.js", std::ios::app);
		if (myfile.is_open()) {
			myfile << "\t[\"" << t->name << "\", " << t->lat << ", " << t->lon << "],\n";
			std::cout << "\t[\"" << t->name << "\", " << t->lat << ", " << t->lon << "],\n";
			myfile.close();
		}
		count++;
		set.insert(t->lat, t->lon, t->name);
		if (t->depth % 2 == 0) {
			if (t->lat > la - rad && t->lat < la + rad) {
				rangeQuerie(t->left, la, lo, rad, filter);
				rangeQuerie(t->right, la, lo, rad, filter);
			}
			else if (t->lat >= la + rad)
				rangeQuerie(t->left, la, lo, rad, filter);
			else if (t->lat < la - rad)
				rangeQuerie(t->right, la, lo, rad, filter);
		}
		else if (t->depth % 2 == 1) {
			if (t->lon < lo + rad && t->lon > lo - rad) {
				rangeQuerie(t->left, la, lo, rad, filter);
				rangeQuerie(t->right, la, lo, rad, filter);
			}
			else if (t->lon >= lo + rad)
				rangeQuerie(t->left, la, lo, rad, filter);
			else if (t->lon < lo - rad)
				rangeQuerie(t->right, la, lo, rad, filter);

		}
	}
	else{
		if (t->depth % 2 == 0) {
			if (t->lat > la - rad && t->lat < la + rad) {
				rangeQuerie(t->left, la, lo, rad, filter);
				rangeQuerie(t->right, la, lo, rad, filter);
			}
			else if (t->lat >= la + rad)
				rangeQuerie(t->left, la, lo, rad, filter);
			else if (t->lat < la - rad)
				rangeQuerie(t->right, la, lo, rad, filter);
		}
		if (t->depth % 2 == 1) {
			if (t->lon < lo + rad && t->lon > lo - rad) {
				rangeQuerie(t->left, la, lo, rad, filter);
				rangeQuerie(t->right, la, lo, rad, filter);
			}
			else if (t->lon >= lo + rad)
				rangeQuerie(t->left, la, lo, rad, filter);
			else if (t->lon < lo - rad)
				rangeQuerie(t->right, la, lo, rad, filter);

		}
	}
	return;
}

void KDTree::printNeighbors(double la, double lo, double rad, const std::string &filter) {
	std::ofstream myfile("data/markers.js");
	if (myfile.is_open()) {
		std::cout << "var markers = [\n";
		myfile << "var markers = [\n";
		std::cout << "\t[\"" << "CENTER" << "\", " << la << ", " << lo << "],\n";
		myfile << "\t[\"" << "CENTER" << "\", " << la << ", " << lo << "],\n";
		myfile.close();
	}
	std::ofstream myfile1("data/markers.js", std::ios::app);
	if (myfile1.is_open()) {
		rangeQuerie(root, la, lo, rad, filter);
		myfile1 << "];\n";
		std::cout << "];\n";
		myfile1.close();
	}

}

void KDTree::printConvexHull(double la, double lo, double rad, const std::string &filter) {
	printNeighbors(la, lo, rad, filter);
	set.printlist();
	LLNode *temp = set.getHead();
	set.latSort(&temp);
	set.printlist();
	temp = set.getHead();
	LLNode *temp1 = temp;
	temp = set.increment(temp);
	set.polarSort(&temp, temp1);
	set.printlist();
	hull.insertEnd(temp1);
	temp1 = set.increment(temp1);
	hull.insertEnd(temp1);
	temp1 = set.increment(temp1);
	
	for (int i = 2; i < set.getSize(); i++) {
		LLNode* top = hull.remove();
		while (set.ccwTurn(hull.getTail(), top, temp1) <= 0) {
			top = hull.remove();
		}
		hull.insertEnd(top);
		hull.insertEnd(temp1);
		temp1 = set.increment(temp1);
	}
	hull.print();
}

unsigned int KDTree::getSize() {
	return size;
}
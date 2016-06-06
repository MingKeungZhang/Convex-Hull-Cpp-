#include "stdafx.h"
#include "LList.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <fstream>
#include <iostream>



LLNode::LLNode(double la, double lo, const std::string &desc) {
	next = NULL;
	name = desc;
	lat = la;
	lon = lo;
}

LLNode::~LLNode() {
}

double LLNode::distance(double _la, double _lo) {
	double param = M_PI / 180.0; // required for conversion from degrees to radians
	double rad = 3956.0;  // radius of earth in miles
	double d_lat = (_la - lat) * param;
	double d_lon = (_lo - lon) * param;
	double dist = sin(d_lat / 2) * sin(d_lat / 2) + cos(lat*param) * cos(_la*param) * sin(d_lon / 2) * sin(d_lon / 2);
	dist = 2.0 * atan2(sqrt(dist), sqrt(1 - dist));
	return rad * dist;
}


LinkedList::LinkedList() {
	head = NULL;
	tail = NULL;
}

LinkedList::~LinkedList() {
	destroy(head);
}

void LinkedList::destroy(LLNode *p) {
	LLNode *temp;
	while (p) {
		temp = p;
		p = p->next;
		delete temp;
	}
}

// return the number of nodes in the list
unsigned int LinkedList::getSize() {
	return size;
}

// insert a new node at the beginning of the list
void LinkedList::insert(double la, double lo, const std::string &desc) {
	LLNode *p = new LLNode(la, lo, desc);
	// Case 1 list already exists
	if (head) {
		p->next = head;
		head = p;
	}
	// Case 2 no list has been made yet
	else {
		head = tail = p;
	}
	size++;
}

// insersts new node at the end of the list
void LinkedList::insertEnd(LLNode *p) {
	LLNode *temp = new LLNode(p->lat, p->lon, p->name);
	// Case 1 if list already exists
	if (head) {
		tail->next = temp;
		tail = tail->next;
	}
	// Case 2 no list has been made yet
	else {
		head = tail = temp;
	}
	size++;
}

// print all the nodes under a distance 'rad' from 'la,lo' and where
// filter is a non-empty substring of their description
unsigned int LinkedList::printNeighbors(double la, double lo, double rad, const std::string &filter) {
	unsigned int count = 0;
	std::cout << "var markers = [\n";
	LLNode *p = head;
	std::cout << "\t[\"" << "CENTER" << "\", " << la << ", " << lo << "],\n";
	while (p) {
		if (p->distance(la, lo) < rad && p->name.find(filter) != std::string::npos) {
			std::cout << "\t[\"" << p->name << "\", " << p->lat << ", " << p->lon << "],\n";
			count++;
		}
		p = p->next;
	}
	std::cout << "];\n";
	return count;
}

// Sorts the list based on the latitude
void LinkedList::latSort(LLNode **p) {
	LLNode *temp = *p;
	LLNode* x;
	LLNode* y;

	// base case
	if (temp == NULL || temp->next == NULL) {
		return;
	}

	// Splits list in half
	partition(temp, &x, &y);

	// Sorts one half
	latSort(&x);
	// Sorts other
	latSort(&y);

	// merges two lists and sorts it based on latitude
	*p = mergeLat(x, y);
	
	// updates head of the list
	head = *p;

}

// sorts the list by the polar angle based on a certain point
void LinkedList::polarSort(LLNode **p, LLNode *base) {
	LLNode *temp = *p;
	LLNode *x;
	LLNode *y;

	// base case
	if (temp == NULL || temp->next == NULL) {
		return;
	}

	// Splits list in half
	partition(temp, &x, &y);

	// Sorts one half
	polarSort(&x, base);
	
	// Sorts the other
	polarSort(&y, base);

	// Merges two lists and sorts them by polar angle based on LLNode base
	*p = mergePolar(x, y, base);
	head->next = *p;

}

// cuts the linked list in half
void LinkedList::partition(LLNode *p, LLNode **front, LLNode **back) {
	LLNode *fast;
	LLNode *slow;

	// If only one node in list
	if (p == NULL || p->next == NULL) {
		*front = p;
		*back = NULL;
	}
	else {
		slow = p;
		fast = p->next;
		// Fast is incremented twice as fast as slow so slow = half way point
		while (fast != NULL) {
			fast = fast->next;
			if (fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}
		// Front of first half of list is the head
		*front = p;
		// Front of the second half of the list
		*back = slow->next;
		slow->next = NULL;
	}
}

// merges two linked lists and sorts by the lat
LLNode* LinkedList::mergeLat(LLNode *x, LLNode *y) {
	LLNode *merged = NULL;
	if (x == NULL)
		return y;
	if (y == NULL)
		return x;
	// Compares the latitude and sorts it
	if (x->lat <= y->lat) {
		merged = x;
		merged->next = mergeLat(x->next, y);
	}
	else {
		merged = y;
		merged->next = mergeLat(x, y->next);
	}
	return merged;
}

// Merges two linked lists and sorts it by polar angle
LLNode* LinkedList::mergePolar(LLNode *x, LLNode *y, LLNode *p) {
	LLNode *merged;
	if (x == NULL)
		return y;
	if (y == NULL)
		return x;
	// compares the -x/y to determine which angle is smaller
	if (-(p->lon - x->lon)/(p->lat - x->lat) <= -(p->lon - y->lon)/(p->lat - y->lat)) {
		merged = x;
		merged->next = mergePolar(x->next, y, p);
	}
	else {
		merged = y;
		merged->next = mergePolar(y->next, x, p);
	}
	return merged;
}

// gets the head of the list
LLNode* LinkedList::getHead() {
	return head;
}

// gets the tail of the list
LLNode* LinkedList::getTail() {
	return tail;
}

// sets the head of the list to a certain node
void LinkedList::setHead(LLNode *p) {
	LLNode *temp = head;
	head = p;
	head->next = temp;
}

// increments a node
LLNode* LinkedList::increment(LLNode *p) {
	LLNode *temp = p->next;
	return temp;
}

// determines if 3 nodes are a ccw turn or not
int LinkedList::ccwTurn(LLNode *p, LLNode *c, LLNode *n) {
	double area2 = (c->lon - p->lon)*(n->lat - p->lat) - (c->lat - p->lat)*(n->lon - p->lon);
	if (area2 < 0)
		return -1;						// not a ccw turn
	else if (area2 > 0)
		return 1;						// ccw turn
	else
		return 0;						// the 3 points are co linear
}

// removes and returns the last node
LLNode* LinkedList::remove() {
	LLNode *temp = head;
	// Case 1 no list 
	if (temp == NULL)
		return NULL;
	// Case 2 only 1 node
	if (temp->next == NULL) {
		head = NULL;
		tail = NULL;
		return temp;
	}
	// Case 3 two nodes
	if (temp->next->next == NULL) {
		LLNode *last = tail;
		tail = head;
		return last;
	}
	// Case 4 more than two nodes
	else if (temp->next->next != NULL) {
		while (temp->next->next != NULL) {
			temp = temp->next;
		}
		tail = temp;
		LLNode *last = temp->next;
		tail->next = NULL;
		return last;
	}
	
}

// prints the list in reverse
void LinkedList::reversePrint(LLNode *p) {
	if (p == NULL) {
		return;
	}

	reversePrint(p->next);

	std::ofstream myfile("data/convex.js", std::ios::app);
	if (myfile.is_open()) {
		myfile << "\t{" << "lat: " << p->lat << ", lng: " << p->lon << "},\n";
		myfile.close();
	}

}

// prints the data to convex.js
void LinkedList::print() {
	std::ofstream myfile("data/convex.js");
	if (myfile.is_open()) {
		myfile << "var convex = [\n";
		myfile.close();
	}
	std::ofstream myfile1("data/convex.js", std::ios::app);
	if (myfile.is_open(), std::ios::app) {
		reversePrint(head);
		myfile1 << "\t{" << "lat: " << tail->lat << ", lng: " << tail->lon << "},\n";
		myfile1 << "];";
		myfile1.close();
	}
}

// prints the whole list
void LinkedList::printlist() {
	std::cout << "printing list";
	LLNode *p = head;
	while (p) {
		std::cout << "\t[\"" << p->name << "\", " << p->lat << ", " << p->lon << "],\n";
		p = p->next;
	}
}

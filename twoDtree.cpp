
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"

/* node constructor given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, HSLAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),LT(NULL),RB(NULL)
	{}

/* destructor given */
twoDtree::~twoDtree(){
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


/* operator= given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

/* twoDtree constructor */
twoDtree::twoDtree(PNG & imIn){ 
/* your code here */
	height = imIn.height();
    width = imIn.width();
    stats s = stats(imIn);
    root = buildTree(s, pair<int, int>(0, 0), pair<int, int>(width - 1, height - 1), true);
}

/* buildTree helper for twoDtree constructor */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {

	HSLAPixel pixel = s.getAvg(ul, lr);
	Node *curr = new Node(ul, lr, pixel);
	if (ul.first == lr.first and ul.second == lr.second)
	{
		curr->LT = NULL;
		curr->RB = NULL;
		return curr;
	}

	if (ul.first == lr.first)
	{
		vert = false;
	}
	else if (ul.second == lr.second)
	{
		vert = true;
	}
	double area = (double)s.rectArea(ul, lr);
	double mesf = std::numeric_limits<double>::max();
	if (vert)
	{
		
		pair<int, int> newLR = make_pair(0, 0);
		pair<int, int> newUL = make_pair(0, 0);
		for (int i = ul.first; (i) < lr.first; i++)
		{
			pair<int, int> rectLeftBtm = make_pair(i, lr.second);
			pair<int, int> rectRightTop = make_pair(i + 1, ul.second);
			int areaLeft = s.rectArea(ul, rectLeftBtm);
			int areaRight = s.rectArea(rectRightTop, lr);
			double totEnt = (s.entropy(ul, rectLeftBtm) * areaLeft + s.entropy(rectRightTop, lr) * areaRight) / area;
			if (mesf >= totEnt)
			{
				mesf = totEnt;
				newLR = rectLeftBtm;
				newUL = rectRightTop;
	
			}
		}

		Node *newLT = buildTree(s, ul, newLR, !vert);
		Node *newRB = buildTree(s, newUL, lr, !vert);
		curr->LT = newLT;
		curr->RB = newRB;
	}
	else
	{
		pair<int, int> newTop = make_pair(0, 0);
		pair<int, int> newBtm = make_pair(0, 0);
		for (int i = ul.second; (i) < lr.second; i++)
		{
			pair<int, int> rectTopRight = make_pair(lr.first, i);
			pair<int, int> rectBtmLeft = make_pair(ul.first, i + 1);
			int areaTop = s.rectArea(ul, rectTopRight);
			int areaBtm = s.rectArea(rectBtmLeft, lr);
			double totEnt = (s.entropy(ul, rectTopRight) * areaTop + s.entropy(rectBtmLeft, lr) * areaBtm) / ((1.0) * area);
			if (mesf >= totEnt)
			{
				mesf = totEnt;
				newTop = rectTopRight;
				newBtm = rectBtmLeft;
			}
		}
		Node *newLT = buildTree(s, ul, newTop, !vert);
		Node *newRB = buildTree(s, newBtm, lr, !vert);
		curr->LT = newLT;
		curr->RB = newRB;
	}
	return curr;
}


PNG twoDtree::render(){
	PNG img = PNG(width, height);
	Node* curr = root;
	renderTree(&img, curr);
	return img;
}

// Recursive helper for render
void twoDtree::renderTree(PNG *img, Node* curr) {
	if (curr != NULL && (curr->LT) == NULL && (curr->RB) == NULL) {												
		
		for (int i = curr->upLeft.first; i <= curr->lowRight.first; i++) {
			for (int j = curr->upLeft.second; j <= curr->lowRight.second; j++) {
				*img->getPixel(i, j) = curr->avg;
			}
		}
		return;
	} 
	renderTree(img, curr->LT);
	renderTree(img, curr->RB);
}

/* prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of 
 * the average pixel value contained in the root
 * of the subtree
 */
void twoDtree::prune(double tol){

/* your code here */
	Node* temp = root;
	pruneHelper(temp, tol);
}

/* prune's helper. This prunes the tree! */
vector<HSLAPixel> twoDtree::pruneHelper(Node *node, double tol) {
    
	if (node->LT == NULL and node->RB == NULL)
	{
		vector<HSLAPixel> vect;
		vect.push_back(node->avg);
		return vect;
	}

	vector<HSLAPixel> leftVect = pruneHelper(node->LT, tol);
	vector<HSLAPixel> rightVect = pruneHelper(node->RB, tol);
	for (int i = 0; i < (int)rightVect.size(); i++)
	{
		leftVect.push_back(rightVect[i]);
	}
	if (satisfied(node, tol, leftVect))
	{
		clearNode(node->LT);
		clearNode(node->RB);
		node->LT = NULL;
		node->RB = NULL;
		return leftVect;
	}
	return leftVect;
}

/* pruneHelper's helper. Determines if the leaves of the current node are within the tolerance. */
bool twoDtree::satisfied(Node* node, double tol, vector<HSLAPixel> vect) {
    
	for (int i = 0; i < (int)vect.size(); i++) {

		if ((abs(vect[i].dist(node->avg))) > tol)
		{
			return false;
		}
	}
	return true;
}

/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
	clearNode(root);
}

void twoDtree::clearNode(Node* subRoot){
    if (subRoot == NULL) return;
	
    clearNode(subRoot->LT);
    clearNode(subRoot->RB);
	subRoot->LT = NULL;
	subRoot->RB = NULL;
    delete subRoot;
	subRoot = NULL;
}

/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree & orig){
    height = orig.height;
    width = orig.width;
	Node* subRoot = orig.root;
    root = copyNode(subRoot);
}

twoDtree::Node* twoDtree::copyNode(Node* subRoot){
    if (subRoot == NULL) return NULL;
    Node* temp = new Node(subRoot->upLeft, subRoot->lowRight, subRoot->avg);
    temp->LT = copyNode(subRoot->LT);
    temp->RB = copyNode(subRoot->RB);
    return temp;
}




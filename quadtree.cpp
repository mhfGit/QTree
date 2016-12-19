#include "quadtree.h"
#include <iostream>
/*Constructs a QuadtreeNode with a white pixel
* in the element member.
*/
Quadtree::QuadtreeNode::QuadtreeNode()
{
	nwChild = NULL;
	neChild = NULL;
	swChild = NULL;
	seChild = NULL;

	element = RGBAPixel();
}

/* Constructs a QuadtreeNode with the pixel
* specified .
*/
Quadtree::QuadtreeNode::QuadtreeNode(RGBAPixel const &source)
{
	nwChild = NULL;
	neChild = NULL;
	swChild = NULL;
	seChild = NULL;

	element = source;
}
/* Default Constructor for a Quadtree
* Sets root to NULL and resolution to 0
*/
Quadtree::Quadtree()
{
	root = NULL;
	_resolution = 0;
}
/*Constructor for a Quadtree
* crops the source image determined by
* the value of resolution.
*/
Quadtree::Quadtree(PNG const &source, int resolution)
{
	root = NULL;
	buildTree(source, resolution);
	_resolution = resolution;
}


/*BIG THREE IMPLEMENTED HERE
*
*/


/*Copy constructor
*/
Quadtree::Quadtree(Quadtree const&other)
{
	root = copy(other.root);
	_resolution = other._resolution;
}
/*copy constructor helper function
*/
Quadtree::QuadtreeNode* Quadtree::copy(QuadtreeNode *current)
{
	if (current == NULL)
	{
		return NULL;
	}
	else
	{
		QuadtreeNode *newNode = new QuadtreeNode(current->element);
		newNode->nwChild = copy(current->nwChild);
		newNode->neChild = copy(current->neChild);
		newNode->swChild = copy(current->swChild);
		newNode->seChild = copy(current->seChild);
		return newNode;
	}
}
/*Destructor; frees all the memory associated
* with this Quadtree
*/
Quadtree::~Quadtree()
{
	clear(root);
	root = NULL;
}
/*Destructor helper function
*/
void Quadtree::clear(QuadtreeNode *current)
{
	if (current == NULL)
	{
		return;
	}
	else
	{
		clear(current->nwChild);
		clear(current->neChild);
		clear(current->swChild);
		clear(current->seChild);

		delete current;
		current = NULL;
		return;
	}
}
/*Assignment operator; frees memory associated with this Quadtree
* and sets its contents to be equal to the parameters
*/
Quadtree const& Quadtree::operator=(Quadtree const &other)
{
	if (this != &other)
	{
		clear(root);
		root = copy(other.root);
		_resolution = other._resolution;
	}
	return *this;
}



/*OTHER FUNCTIONS
*/




/*Deletes the current contents of this Quadtree object,
* then turns it into a Quadtree object representing the upper-left
* d by d block of source
*/
void Quadtree::buildTree(PNG const &source, int resolution)
{
	clear(root);
	Point thePoints(0, 0);
	_resolution = resolution;
	root = buildTreehelp(source, resolution, thePoints);
	findAverages(RGBVal(), root, resolution);
}
/*Helper function for the buildtree function
*/
Quadtree::QuadtreeNode* Quadtree::buildTreehelp(PNG const &source, int resolution, Quadtree::Point thePoint)
{
	if (resolution == 1)
	{
		RGBAPixel const* temp = source((size_t)thePoint.x, (size_t)thePoint.y);
		QuadtreeNode *newNode = new QuadtreeNode(*temp);
		return newNode;
	}
	else
	{
		QuadtreeNode *newNode = new QuadtreeNode();
		newNode->nwChild = buildTreehelp(source, resolution / 2, thePoint);
		newNode->neChild = buildTreehelp(source, resolution / 2, Point(thePoint.x + (resolution / 2), thePoint.y));
		newNode->swChild = buildTreehelp(source, resolution / 2, Point(thePoint.x, thePoint.y + (resolution / 2)));
		newNode->seChild = buildTreehelp(source, resolution / 2, Point(thePoint.x + (resolution / 2), thePoint.y + (resolution / 2)));

		return newNode;
	}
}
/*Finds the internal node values for their element. Should be the average.
*/
Quadtree::RGBVal Quadtree::findAverages(RGBVal total, QuadtreeNode* current, int resolution)
{
	if (resolution == 1)
	{
		total.R = current->element.red;
		total.G = current->element.green;
		total.B = current->element.blue;
		return total;
	}
	else
	{
		RGBVal nwValues;
		RGBVal neValues;
		RGBVal swValues;
		RGBVal seValues;
		nwValues = findAverages(total, current->nwChild, resolution / 2);
		neValues = findAverages(total, current->neChild, resolution / 2); //should i use int to pass sumR or uint8_t
		swValues = findAverages(total, current->swChild, resolution / 2);
		seValues = findAverages(total, current->seChild, resolution / 2);

		total.R = (nwValues.R + neValues.R + swValues.R + seValues.R) / 4;
		total.G = (nwValues.G + neValues.G + swValues.G + seValues.G) / 4;
		total.B = (nwValues.B + neValues.B + swValues.B + seValues.B) / 4;


		current->element.red = total.R;
		current->element.green = total.G;
		current->element.blue = total.B;
		return total;
	}
}
/*Gets the RGBAPixel corresponding to the pixel at coordinates (x,y) in the bitmap
* image which the Quadtree represents
*/
RGBAPixel Quadtree::getPixel(int x, int y) const
{
	RGBAPixel temp = getPixelhelp(x, y, root, _resolution);
	return temp;
}
/*Helper function to find the pixel
*/
RGBAPixel Quadtree::getPixelhelp(int x, int y, QuadtreeNode *current, int resolution) const
{
	if (x >= resolution || y >= resolution || current == NULL)
	{
		return RGBAPixel();
	}
	while (current->nwChild != NULL)
	{
		if ((x < (resolution / 2)) && (y < (resolution / 2)))
		{
			current = current->nwChild;
			resolution = resolution / 2;
		}
		else if ((x >= (resolution / 2)) && (y < (resolution / 2)))
		{
			current = current->neChild;
			x = x - (resolution / 2);
			resolution = resolution / 2;
		}
		else if ((x < (resolution / 2)) && (y >= (resolution / 2)))
		{
			current = current->swChild;
			y = y - (resolution / 2);
			resolution = resolution / 2;
		}
		else
		{
			current = current->seChild;
			x = x - (resolution / 2);
			y = y - (resolution / 2);
			resolution = resolution / 2;
		}
	}
	return current->element;
}
/*Returns the underlying PNG object represented by the Quadtree
*/
PNG Quadtree::decompress() const
{
	if (root == NULL)
	{
		return PNG();
	}
	PNG newImage((size_t)_resolution, (size_t)_resolution);
	for (int height = 0; height < _resolution; height++)
	{
		for (int width = 0; width < _resolution; width++)
		{
			*newImage(width, height) = getPixel(width, height);
		}
	}
	return newImage;
}
/*Rotates the Quadtree object's underlying image clockwise by 90 degrees.
*/
void Quadtree::clockwiseRotate()
{
	clockwiseRotatehelp(root);
}
/*clockwise Rotate helper function
* needs to use return pointers so we make it private.
*/
Quadtree::QuadtreeNode* Quadtree::clockwiseRotatehelp(QuadtreeNode *current)
{
	if (current == NULL)
	{
		return NULL;
	}
	else
	{
		current->nwChild = clockwiseRotatehelp(current->nwChild);
		current->neChild = clockwiseRotatehelp(current->neChild);
		current->swChild = clockwiseRotatehelp(current->swChild);
		current->seChild = clockwiseRotatehelp(current->seChild);

		QuadtreeNode *temp = current->nwChild;
		current->nwChild = current->swChild;
		current->swChild = current->seChild;
		current->seChild = current->neChild;
		current->neChild = temp;
		return current;
	}
}
/*Compresses the image this Quadtree represents.
*/
void Quadtree::prune(int tolerance)
{
	root = pruneHelp(root, tolerance, _resolution);
}
/*Helper function for prune help
* recursively checks if each node can be compressed to avg of values
*/
Quadtree::QuadtreeNode* Quadtree::pruneHelp(QuadtreeNode *current, int tolerance, int resolution)
{
	if (current == NULL)
	{
		return NULL;
	}
	else
	{
		if (pruneChecker(current, current, tolerance, resolution))
		{
			clear(current->nwChild);
			clear(current->neChild);
			clear(current->swChild);
			clear(current->seChild);
			current->nwChild = NULL;
			current->neChild = NULL;
			current->swChild = NULL;
			current->seChild = NULL;
			return current;
		}
		else
		{
			current->nwChild = pruneHelp(current->nwChild, tolerance, resolution / 2);
			current->neChild = pruneHelp(current->neChild, tolerance, resolution / 2);
			current->swChild = pruneHelp(current->swChild, tolerance, resolution / 2);
			current->seChild = pruneHelp(current->seChild, tolerance, resolution / 2);

			return current;
		}
	}
}
/*Helper function for helper function to prune help
* IF the return value is a 1 then we want to prune the
* root node we are at.
*/
bool Quadtree::pruneChecker(QuadtreeNode *Current, QuadtreeNode *ROOT, int tolerance, int resolution) const
{
	if (Current == NULL)
	{
		return 0;
	}
	if (Current->nwChild == NULL)
	{
		int Red = Current->element.red;
		int Green = Current->element.green;
		int Blue = Current->element.blue;
		//std::cout << "current" << Red << std::endl;

		int redRoot = ROOT->element.red;
		int greenRoot = ROOT->element.green;
		int blueRoot = ROOT->element.blue;
		//std::cout << "root" << redRoot << std::endl;
		//std::cout << "tolerance" << tolerance<< std::endl;	

		int newTol = (Red - redRoot)*(Red - redRoot) + (Green - greenRoot)*(Green - greenRoot) + (Blue - blueRoot)*(Blue - blueRoot);
		if (newTol > tolerance)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		bool nw = pruneChecker(Current->nwChild, ROOT, tolerance, resolution / 2);
		bool ne = pruneChecker(Current->neChild, ROOT, tolerance, resolution / 2);
		bool sw = pruneChecker(Current->swChild, ROOT, tolerance, resolution / 2);
		bool se = pruneChecker(Current->seChild, ROOT, tolerance, resolution / 2);

		int sum = nw + ne + sw + se;
		if (sum == 4)
		{
			//std::cout << "in" << std::endl;
			return 1;
		}
		else
		{
			return 0;
		}
	}
}
/*This function is similar to prune however it does not
* remove any nodes.
*/
int Quadtree::pruneSize(int tolerance) const
{
	return prunesizeHelp(root, tolerance, _resolution);
}
/*Helper function to pruneSize very similar to the helper function of
* prune but instead it returns an int.
*/
int Quadtree::prunesizeHelp(QuadtreeNode *current, int tolerance, int resolution) const
{
	if (current == NULL)
	{
		return 0;
	}
	if (current->nwChild == NULL)
	{
		return 1;
	}
	else
	{
		if (pruneChecker(current, current, tolerance, resolution))
		{
			return 1;
		}
		else
		{
			int nw = prunesizeHelp(current->nwChild, tolerance, resolution / 2);
			//std::cout << "nw" << nw << std::endl;
			//std::cout << "resolution" << resolution << std::endl;
			int ne = prunesizeHelp(current->neChild, tolerance, resolution / 2);
			int sw = prunesizeHelp(current->swChild, tolerance, resolution / 2);
			int se = prunesizeHelp(current->seChild, tolerance, resolution / 2);

			return (nw + ne + sw + se);
		}
	}
}
/*Calculates and returns the minimum tolerance necessary to guarantee that upon
* pruning the tree, no more than numLeaves leaves remain in the Quadtree.
*/
int Quadtree::idealPrune(int numLeaves)const
{
	int maxTol = 255 * 255 * 3;
	int testTol = maxTol / 2;
	int bottomVal = 0;

	while (pruneSize(testTol) != numLeaves)
	{
		if (pruneSize(testTol) > numLeaves)
		{
			bottomVal = testTol;
			testTol = (maxTol + testTol) / 2;
		}
		if (pruneSize(testTol) < numLeaves)
		{
			maxTol = testTol;
			testTol = (bottomVal + testTol) / 2;
		}

	}
	while (pruneSize(testTol) == numLeaves)
	{
		testTol--;
	}

	return testTol + 1;
}
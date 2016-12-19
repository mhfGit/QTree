#ifndef QUADTREE_H
#define QUADTREE_H

#include "png.h"

class Quadtree
{
public:
	/*
	* Default Quadtree constructor.
	* creates an empty quadtree.
	*/
	Quadtree();
	/*
	* Build a quadtree representing the upper
	* left d by d block of the source image.
	* Crops the source image into a d x d square.
	*/
	Quadtree(PNG const &source, int resolution);
	/*
	* Copy constructor for a quadtree.
	* Need to constructo Big three after this.
	*/
	Quadtree(Quadtree const &other);
	/*
	* Overloaded assignment operator for quadtree.
	* Part of the big three that we must define because
	* the class allocates dynamic memory.
	*/
	Quadtree const & operator=(Quadtree const &other);
	/*
	*  Destroys the current quadtree.
	*  This ensures there are no memory leaks.
	*/
	~Quadtree();
	/*Deletes the current contents of this Quadtree object
	* then turns it into a Quadtree object representing the upper-left d by d block of source.
	*/
	void buildTree(PNG const &source, int resolution);
	/*Gets the RGBAPixel corresponding to the pixel at coordinates (x,y) in
	* the bitmap image which the Quadtree represents.
	*/
	RGBAPixel getPixel(int x, int y) const;
	/*Returns the underlying PNG object represented by the Quadtree
	*/
	PNG decompress() const;
	/*This class is for storing the current pixel
	* position. Used for build tree
	*/
	class Point
	{
		//A class to hold the values of the current pixel that 
		//is going to be stored in the Quadtree rgbapixel element
	public:
		Point(int newX, int newY)
		{
			x = newX;
			y = newY;
		}

		int x; // x coordinate of the PNG
		int y; // y coordinate of the PNG
	};
	//This class is used to pass the RGB values of the leaf node
	//elements and the internal nodes attain the sum so they can be
	//used for internal nodes higher up.
	class RGBVal
	{
	public:
		//Constructor for the RGBVal class sets everything to zero.
		RGBVal()
		{
			R = 0;
			G = 0;
			B = 0;
			Alpha = 0;
		}

		unsigned long R; //The red channel of the leaf nodes
		unsigned long G; //The green channel of the leaf nodes
		unsigned long B; //The blue channel of the leaf nodes
		unsigned long Alpha; //The alpha channel of the leaf nodes

	};
	/*Rotates the Quadtree object's underlying image clockwise by 90 degrees.
	*/
	void clockwiseRotate();
	/*Compresses the image this Quadtree represents
	*/
	void prune(int tolerance);
	/*This function is similar to prune; however, it does not actually
	* prune the Quadtree.
	*/
	int pruneSize(int tolerance) const;
	/*Calculates and returns the minimum tolerance necessary to guarantee that upon
	* pruning the tree, no more than numLeaves leaves remain in the Quadtree.
	*/
	int idealPrune(int numLeaves) const;


private:

	// A simple class representing a single node of a Quadtree.
	// You may want to add to this class; in particular, it could probably
	// use a constructor or two...
	class QuadtreeNode
	{
	public:
		QuadtreeNode(); //default constructor
		QuadtreeNode(RGBAPixel const &source); //constructs with specified rgbapixel.	

		QuadtreeNode* nwChild;  // pointer to northwest child
		QuadtreeNode* neChild;  // pointer to northeast child
		QuadtreeNode* swChild;  // pointer to southwest child
		QuadtreeNode* seChild;  // pointer to southeast child

		RGBAPixel element;  // the pixel stored as this node's "data"
	};
	/*
	* Copy constructor helper function
	*/
	QuadtreeNode* copy(QuadtreeNode *current);
	/*
	* buildTree helper function
	*/
	QuadtreeNode* buildTreehelp(PNG const &source, int resolution, Point thePoint);
	/*helper function to find the pixel
	*/
	RGBAPixel getPixelhelp(int x, int y, QuadtreeNode *current, int resolution) const;
	/*
	* Destructor helper function.
	*/
	void clear(QuadtreeNode* current);
	/*
	* clockwise rorate helper function, needed to use recusrion to traverse tree
	*/
	QuadtreeNode* clockwiseRotatehelp(QuadtreeNode *current);
	/*helper function to find the averages of the
	* internal nodes
	*/
	RGBVal findAverages(RGBVal total, QuadtreeNode* current, int resolution);
	/*Helper function for prune since we need to traverse tree
	* with recursion.
	*/
	QuadtreeNode* pruneHelp(QuadtreeNode *current, int tolerance, int resolution);
	/*helper function to the helper function of pruneHelp
	*/
	bool pruneChecker(QuadtreeNode *Current, QuadtreeNode *ROOT, int tolerance, int resolution) const;
	/*Helper function to pruneSize very similar to the helper function of prune
	* but instead it returns an int.
	*/
	int prunesizeHelp(QuadtreeNode *current, int tolerance, int resolution) const;

	QuadtreeNode* root;    // pointer to root of quadtree
	int _resolution;		//resoultion of the quadtree



							/**** Functions added for testing/grading                ****/
							/**** Do not remove this line or copy its contents here! ****/
#include "quadtree_given.h"
};

#endif
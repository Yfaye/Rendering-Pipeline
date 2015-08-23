/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 3 Due: 3/5
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Shading and Illumination Modeling

ScanLineConstant class: 
				1) taking in the transformed vertex coordinates and using Edge Table and Active Edge Table to decide value for each pixcel in given screen size
				2) implementing Phong shading model
*/

#include "stdafx.h"
#include "EdgeTable_T.h"
#include <vector>


#ifndef SCANLINETEXTURE_H
#define SCANLINETEXTURE_H

using namespace std;

class ScanLineTexture{
public:
	//constructor takes in four arguments: the array contains all the polygon (the index of vertex that compose the polygon) vector< vector<int> > and visible polygons vector< vector<int> > , vertex coordinates vector< vector<float> > and world coordinates vector< vector<float> >
	ScanLineTexture(vector< vector<int> >, vector< vector<int> >, vector< vector<float> >, vector< vector<float> >);

	//pixel data for final draw function: pixel[m][n][0] = used to check whether the pixel has been assigned value (=1) or not (=0);
	//									  pixel[m][n][1] = value of z coordinate;
	//									  pixel[m][n][2]..pixel[m][n][4] = value of this pixel's R,G,B color
	vector< vector< vector<float> > > pixels;
	
	//set the size of the final display screen
	void setScreen(int, int, int, int);
	
	//calculate x, y value after round up
	void setXY(vector< vector<float> > &, vector< vector<float> >);

	//generate pixel values for final point drawing
	void setPixelValue();

	//for Edge table, sort the vertices by y value in one polygon
	void sortY(vector< vector<float> > &, int); 

	//match Neighbour vertices with current vertices which have been sorted by y value
	void getNeighbourVertex(vector< vector<float> >&, vector< vector<float>>&, vector< vector<float> >, int); 

	
	//put Ymax, X, xSlope, zSlope of one edge into Edge table
	void Edgetable(vector< vector<float> >, vector< vector<float> >, vector< vector<int> >, int);
	
	//read in edges from edge table, get X,Y coordinates and assign color value to each Pixel
	void activeEdgeTable(vector <EdgeTable_T>, int);

	//calculate vertex normal
	void getVertexNormal();

	//map loaded texture to object
	void textureMapping(float&, float &, vector<float>);

	//load image file
	void textureLoader(const char*);

	//compute pixel intensity
	void computePixelIntensity();

private:
	vector< vector<int> > poly; //
	vector< vector<int> > visible_Poly; // read in from ViewingTransform Class m_polys
	vector< vector<float> > trans_vertex;// read in from ViewingTransform Class m_verts
	vector< vector<float> > rounded_vertex;
	vector< vector < vector <float> > > pixel_Buffer; // for Z value and RGB valut interplolating

	//displaying window size
	int Xmin;
	int Ymin;
	int Xmax;
	int Ymax;

	//parameters for calculating illumination
	vector< vector<float> > vertex_normal; // vertex normal
	vector< vector <float> > world_vertex; // vertex X,Y,Z in the world coordinate system
	vector <vector <float> > lightIntensity; // store the calculated light intensity
	vector <float> eyePos; // direction of view
	vector <float> lightPos; // direction of light
	vector<float> h_vector; //normalized halfway reflector

	//light components
	float Kd; //diffusion term
	float Ks; //specular term
	float Ka; // ambient term

	//texture 
	unsigned int texture_width;
	unsigned int texture_height;
	unsigned char texture_red;
	unsigned char texture_green;
	unsigned char texture_blue;
	vector< vector < vector <float> > > texturePixels;
};

#endif
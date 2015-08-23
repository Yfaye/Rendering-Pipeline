/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

LoadFile Class: Loading the .d file into arrays
	
	attributes:
		vertex_num : the number of vertex in the .d file
		polygon_num: the number of ploygons in the .d file
		polygonVertex_num: the number of vertex that compose each polygon
		allVertex: the array contains all the vertex in x,y,z,1 format for each row;
		allPolygon: the array contains all the polygon (the index of vertex that compose the polygon);

	methods:
		void loadModel(string datafileName): read in the vertex data and polygon data in .d file
					  input: name of the .d file in the string format, e.g, "house.d"
		
*/


#ifndef LOADFILE_H
#define LOADFILE_H


#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class LoadFile
{
public:

	int vertex_num; // number of vertex in the .d file
	int polygon_num; // number of ploygons in the .d file
	int polygonVertex_num; // number of vertex in each polygon

	vector<vector<float>> allVertex; // vertex in local coordinates given by the x, y, z
	vector<vector<int>> allPolygon; // ploygon given by vertex number in clockwise order(when looking from outside the object)

	// methods
	void loadModel(string datafileName); // read in the vertex data and polygon data in .d file
};

#endif


/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

Implementing LoadFile.h: Loading the .d file into arrays
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

#include "stdafx.h"
#include "LoadFile.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void LoadFile::loadModel(string datafile){

	string data;
	int vertexIndex;
	vector<float> vertex(4);



	////open the .d file

	ifstream in(datafile.c_str());

	if (!in)
	{
		cout << "Failed in loading model file!" << endl;
	}

	in >> data >> vertex_num >> polygon_num;

	//load all the vertex and put into the array: allVertex
	for (int i = 0; i < vertex_num; i++)
	{
		in >> vertex[0] >> vertex[1] >> vertex[2];
		vertex[3] = 1;
		allVertex.push_back(vertex);
	}

	//load all the polygon and put into the array: allPolygon

	for (int i = 0; i < polygon_num; i++)
	{
		in >> polygonVertex_num;
		vector<int> polygon;
		polygon.push_back(polygonVertex_num);
		for (int j = 0; j < polygonVertex_num; j++)
		{
			in >> vertexIndex;
			polygon.push_back(vertexIndex);
		}
		allPolygon.push_back(polygon);
	}
	in.close();
}

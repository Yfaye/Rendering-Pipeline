/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 3 Due: 3/5
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Shading and Illumination Modeling

EdgeTable_T.class: 
	generate Edge Table containing all edges with given data from each polygon in Phong Shading Model
*/

#include "stdafx.h"
#include <vector>

#ifndef Edge_T_H_INCLUDED
#define Edge_T_H_INCLUDED

using namespace std;

class Edge_T{
public:
	vector<float>* edgeValue;
	Edge_T()
	{
		edgeValue = new vector<float>();
	};
	void setEdgeValue(float ymax, float xStart, float xSlope, float zStart, float zSlope, float nxStart, float nxSlope, float nyStart, float nySlope, float nzStart, float nzSlope) 
	
	{
		edgeValue->clear();
		edgeValue->push_back(ymax);
		edgeValue->push_back(xStart);
		edgeValue->push_back(xSlope);
		edgeValue->push_back(zStart);
		edgeValue->push_back(zSlope);

		// for Phong shading model : store the interpolated Normal value in x,y,z
		edgeValue->push_back(nxStart); 
		edgeValue->push_back(nxSlope); 
		edgeValue->push_back(nyStart);
		edgeValue->push_back(nySlope); 
		edgeValue->push_back(nzStart); 
		edgeValue->push_back(nzSlope);
	}
	~Edge_T()
	{
		delete edgeValue;
	};
};
#endif


#ifndef EdgeTable_T_H_INCLUDED
#define EdgeTable_T_H_INCLUDED

class EdgeTable_T{
public:
	//count the number of edges with same ymax
	int count;
	vector < Edge_T* > tempEdge;
	EdgeTable_T()
	{
		count = 0;
	}

	void insertValue(float ymax, float xStart, float xSlope, float zStart, float zSlope, float nxStart, float nxSlope, float nyStart, float nySlope, float nzStart, float nzSlope) 
	{
		Edge_T* edgeValue = new Edge_T();
		edgeValue->setEdgeValue(ymax, xStart, xSlope, zStart, zSlope, nxStart, nxSlope, nyStart, nySlope, nzStart, nzSlope); 
		tempEdge.push_back(edgeValue);
		count++;
	}

	vector<float>* getEdgeValue(int index)
	{
		return tempEdge[index]->edgeValue;
	}

};
#endif 
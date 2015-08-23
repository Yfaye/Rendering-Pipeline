/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

MatrixTool Class: Vector and Matrix operation

	static methods:
		vector_sum: calculate the sum of two vectors
		vector_subtraction:calculate the subtraction of two vectors;
		vector_module: calculate module of the vector;
		vector_dot_product: calculate the dot product of two vectors;
		vector_cross_product: calculate the cross product of two vectors;
		matrix_vector_product: calculate the result of 4x4 matrix and one vector multiplication;
		matrix_4x4: calculate the result of two 4x4 matrices multiplication;

*/

#ifndef MATRIXTOOL_H
#define MATRIXTOOL_H

#include "stdafx.h"
#include <vector>

using namespace std;

class MatrixTool{

public:
	//calculate the sum of two vectors
	static void vector_sum(vector<float> &, vector<float>, vector<float>);

	//calculate the subtraction of two vectors
	static void vector_subtraction(vector<float> &, vector<float>, vector<float>);
	
	//calculate module of the vector;
	static float vector_module(vector<float>);

	//calculate the dot product of two vectors
	static float vector_dot_product(vector<float>, vector<float>);

	//calculate the cross product of two vectors
	static void vector_cross_product(vector<float> &, vector<float>, vector<float>);

	//calculate the result of 4x4 matrix and one vector multiplication
	static void matrix_vector_product(vector <float> &, vector< vector<float> >, vector <float>);

	//calculate the result of two 4x4 matrices multiplication
	static void matrix_4x4(vector< vector<float> >&, vector< vector<float> >, vector< vector<float> >);
	
	//normalize the vector;
	static float vector_normalize(vector<float>&);

};
#endif
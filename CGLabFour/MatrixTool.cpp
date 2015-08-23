/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

Implementing the MatrixTool Class: Vector and Matrix operation

	static methods:
		vector_sum: calculate the sum of two vectors
		vector_sub:calculate the subtraction of two vectors;
		vector_module: calculate module of the vector;
		vector_dot_product: calculate the dot product of two vectors;
		vector_cross_product: calculate the cross product of two vectors;
		matrix_vector_product: calculate the result of 4x4 matrix and one vector multiplication;
		matrix_4x4: calculate the result of two 4x4 matrices multiplication;

*/

#include "stdafx.h"
#include "MatrixTool.h"
#include <vector>
#include <math.h>

using namespace std;

//calculate the sum of two vectors: vector_2 - vector_1
void MatrixTool::vector_sum(vector<float> &sumresult, vector<float> vector_1, vector<float> vector_2){


	for (int i = 0; i < 3; i++)
		sumresult[i] = vector_2[i] + vector_1[i];
}


//calculate module of the vector;
float MatrixTool::vector_module(vector<float> vector_1){
	return sqrt(vector_1[0] * vector_1[0] + vector_1[1] * vector_1[1] + vector_1[2] * vector_1[2]);
}

//calculate the subtraction of two vectors: vector_2 - vector_1
void MatrixTool::vector_subtraction(vector<float> &sub, vector<float> vector_1, vector<float> vector_2){
	
	
	for (int i = 0; i < 3; i++)
		sub[i] = vector_2[i] - vector_1[i];
}

//calculate the dot product of two vectors
float MatrixTool::vector_dot_product(vector<float> vector_1, vector<float> vector_2){
	return vector_1[0] * vector_2[0] + vector_1[1] * vector_2[1] + vector_1[2] * vector_2[2];
}

//calculate the cross product of two vectors
void MatrixTool::vector_cross_product(vector<float> &normal, vector<float>vector_1, vector<float>vector_2){
	//compute cross product
	normal[0] = vector_1[1] * vector_2[2] - vector_2[1] * vector_1[2];
	normal[1] = -(vector_1[0] * vector_2[2] - vector_2[0] * vector_1[2]);
	normal[2] = vector_1[0] * vector_2[1] - vector_2[0] * vector_1[1];
}

//calculate the result of 4x4 matrix and one vector multiplication
void MatrixTool::matrix_vector_product(vector <float> &matrix_out, vector< vector<float> >matrix_L, vector <float>matrix_R){
	for (int i = 0; i < 4; i++){
		matrix_out[i] =
			matrix_L[i][0] * matrix_R[0] +
			matrix_L[i][1] * matrix_R[1] +
			matrix_L[i][2] * matrix_R[2] +
			matrix_L[i][3] * matrix_R[3];
	}
}

//calculate the result of two 4x4 matrices multiplication
void MatrixTool::matrix_4x4(vector< vector<float> > &matrix_out, vector< vector<float> >matrix_L, vector< vector<float> >matrix_R){
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
		matrix_out[i][j] =
			matrix_L[i][0] * matrix_R[0][j] +
			matrix_L[i][1] * matrix_R[1][j] +
			matrix_L[i][2] * matrix_R[2][j] +
			matrix_L[i][3] * matrix_R[3][j];
		}
}

//normalize the vector;
static float vector_normalize(vector<float>& toBeNormalized){
	float temp_length = sqrt(toBeNormalized[0] * toBeNormalized[0] + toBeNormalized[1] * toBeNormalized[1] + toBeNormalized[2] * toBeNormalized[2]);
	for (int i = 0; i<3; i++){
		toBeNormalized[i] = toBeNormalized[i] / temp_length;

	}

}

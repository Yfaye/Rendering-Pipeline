/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

	ViewTransform Class: according to given viewing specification, transform a geometric data of a polygonal object 
						 to generate Perspective view of the object displayed on the viewport with back faces removed
*/
#include "stdafx.h"
#include <vector>

#ifndef VIEWINGTRANSFORM_H
#define VIEWINGTRANSFORM_H

#pragma once

using namespace std;

class ViewingTransform{
	public:
		//constructor
		ViewingTransform();
		//virtual
		virtual ~ViewingTransform();

		//====================== Attributes========================
		//arrays loaded from .d file
		vector< vector<float> > m_verts; //the array contains all the vertex in x,y,z,1 format for each row;
		vector< vector<int> > m_polys; //the array contains all the polygon (the index of vertex that compose the polygon);

		//arrays store vertex X,Y,Z value in the world coordinate system
		vector< vector<float> > world_verts;

		//arrays stored visible polygon index and vertex index after back-face culling
		vector<int>  visible_polygon_index;//the index of all visible polygon 
		vector< vector<int> > visible_polygon;//the vertex data of all visible polygon

		//====================== Methods========================

		//set viewing specification

		void setObjectPosition(float, float, float); //set the Object position in world coordinate

		void setCameraPosition(float, float, float); ////set the Camera position in world coordinate

		void setUpVec(float, float, float); //set the up vector for camera

		void setNear(float); //set near clipping distance

		void setFar(float); //set far clipping distance

		void setHeight(float); //set h for perspective matrix

		void set2DWindow(float, float, float, float); //set 2D window Xmin, Ymin, Xmax, Ymax

		void setProjectRef(float, float, float); 		//set project reference for camera

		void view_N(vector<float> &, vector<float>, vector<float>); //compute N vector for camera

		void view_U(vector<float> &, vector<float>, vector<float>); //compute U vector for camera

		void view_V(vector<float> &, vector<float>, vector<float>); //compute V vector for camera


		//calculate M_model, M_view and M_pers

		void matrixModel(vector< vector<float> >&, vector<float>);//calculate modeling transformation matrix

		void matrixView(vector< vector<float> >&, vector<float>, vector<float>);	//calculate viewing transformation Matrix

		void matrixPers(vector< vector<float> >&, float, float, float);	//calculate projection transformation matrix


		//calculate vertex data after view tranform with back-face culling and normalization
		vector< vector<float> > viewTransform();

		//draw lines between visible vertex
		void drawModel();


	private:
		//====================== Attributes========================
		vector <float> objectToWorld;//object shift from local to world
		vector <float> camera; //camera position in world
		vector <float> up_vector;//up vector for camera
		vector<float> P_ref;//project reference for camera

		float h;//for perspective matrix
		float nearClip;//near clipping plane
		float farClip;//far clipping plane
		
		//set a 2D window in the perspective plane
		float Xmin;
		float Ymin;
		float Xmax;
		float Ymax;

		vector< vector<float> > m_model;//Model transform Matrix
		vector< vector<float> > m_view;//view transformation Matrix
		vector< vector<float> > m_pers;//Perspective transform Matrix
};
#endif
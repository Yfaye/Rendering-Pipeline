/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 3 Due: 3/5
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Shading and Illumination Modeling

Implementing ScanLineConstant class:
	1) taking in the transformed vertex coordinates and using Edge Table and Active Edge Table to decide value for each pixcel in given screen size
	2) implementing Phong Shading model
*/

#include "stdafx.h"
#include "MatrixTool.h"
#include "ViewingTransform.h"
#include "EdgeTable_T.h"
#include "ScanLineTexture.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <fstream>
#define PI 3.14159265
//========================================================================================
//initilize screen size and set X,Y coordinates
//========================================================================================

//constructor takes in two arguments: visible polygons vector< vector<int> > and vertex coordinates vector< vector<float> >
ScanLineTexture::ScanLineTexture(vector< vector<int> > Poly, vector< vector<int> > visiblePoly, vector< vector<float> > transVertex, vector< vector<float> > worldVertex){
	//initialize the class with visible polygons and vertices data from the ViewingTransform class
	poly = Poly;
	visible_Poly = visiblePoly;
	trans_vertex = transVertex;
	world_vertex = worldVertex;

	//set value for x,y coordinates and put them into rounded_vertex
	setXY(rounded_vertex, trans_vertex);

	//set lighting parameters
	Kd = 0.2; // diffuse reflectivity of the surface
	Ks = 0.05; // specular reflectivity of the surface
	Ka = 0.01; // ambient term 
	
	//set light position and eye position
	vector<float> temp_lightPos(3);
	temp_lightPos[0] = -1;
	temp_lightPos[1] = 2;
	temp_lightPos[2] = 2;
	lightPos = temp_lightPos;
	vector<float> temp_eyePos(3);
	temp_eyePos[0] = 0;
	temp_eyePos[1] = 1;
	temp_eyePos[2] = 5;
	eyePos = temp_eyePos;
}

void ScanLineTexture::setScreen(int xmin, int xmax, int ymin, int ymax)
{   //set screen size
	Xmin = xmin;
	Xmax = xmax;
	Ymin = ymin;
	Ymax = ymax;

	//initialize pixels matrix and pixel_zBuffer matrix
	vector< vector < vector <float> > > temp(Ymax, vector< vector< float > >(Xmax, vector < float >(5)));
	for (int i = 0; i< Ymax; i++)
		for (int j = 0; j< Xmax; j++)
			temp[i][j][1] = -1000;
	pixel_Buffer = temp;
	pixels = temp;
}

//turn transformed object vertex into the smallest integer value not less than itself (round up)
void ScanLineTexture::setXY(vector< vector<float> > &rounded_vertex, vector< vector<float> > transVertex)
{
	
	rounded_vertex = transVertex;

	for (int i = 0; i<(int)rounded_vertex.size(); i++)
	{
		if (transVertex[i].size() == 5)
		{
			for (int j = 0; j < 2; j++)
			{
				// round up x,y coordinates
				rounded_vertex[i][j] = ceil(trans_vertex[i][j]);
			}
		}
	}
}

//========================================================================================
//calculate the light intensity
//========================================================================================
//calculate vertex normal
void ScanLineTexture::getVertexNormal()
{
	vector <float> temp_vector_1(3); // store vector coordinates to calculate polygon normal
	vector <float> temp_vector_2(3); // store store vector coordinates to calculate polygon normal
	vector <float> polygon_normal(3); // calculated polygon normal
	vector <vector <float>>temp_vertex_normal(world_vertex.size(), vector<float>(3)); //calculated vertex normal

	//calculate the polygon normal and the vertex normal
	for (int i = 0; i < (int)poly.size(); i++)
	{
		MatrixTool::vector_subtraction(temp_vector_1, world_vertex[poly[i][2] - 1], world_vertex[poly[i][1] - 1]);
		MatrixTool::vector_subtraction(temp_vector_2, world_vertex[poly[i][2] - 1], world_vertex[poly[i][3] - 1]);
		MatrixTool::vector_cross_product(polygon_normal, temp_vector_1, temp_vector_2); //polygon normal

		//vertex normal
		for (int j = 1; j <= (int)poly[i][0]; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				temp_vertex_normal[poly[i][j] - 1][k] += polygon_normal[k];
			}
		}
	}

	for (int i = 0; i < (int)temp_vertex_normal.size(); i++)
	{
		float temp_module = MatrixTool::vector_module(temp_vertex_normal[i]);
		for (int j = 0; j < 3; j++)
		{
			temp_vertex_normal[i][j] = temp_vertex_normal[i][j] / temp_module;
		}

	}
	vertex_normal = temp_vertex_normal;
}

//calculate the light intensity using the Phong Shading model
void ScanLineTexture::computePixelIntensity(){
	
	vector <float> Ld(3); // diffuse light intensity
	vector <float> Ls(3); // specular light intensity
	vector <float> La(3); // ambient light intensity
	vector <vector <float>> temp_intensity(world_vertex.size(), vector<float>(3)); // sum of diffuse, specular and ambient light intensity for each vertex
	float u, v;

	//set the color of the light
	vector <float> lightColor(3);
	lightColor[0] = 1.0; //Red
	lightColor[1] = 1.0; //Green
	lightColor[2] = 1.0; //Blue

	//calculate the halfway reflector
	vector<float> unnormalized_h(3); // calculated halfway reflector
	MatrixTool::vector_sum(unnormalized_h, lightPos, eyePos); //
	for (int i = 0; i < 3; i++)
	h_vector.push_back (unnormalized_h[i] / MatrixTool::vector_module(unnormalized_h)); //normalization


	//calculate Ld, Ls and La and sum to the light intensity for each pixel
	for (int i = 0; i < Ymax; i++)
	{
		for (int j = 0; j < Xmax; j++)
		{
			if (pixels[i][j][0]==1){
				vector <float> pixel_normal(pixels[i][j].begin() + 2, pixels[i][j].end());
				
				//MatrixTool::vector_normalize(pixel_normal);
				//normalize pixel_normal
				float temp_length = sqrt(pixel_normal[0] * pixel_normal[0] + pixel_normal[1] * pixel_normal[1] + pixel_normal[2] * pixel_normal[2]);
				for (int temp_k = 0; temp_k<3; temp_k++){
					pixel_normal[temp_k] = pixel_normal[temp_k] / temp_length;

				}
				
				textureMapping(u, v, pixel_normal);

				for (int k = 0; k < 3; k++)
				{
					Ld[k] = Kd * lightColor[k] * MatrixTool::vector_dot_product(pixel_normal, lightPos);
					Ls[k] = Ks * lightColor[k] * pow(MatrixTool::vector_dot_product(pixel_normal, h_vector), 2);
					La[k] = Ka * lightColor[k];

					pixels[i][j][k + 2] = Ld[k] + Ls[k] + La[k] + texturePixels[(int)v][(int)u][k];
					if (pixels[i][j][k + 2] > 1) // to avoid violating the energy conservation
					{
						pixels[i][j][k + 2] = 1.0;
					}
				}
			}
		}

	}
}

//========================================================================================
//Texture Loading and Texture Mapping: 
//			load .bmp file as texture;
//			map the loaded texture file to object;
//========================================================================================
//
void ScanLineTexture::textureLoader(const char* filename){
	FILE *image;
	errno_t err;
	err = fopen_s(&image, filename, "rb+");
	//if cannot find the file
	if (err!= 0)
	{
		cerr << "Texture could not be opened" << endl;
		exit(1);
	}
	fseek(image, 18, SEEK_SET);
	fread(&texture_width, 4, 1, image); //get width pixel number
	fseek(image, 22, SEEK_SET);
	fread(&texture_height, 4, 1, image);//get height pixel number
	fseek(image, 54, SEEK_SET); //move to Pixel data
	vector< vector < vector <float> > > temp_pixels(texture_height, vector< vector< float > >(texture_width, vector < float >(3)));

	//read in the color of texture in BGR sequence and assigned to texturePixels with RGB color
	for (int i = 0; i< (int)texture_height; i++){
		for (int j = 0; j < (int)texture_width; j++){
			fread(&texture_blue, sizeof(texture_blue), 1, image);
			fread(&texture_green, sizeof(texture_green), 1, image);
			fread(&texture_red, sizeof(texture_red), 1, image);
			//cout << (float)texture_red << " ";
			//cout << (float)texture_green << " ";
			//cout << (float)texture_blue << endl;
			temp_pixels[i][j][0] = (float)texture_red / 255;
			temp_pixels[i][j][1] = (float)texture_green / 255;
			temp_pixels[i][j][2] = (float)texture_blue / 255;
			//cout << temp_pixels[i][j][0] << " ";
			//cout << temp_pixels[i][j][1] << " ";
			//cout << temp_pixels[i][j][2] << endl;
		}
	}
	texturePixels = temp_pixels;
	fclose(image);
}

void ScanLineTexture::textureMapping(float &u, float &v, vector<float> pixel_normal){
	float theta, r;
	r = texture_width / (2 * PI);
	theta = acos(pixel_normal[0]) * 180.0 / PI;
	u = (theta / 180) * texture_width;
	v = (pixel_normal[1] + 1) * texture_height / 2;
}

//========================================================================================
//Set the pixcel's value in the pixel matrixl: 
//			For each polygon:
//				1. sort vertecies by their Y value;
//				2. determine the edge by match current vertex and with its two neighbouring vertecies;
//				3. add edges to edge table in the sequence of sorted Y value (put Ymax, X, xSlope, zSlope of each edge into Edge table)
//				4. run activeEdgeTable process to start scanline fill and determine the color of each pixel
//========================================================================================
//
void ScanLineTexture::setPixelValue()
{	
	//calculate the Vertex Normal
	getVertexNormal();

	//put y value and vertex index in temp_y
	vector< vector<float>> temp_y;
	vector< vector<float>> nearVertex;
	vector< vector<float>> startVertex;

	//for each polygon : 
	for (int poly_num = 0; poly_num< (int)visible_Poly.size(); poly_num++){
		temp_y.clear();
		startVertex.clear();
		nearVertex.clear();

		//sort the y value of all the vertices in a polygon
		sortY(temp_y, poly_num);

		//find vertices neighbouring current vertice to define the edge
		getNeighbourVertex(nearVertex, startVertex, temp_y, poly_num);

		//create Edgetable and Active Edge Table to fill value into the pixel matrix
		Edgetable(startVertex, nearVertex, visible_Poly, poly_num);
	}
	computePixelIntensity();
}

// sort the value of Y of all the vertices in one polygon
void ScanLineTexture::sortY(vector< vector<float>> & temp_y, int poly_index)
{
	// loop in all the vertices in one polygon
	for (int i = 1; i <= visible_Poly[poly_index][0]; i++)
	{
		vector<float> temp;
		//read in the index of vertice in one polygon
		temp.push_back(visible_Poly[poly_index][i] - 1);
		//cout << visible_Poly[poly_index][i] - 1 << " ";

		//read in the rounded y value of vertices in one polygon
		temp.push_back(rounded_vertex[visible_Poly[poly_index][i] - 1][1]);
		//cout << rounded_vertex[visible_Poly[poly_index][i] - 1][1] << " ";

		//read in the index of neighboring vertices: previous vertex and next vertex
		//for the start vertex
		if (i == 1)
		{
			temp.push_back(visible_Poly[poly_index][visible_Poly[poly_index][0]] - 1); //prev
			temp.push_back(visible_Poly[poly_index][i + 1] - 1); //next
			//cout << visible_Poly[poly_index][visible_Poly[poly_index][0]] - 1 << " ";
			//cout << visible_Poly[poly_index][i + 1] - 1 << endl;
		}
		//for the end vertex
		else if (i == visible_Poly[poly_index][0])
		{
			temp.push_back(visible_Poly[poly_index][i - 1] - 1); //prev
			temp.push_back(visible_Poly[poly_index][1] - 1); //next
			//cout << visible_Poly[poly_index][i - 1] - 1 << " ";
			//cout << visible_Poly[poly_index][1] - 1 << endl;
		}
		else
		{
			temp.push_back(visible_Poly[poly_index][i - 1] - 1); //prev
			temp.push_back(visible_Poly[poly_index][i + 1] - 1); //next
			//cout << visible_Poly[poly_index][i - 1] - 1 << " ";
			//cout << visible_Poly[poly_index][i + 1] - 1 << endl;
		}
		// each row of temp_y contains: index of current vertex, rounded Y value of current vertex, index of previous vertex, index of next vertex
		temp_y.push_back(temp);

	}

	//sort all the vertices by y value
	bool flag = false;
	for (int i = 0; i<(int)temp_y.size() - 1; i++){
		for (int j = 0; j<(int)temp_y.size() - i - 1; j++){
			if (temp_y[j][1] > temp_y[j + 1][1]){
				swap(temp_y[j], temp_y[j + 1]);
				flag = true;
			}
		}
		if (flag)
			flag = false;
		else
			break;
	}
}


// find the vertices x,y coordinates and vertex normal of vertices neighbouring the current vertice which has been sorted by y value
void ScanLineTexture::getNeighbourVertex(vector< vector<float> > &neighbourVertex, vector< vector<float> > &currentVertex, vector< vector<float> >temp_y, int poly_num){


	for (int i = 0; i < (int)temp_y.size(); i++)
	{
		vector< float > temp_0(rounded_vertex[temp_y[i][0]]);
		temp_0[4] = temp_y[i][0];//real index
		temp_0.insert(temp_0.end(), vertex_normal[temp_y[i][0]].begin(), vertex_normal[temp_y[i][0]].end());
		currentVertex.push_back(temp_0); 
		
		vector< float > temp_1(rounded_vertex[temp_y[i][2]]); // store the rounded x, y coordinates for previous vertex
		vector< float >temp_2(rounded_vertex[temp_y[i][3]]); // store the rounded x, y coordinates for next vertex
		temp_1[4] = temp_y[i][2];
		temp_2[4] = temp_y[i][3];
		temp_1.insert(temp_1.end(), vertex_normal[temp_y[i][2]].begin(), vertex_normal[temp_y[i][2]].end());
		temp_2.insert(temp_2.end(), vertex_normal[temp_y[i][3]].begin(), vertex_normal[temp_y[i][3]].end());

		neighbourVertex.push_back(temp_1); 
		neighbourVertex.push_back(temp_2);
	}
}


//=====================================================================================
//Setting up: Edgetable and Active Edgetable 
//=====================================================================================

//read in edge value according to currentVertex and its neighbourVertex, store Ymax, Xmin, xIncrease and zIncrease to the edge table
void ScanLineTexture::Edgetable(vector< vector<float> > currentVertex, vector< vector<float> > neighbourVertex, vector< vector<int> >front_Poly, int poly_num)
{
	//vector<float> edgeValue;
	vector <EdgeTable_T> ET(Ymax);
	vector<int> usedVertex;//array store the index of verteices that have allready been added into the edgetable
	float xSlope;
	float zSlope;
	// for Phong Shading model set normal interpolating slope in X,Y,Z
	float NXSlope; 
	float NYSlope; 
	float NZSlope;

	for (int i = 0; i< (int)currentVertex.size(); i++)
	{
		for (int j = 0; j<2; j++)
		{
			//edgeValue.clear();
			//check if one edge is already in edgetable by comparing the sum of index of current vertex and each of its two neighbourVertex (prev and next)
			if (find(usedVertex.begin(), usedVertex.end(), currentVertex[i][4] * neighbourVertex[i * 2 + j][4]) != usedVertex.end())
				continue;
			//if they have not been used, add the sum of index in the check vector
			else
				usedVertex.push_back(currentVertex[i][4] * neighbourVertex[i * 2 + j][4]);
			//check if they are forming an horizontal edge and if they are, delete the horizontal edge: current Vertex and each of its two neighbourVertex have the same Y value but different X value 
			if ((currentVertex[i][1] == neighbourVertex[i * 2 + j][1]) && currentVertex[i][0] != neighbourVertex[i * 2 + j][0])
			{
				continue;
			}
			//check if the verteices are overlapped : currentVertex and each of its two neighbourVertex have the same X,Y value
			else if ((currentVertex[i][1] == neighbourVertex[i * 2 + j][1]) && currentVertex[i][0] == neighbourVertex[i * 2 + j][0])
			{
				xSlope = 0;
			}
			else
				//calculate x slope: 1/m 
				xSlope = (currentVertex[i][0] - neighbourVertex[i * 2 + j][0]) / (currentVertex[i][1] - neighbourVertex[i * 2 + j][1]);

			//store y max value
			//edgeValue.push_back(neighbourVertex[i * 2 + j][1]);

			//store the x value of current vertex
			//edgeValue.push_back(currentVertex[i][0]);
			//edgeValue.push_back(xSlope);

			//calculate z slope and normal at X,Y,Z's slope
			//check if the verteices are in the same hight: avoiding divided by 0
			if (currentVertex[i][1] == neighbourVertex[i * 2 + j][1]){
				zSlope = 0;
				NXSlope = 0;
				NYSlope = 0;
				NZSlope = 0;

			}
			else
			{
				zSlope = (neighbourVertex[i * 2 + j][2] - currentVertex[i][2]) / (neighbourVertex[i * 2 + j][1] - currentVertex[i][1]);
				NXSlope = (neighbourVertex[i * 2 + j][5] - currentVertex[i][5]) / (neighbourVertex[i * 2 + j][1] - currentVertex[i][1]);
				NYSlope = (neighbourVertex[i * 2 + j][6] - currentVertex[i][6]) / (neighbourVertex[i * 2 + j][1] - currentVertex[i][1]);
				NZSlope = (neighbourVertex[i * 2 + j][7] - currentVertex[i][7]) / (neighbourVertex[i * 2 + j][1] - currentVertex[i][1]);
			}
			//put Ymax, X, xSlope, zSlope of one edge into Edge table
			ET[currentVertex[i][1]].insertValue(neighbourVertex[i * 2 + j][1],
				currentVertex[i][0], xSlope, 
				currentVertex[i][2], zSlope,
				currentVertex[i][5], NXSlope,
				currentVertex[i][6], NYSlope,
				currentVertex[i][7], NZSlope);
		}
	}
	//go to Active Edge Table process
	activeEdgeTable(ET, poly_num);
}

//activeEdgeTable: read in edges from edge table, get X,Y coordinates and assign color value to each Pixel
void ScanLineTexture::activeEdgeTable(vector <EdgeTable_T> ET, int poly_num){

	//initialize Active Edge Table
	vector< vector<float>* > AET;
	int start = 0;
	float zBuffer;
	float nxBuffer;//normal x
	float nyBuffer;//normal y
	float nzBuffer;//normal z
	vector<float> temp;

	//filling Active Edge Table
	for (int y_min = 0; y_min< (int)ET.size(); y_min++) {
		//check if the Edge Table is empty
		if (ET[y_min].count == 0 && start == 0)
			continue;
		//if the Edge Table is NOT empty
		else{
			start = 1;
			//read edge value from ET into AET
			for (int i = 0; i< ET[y_min].count; i++){
				AET.push_back(ET[y_min].getEdgeValue(i));
			}

			//Remove from AET entries of which y = ymax
			for (int j = 0; j< (int)AET.size(); j++){
				if ((*AET[j])[0] == y_min){
					AET.erase(AET.begin() + j);
					j--;
				}
			}

			//odd-even rule
			if ((int)AET.size() % 2 != 0){
				AET.erase(AET.begin());
			}

			//check if the active edge table is empty
			if (AET.size() == 0)
				break;
			//if the AET is NOT empty, sort AET by x value
			bool flag = false;
			for (int i = 0; i < (int)AET.size() - 1; i++){
				for (int j = 0; j <(int)AET.size() - i - 1; j++){
					if ((*AET[j])[1] >(*AET[j + 1])[1]){
						swap(AET[j], AET[j + 1]);
						flag = true;
					}
				}
				if (flag)
					flag = false;
				else
					break;
			}

			for (int j = 0; j< (int)AET.size(); j += 2){
				if (ceil((*AET[j])[1]) == ceil((*AET[j + 1])[1]))
				{
					zBuffer = 0;
					nxBuffer = 0;
					nyBuffer = 0;
					nzBuffer = 0;
				}
				else
				{
					zBuffer = ((*AET[j + 1])[3] - (*AET[j])[3]) / (ceil((*AET[j + 1])[1]) - ceil((*AET[j])[1]));
					nxBuffer = ((*AET[j + 1])[5] - (*AET[j])[5]) / (ceil((*AET[j + 1])[1]) - ceil((*AET[j])[1]));
					nyBuffer = ((*AET[j + 1])[7] - (*AET[j])[7]) / (ceil((*AET[j + 1])[1]) - ceil((*AET[j])[1]));
					nzBuffer = ((*AET[j + 1])[9] - (*AET[j])[9]) / (ceil((*AET[j + 1])[1]) - ceil((*AET[j])[1]));
				}
				for (int X_value = (int)ceil((*AET[j])[1]); X_value <= (int)floor((*AET[j + 1])[1]); X_value++){
					//Z value
					if (X_value == ceil((*AET[j])[1]))
					{ 
						pixel_Buffer[y_min][X_value][1] = (*AET[j])[3];
						pixel_Buffer[y_min][X_value][2] = (*AET[j])[5];
						pixel_Buffer[y_min][X_value][3] = (*AET[j])[7];
						pixel_Buffer[y_min][X_value][4] = (*AET[j])[9];
					}
					else
					{ 
						pixel_Buffer[y_min][X_value][1] = pixel_Buffer[y_min][X_value - 1][1] + zBuffer;
						pixel_Buffer[y_min][X_value][2] = pixel_Buffer[y_min][X_value - 1][2] + nxBuffer;
						pixel_Buffer[y_min][X_value][3] = pixel_Buffer[y_min][X_value - 1][3] + nyBuffer;
						pixel_Buffer[y_min][X_value][4] = pixel_Buffer[y_min][X_value - 1][4] + nzBuffer;
					}
					if (pixels[y_min][X_value][0] == 0)
					{
						//if there doesn't exit a pixel
						pixels[y_min][X_value][0] = 1;
						pixels[y_min][X_value][1] = pixel_Buffer[y_min][X_value][1];
						pixels[y_min][X_value][2] = pixel_Buffer[y_min][X_value][2];
						pixels[y_min][X_value][3] = pixel_Buffer[y_min][X_value][3];
						pixels[y_min][X_value][4] = pixel_Buffer[y_min][X_value][4];
					}
					else if (pixel_Buffer[y_min][X_value][1] < pixels[y_min][X_value][1])
					{
						//while exist a nearer pixel
						pixels[y_min][X_value][1] = pixel_Buffer[y_min][X_value][1];
						pixels[y_min][X_value][2] = pixel_Buffer[y_min][X_value][2];
						pixels[y_min][X_value][3] = pixel_Buffer[y_min][X_value][3];
						pixels[y_min][X_value][4] = pixel_Buffer[y_min][X_value][4];
					}
				}
			}

			//update x and z
			for (int j = 0; j< (int)AET.size(); j++){
				//update x by x + xSlope
				(*AET[j])[1] = (*AET[j])[1] + (*AET[j])[2];
				//update z by z + xSlope
				(*AET[j])[3] = (*AET[j])[3] + (*AET[j])[4];
				//update Normal at X by NX + nxSlope
				(*AET[j])[5] = (*AET[j])[5] + (*AET[j])[6];
				//update Normal at Y  by NY + nySlope
				(*AET[j])[7] = (*AET[j])[7] + (*AET[j])[8];
				//update Normal at Z by NZ + nzSlope
				(*AET[j])[9] = (*AET[j])[9] + (*AET[j])[10];
			}
		}
	}
}
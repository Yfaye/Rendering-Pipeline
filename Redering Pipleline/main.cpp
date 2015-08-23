/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 4 Due: 4/9
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Texture Mapping
=============================================================================================
Description: Implement the z-buffer algorithm. Each polygon should be constant colored differently.
Input:	a) Geometric data for a polygonal object 
			(files are from http://www.icg.gwu.edu/cs266/resources/model_osu/model_osu.html).
		b) Viewing specification
		c) Lighting parameters
		d) Texture map
Output: Texture mapped, Phong shaded view of objects. 
=============================================================================================

Read Me:
	Five classes are involved:
			LoadFile:  Loading the .d file into arrays: 
							allVertex: the array contains all the vertex in x,y,z,1 format for each row;
							allPolygon: the array contains all the polygon (the index of vertex that compose the polygon);
			MatrixTool: Matrix and Vector calculation tool:
							vector_subtraction, vector_module, 
							vector_dot_product, vector_cross_product, 
							matrix_vector_product, matrix_4x4;		
			ViewingTransform: With given viewing specification, transform the loaded data of a polygonal object 
							  to generate Perspective view of the object displayed on the viewport with back faces removed.
			EdgeTable:
			ScanlineTexture:
	Steps: 
	        ------------------------code in Lab 1 -------------------------------------------
			1. load in the data of object in local coordinate system
			1. assign the polygon to the world coordinate system (position)
			2. set the camera in the world coordinates (position, up-vector, perspective)
			3. transform the loaded data of polygon to camera's view space (3D)
			4. perform back face culling in the camera's view space £¨3D£©
			5. update the arrays stored the vertex index of all visible polygon and vertex X,Y,Z value after transforming
			------------------------code in Lab 2 -------------------------------------------
			6. read in the arrays stored the vertex index of all visible polygon and vertex X,Y,Z value after transforming
			7. according to given screen setting turn vertex coordinates into the smallest integer value not less than itself (round up)
			8. Set the pixcels value in the pixel matrixl: 
 				For each polygon:
 				1) sort vertecies by their Y value;
 				2) determine the edge by match current vertex and with its two neighbouring vertecies;
			------------------------code in Lab 3 -------------------------------------------
				3) calculate vertex 
 				3) add edges to edge table in the sequence of sorted Y value (put Ymax, X, xSlope, zSlope of each edge into Edge table)
 				4) run activeEdgeTable process to start scanline fill and determine the color of each pixel
			9. draw point using openGL based on the pixel Matrix
			
			
*/

#include "stdafx.h"
#include "ViewingTransform.h"
#include "MatrixTool.h"
#include "LoadFile.h"
#include "EdgeTable_T.h"
#include "ScanLineTexture.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <string>

using namespace std;

#define INITIAL_WIDTH 600
#define INITIAL_HEIGHT 600
#define INITIAL_X_POS 100
#define INITIAL_Y_POS 100

#define WINDOW_NAME  "CG Lab4 Fei Yan Texture Mapping "



LoadFile fileLoader; // object to load the data
ViewingTransform obj1; 
//ViewingTransform obj2;
//ViewingTransform obj3;

// set a 2D window in the perspective plane
float Xmin = 0;
float Xmax = INITIAL_WIDTH;
float Ymin = 0;
float Ymax = INITIAL_HEIGHT;

// initialize two pixel matrices for two objects
vector< vector < vector <float> > > pixels1(Ymax, vector< vector< float > >(Xmax, vector < float >(5)));
//vector< vector < vector <float> > > pixels2(Ymax, vector< vector< float > >(Xmax, vector < float >(5)));
//vector< vector < vector <float> > > pixels3(Ymax, vector< vector< float > >(Xmax, vector < float >(5)));

void init(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(Xmin, Xmax, Ymin, Ymax); //create the 2D window in the perspective plane
}

void scene(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// input Model file name
	//string datafile = "house.d";
	string datafile = "better-ball.d";
	//string datafile = "king.d";
	//string datafile = "cow.d";
	
	fileLoader.loadModel(datafile);

	// object to be transformed inheriting the loaded data
	obj1.m_verts = fileLoader.allVertex;
	obj1.m_polys = fileLoader.allPolygon;
	

	//obj2.m_verts = fileLoader.allVertex;
	//obj2.m_polys = fileLoader.allPolygon;

	//obj3.m_verts = fileLoader.allVertex;
	//obj3.m_polys = fileLoader.allPolygon;


	obj1.set2DWindow(Xmin, Xmax, Ymin, Ymax);
	//obj1.setObjectPosition(5, 0, -8); //object position for Sun and Earth
	obj1.setObjectPosition(0, 0, -2); //object position for single object
	//obj1.setObjectPosition(1, 0, -6); //object position for single object

	//obj2.set2DWindow(Xmin, Xmax, Ymin, Ymax);
	//obj2.setObjectPosition(0, 0, -2);//object position for Sun and Earth
	//obj2.setObjectPosition(0, 0, -2); //object position for single ball


	//obj3.set2DWindow(Xmin, Xmax, Ymin, Ymax);
	//obj3.setObjectPosition(-2.5, 0, -6);//object position for ball
	//obj3.setObjectPosition(5, 0, -11); //object position for house

	vector<vector<float>> temp1 = obj1.viewTransform();
	//vector<vector<float>> temp2 = obj2.viewTransform();
	//vector<vector<float>> temp3 = obj3.viewTransform();

	
	ScanLineTexture scan1(obj1.m_polys, obj1.visible_polygon, temp1, obj1.world_verts);
	//scan1.textureLoader("earth-texture.bmp");
	//scan1.textureLoader("basketball.bmp");
	//scan1.textureLoader("metaltexture.bmp");
	//scan1.textureLoader("water.bmp");
	scan1.textureLoader("strawberry-texture.bmp");

	//scan line 1
	scan1.setScreen(Xmin, Xmax, Ymin, Ymax);
	scan1.setPixelValue();
	pixels1 = scan1.pixels;

	/*
	//scan line 2
	ScanLineTexture scan2(obj2.m_polys, obj2.visible_polygon, temp2, obj2.world_verts);
	scan2.textureLoader("sun-texture.bmp");
	scan2.setScreen(Xmin, Xmax, Ymin, Ymax);
	scan2.setPixelValue();
	pixels2 = scan2.pixels;

	for (int i = 0; i<Ymax; i++){
		for (int j = 0; j<Xmax; j++){
			if (pixels1[i][j][0] == 0 && pixels2[i][j][0] == 1)
				swap(pixels1[i][j], pixels2[i][j]);
			else if (pixels1[i][j][0] == 1 && pixels2[i][j][0] == 1){
				if (pixels1[i][j][1] > pixels2[i][j][1]){
					//if pixel_1 more far
					swap(pixels1[i][j], pixels2[i][j]);
				}
			}
		}
	}
	
	//scan line 3
	ScanLineTexture scan3(obj3.visible_polygon, temp3);
	
	scan3.setScreen(Xmin, Xmax, Ymin, Ymax);
	scan3.setPixelValue();
	pixels3 = scan3.pixels;

	for (int i = 0; i<Ymax; i++){
		for (int j = 0; j<Xmax; j++){
			if (pixels1[i][j][0] == 0 && pixels3[i][j][0] == 1)
				swap(pixels1[i][j], pixels3[i][j]);
			else if (pixels1[i][j][0] == 1 && pixels3[i][j][0] == 1){
				if (pixels1[i][j][1] > pixels3[i][j][1]){
					//if pixel_1 is more far
					swap(pixels1[i][j], pixels3[i][j]);
				}
			}
		}
	}
	*/
//========================================================================================
//draw all the points with opengl function of draw point
//========================================================================================
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for (int i = 0; i < Xmax; i++)
	{
		for (int j = 0; j < Ymax; j++)
		{
			glBegin(GL_POINTS);
			if (pixels1[j][i][0] == 1)//if there is a pixel
			{   //set pixel color
				glColor3f(pixels1[j][i][2], pixels1[j][i][3],
					pixels1[j][i][4]);
				glVertex2i(i, j);
			}
			glEnd();
		}
	}
	glutSwapBuffers();
}

	int main(int argc, char** argv){
		
		//Initialize GLUT
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);		
		glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS); 
		glutCreateWindow(WINDOW_NAME); 	
		glutDisplayFunc(scene);

		//OpenGL and other program initialization
		init();
	
		glutMainLoop();

		return 1;
	}

	
/*
CS 6554 - Computer Graphics II - Spring 2015
Assignment 2 Due: 2/19
Student: Fei Yan
email: hcifaye@gwu.edu / fayefei@gmail.com

Scan-convert & Z-Buffer

Implementing ViewTransform Class: according to given viewing specification, transform a geometric data of a polygonal object to generate Perspective view of the object displayed on the viewport with back faces removed

*/

#include "stdafx.h"
#include "ViewingTransform.h"
#include "MatrixTool.h"
#include <GL/glut.h>
#include <vector>
#include <math.h>

using namespace std;

//========================================================================================
//Constructor
//========================================================================================
ViewingTransform::ViewingTransform()
{
	//set viewing specification
	setNear(10);
	setFar(200);
	setHeight(10);
	//setUpVec(0, 0, -1);//queen,king: up vector of camera
	setUpVec(0, 1, 0); //house,ball: up vector of camera
	//setCameraPosition(-5, 0, 10);//queen,king:set camera position on world coordinate
	setCameraPosition(0, 0, 0);//house,ball:set camera position on world coordinate
	//setProjectRef(1, 0, -1);//queen,king:set camera project reference on world coordinate
	setProjectRef(0, 0, -1);//house,ball:set camera project reference on world coordinate
}


//========================================================================================
//set viewing specification
//========================================================================================

//set vector for object shift from local to world
void ViewingTransform::setObjectPosition(float x, float y, float z)
{
	objectToWorld.push_back(x);
	objectToWorld.push_back(y);
	objectToWorld.push_back(z);
}

//set the Camera position in world coordinate
void ViewingTransform::setCameraPosition(float x, float y, float z)
{
	camera.push_back(x);
	camera.push_back(y);
	camera.push_back(z);
}

//set the up vector for camera
void ViewingTransform::setUpVec(float x, float y, float z)
{
	up_vector.push_back(x);
	up_vector.push_back(y);
	up_vector.push_back(z);
}

//set near clipping distance
void ViewingTransform::setNear(float nclip)
{
	nearClip = nclip;
}

//set far clipping distance
void ViewingTransform::setFar(float fclip)
{
	farClip = fclip;
}

//set h for perspective matrix
void ViewingTransform::setHeight(float height)
{
	h = height;
}

////set a 2D window in the perspective plane
void ViewingTransform::set2DWindow(float xmin, float xmax, float ymin, float ymax)
{
	Xmin = xmin;
	Ymin = ymin;
	Xmax = xmax;
	Ymax = ymax;
}

//set project reference for camera
void ViewingTransform::setProjectRef(float x, float y, float z)
{
	P_ref.push_back(x);
	P_ref.push_back(y);
	P_ref.push_back(z);
}



//========================================================================================
//calculate M_model, M_view and M_pers
//========================================================================================
//calculate M_model(modeling transformation matrix) with given model position in the world coordinate system.
void ViewingTransform::matrixModel(vector< vector<float> >& m_model, vector<float> modelPosition)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++){
		if (i == j)
			m_model[i][j] = 1;
		else if (i == 0 && j == 3)
			m_model[i][j] = modelPosition[0];
		else if (i == 1 && j == 3)
			m_model[i][j] = modelPosition[1];
		else if (i == 2 && j == 3)
			m_model[i][j] = modelPosition[2];
		else
			m_model[i][j] = 0;
		}
}


//Calculate camera'N vector for M_view : 
//N = P.ref - C / |P.ref - C| 
//to make it in the same right hand coordinate system as the object did
void ViewingTransform::view_N(vector<float> & N_vector, vector<float> camera_position, vector<float> p_ref)
{
	
	vector<float> temp_vector(3); //temporary array for store the result of (P.ref - C )	
	float temp_module; //temporary float for store the result of (|P.ref - C |)

	MatrixTool::vector_subtraction(temp_vector, camera_position, p_ref); // left hand system
	//MatrixTool::vector_subtraction(temp_vector, p_ref, camera_position); // right hand system
	temp_module = MatrixTool::vector_module(temp_vector); //
	for (int i = 0; i < 3; i++)
		N_vector[i] = temp_vector[i] / temp_module;
}


//Calculate camera'U vector for M_view : U = N x V' / |N x V'|
void ViewingTransform::view_U(vector<float> & U_vector, vector<float> n_vector, vector<float> up_vector)
{
	vector<float> temp_vector(3);  // temporary array for store the result of N x V'
	float temp_module;  //temporary float for store the result of |N x V'|
	MatrixTool::vector_cross_product(temp_vector, n_vector, up_vector); // N x V'
	temp_module = MatrixTool::vector_module(temp_vector); // |N x V'|

	for (int i = 0; i < 3; i++)
		U_vector[i] = temp_vector[i] / temp_module;
	
}

//Calculate camera'V vector for M_view : V = U x N
void ViewingTransform::view_V(vector<float> & V_vector, vector<float> u_vector, vector<float> n_vector)
{
	MatrixTool::vector_cross_product(V_vector, u_vector, n_vector);
}

//calculate M_view(view transformation matrix) with given camera position, up vector and perspective reference vector in the world coordinate system.
void ViewingTransform::matrixView(vector< vector<float> >& m_view, vector<float> camera_p, vector<float> up_vector )
{
	
	vector< vector<float> > m_viewR(4, vector<float>(4)); //rotate matrix	
	vector< vector<float> > m_viewT(4, vector<float>(4)); //translate matrix

	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++){
		if (i == j){
			m_viewR[i][j] = 1;
			m_viewT[i][j] = 1;
		}
		else{
			m_viewR[i][j] = 0;
			m_viewT[i][j] = 0;
		}
		}

	for (int i = 0; i<3; i++)
	{ 
		m_viewT[i][3] = -camera_p[i];
	}

	view_N(m_viewR[2], camera_p, P_ref); //m_viewR[2]: Nx Ny Nz
	view_U(m_viewR[0], m_viewR[2], up_vector);  //m_viewR[0]:Ux Uy Uz
	view_V(m_viewR[1], m_viewR[0], m_viewR[2]); //m_viewR[1]: Vx Vy Vz
	
	MatrixTool::matrix_4x4(m_view, m_viewR, m_viewT);
}


//calculate M_pers(projection transformation matrix) with given height, near clip and far clip
void ViewingTransform::matrixPers(vector< vector<float> >& m_pers, float d, float f, float h)
{
	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++){
		if (i == 0 && j == 0)
			m_pers[i][j] = d/h;
		else if (i == 1 && j == 1)
			m_pers[i][j] = d/h;
		else if (i == 2 && j == 2)
			m_pers[i][j] = f / (f - d);
		else if (i == 2 && j == 3)
			m_pers[i][j] = -f * d / (f - d);
		else if (i == 3 && j == 2)
			m_pers[i][j] = 1;
		else
			m_pers[i][j] = 0;
		}
}


//========================================================================================
//calculate vertex data after view tranform WITH back-face culling and normalization
//========================================================================================
vector< vector<float> > ViewingTransform::viewTransform(){
	//initialize modeling, viewing, perspective transformation matrix
	vector< vector<float> > m_model(4, vector<float>(4));
	vector< vector<float> > m_view(4, vector<float>(4));
	vector< vector<float> > m_pers(4, vector<float>(4));

	
	matrixModel(m_model, objectToWorld);//calculate Modeling transform Matrix
	matrixView(m_view, camera, up_vector);//calculate viewing transform Matrix	
	matrixPers(m_pers, nearClip, farClip, h);//calculate Perspective transform Matrix

	//calculate the x,y,z of vertecies in the world coordinate system
	vector< vector<float> > temp_world_verts(m_verts.size(), vector<float>(4));

	for (int i = 0; i < (int)m_verts.size(); i++)
	MatrixTool::matrix_vector_product(temp_world_verts[i], m_model, m_verts[i]);

	world_verts = temp_world_verts;

	//calculate the transformed  x,y,z of vertecies in the view space

	vector< vector<float> > m_model_view(4, vector<float>(4)); // store matrix for transforming object to view space
	MatrixTool::matrix_4x4(m_model_view, m_view, m_model);//calculate the matrix for transforming object to view space

	
	vector< vector<float> > temp_vertex(m_verts.size(), vector<float>(4)); //store the transformed vertex data in the view space

	for (int i = 0; i < (int)m_verts.size(); i++)
	//transform each vertex from local coordinate system to the view space
	MatrixTool::matrix_vector_product(temp_vertex[i], m_model_view, m_verts[i]);

	//initialize arrays in for calculating the visible polyons in the view space
	vector <float> view_normal(3); // store the vector of the direction point to camera
	vector <float> poly_edge_1(3); // one edge in the polygon
	vector <float> poly_edge_2(3); // one edge in the polygon
	vector <float> poly_normal(3); // store the vector for the polygon normal
	vector <float> visible_polygon_vertex_index;//store the index of visible vertex of one polygon

	//calculate the visible polyons in the view space
	for (int i = 0; i<(int)m_polys.size(); i++){
		//calculate two vectors on one polygon
		MatrixTool::vector_subtraction(poly_edge_1, temp_vertex[m_polys[i][2] - 1], temp_vertex[m_polys[i][1] - 1]);
		MatrixTool::vector_subtraction(poly_edge_2, temp_vertex[m_polys[i][2] - 1], temp_vertex[m_polys[i][3] - 1]);
		//calculate the direction to the camera
		MatrixTool::vector_subtraction(view_normal, camera, temp_vertex[m_polys[i][2] - 1]);
		//calculate the normal of the polygon
		MatrixTool::vector_cross_product(poly_normal, poly_edge_1, poly_edge_2);

		//polygon is visible if the dot product is greater than 0
		if (MatrixTool::vector_dot_product(poly_normal, view_normal) > 0){
			//store the index of visible polygon
			visible_polygon_index.push_back(i);
			//store the index of vertex in the visible polygon
			for (int j = 1; j< (int)m_polys[i][0] + 1; j++)
				visible_polygon_vertex_index.push_back(m_polys[i][j]);
		}
	}

	for (int i = 0; i< (int)visible_polygon_index.size(); i++)
		//store visible polygon's vertex data
		visible_polygon.push_back(m_polys[visible_polygon_index[i]]);

	//mark visible vertex by add -1 at the back of them
	for (int i = 0; i < (int)visible_polygon_vertex_index.size(); i++){
		if (temp_vertex[visible_polygon_vertex_index[i] - 1].size() == 4){
			vector<float>temp(4);
			//calculate  vertex in perspective space from view space vertices
			MatrixTool::matrix_vector_product(temp, m_pers, temp_vertex[visible_polygon_vertex_index[i] - 1]);
			for (int j = 0; j < 4; j++){
				//nomalized vertex through divide by W
				temp[j] /= temp[3];
			}
			temp[0] = ((Xmax - Xmin) / 2)*(temp[0] + 1);
			temp[1] = ((Ymax - Ymin) / 2)*(temp[1] + 1);
			temp_vertex[visible_polygon_vertex_index[i] - 1] = temp;
			temp_vertex[visible_polygon_vertex_index[i] - 1].push_back(-1);
		}
	}
	return temp_vertex;
}

//========================================================================================
//draw all the visible vertex with opengl function of draw line
//========================================================================================

//draw lines between visible vertex
	void ViewingTransform::drawModel(){

		vector<vector<float>> temp = viewTransform(); // get vertex data in the view space
		vector< vector<int> >  visible_poly = visible_polygon; // get the visible polygon data

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		for (int i = 0; i < (int)visible_poly.size(); i++)
		{
			//draw line from one vertex to the next vertex
			for (int j = 1; j < visible_poly[i][0]; j++)
			{
				glBegin(GL_LINES);
				glVertex2f(temp[visible_poly[i][j] - 1][0], temp[visible_poly[i][j] - 1][1]);
				glVertex2f(temp[visible_poly[i][j + 1] - 1][0], temp[visible_poly[i][j + 1] - 1][1]);
				glEnd();
			}
			//connect last vertex and first vertex
			glBegin(GL_LINES);
			glVertex2f(temp[visible_poly[i][visible_poly[i][0]] - 1][0], temp[visible_poly[i][visible_poly[i][0]] - 1][1]);
			glVertex2f(temp[visible_poly[i][1] - 1][0], temp[visible_poly[i][1] - 1][1]);
			glEnd();
		}
		glutSwapBuffers();
	
	}

/*
//========================================================================================
//calculate vertex data after view tranform WITHOUT back-face culling and normalization
//========================================================================================
vector< vector<float> > ViewingTransform::viewTransform(){
	//initialize modeling, viewing, perspective transformation matrix
	vector< vector<float> > m_model(4, vector<float>(4));
	vector< vector<float> > m_view(4, vector<float>(4));
	vector< vector<float> > m_pers(4, vector<float>(4));


	matrixModel(m_model, objectToWorld);//calculate Modeling transform Matrix
	matrixView(m_view, camera, up_vector);//calculate viewing transform Matrix	
	matrixPers(m_pers, nearClip, farClip, h);//calculate Perspective transform Matrix

	vector< vector<float> > m_model_view(4, vector<float>(4)); // store matrix for transforming object to view space
	vector< vector<float> > m_final_view(4, vector<float>(4)); // store matrix for final object transformation
	MatrixTool::matrix_4x4(m_model_view, m_view, m_model);//calculate the matrix for transforming object to view space
	MatrixTool::matrix_4x4(m_final_view, m_pers, m_model_view);//calculate the matrix for final object transformation


	vector< vector<float> > temp_vertex(m_verts.size(), vector<float>(4)); //store the transformed vertex data

	for (int i = 0; i < (int)m_verts.size(); i++){

		MatrixTool::matrix_vector_product(temp_vertex[i], m_final_view, m_verts[i]);
		vector<float>temp(4);
		temp = temp_vertex[i];
		for (int j = 0; j < 4; j++){
			//nomalized vertex through divide by W
			temp[j] /= temp[3];
		}
		temp[0] = ((Xmax - Xmin) / 2)*(temp[0] + 1);
		temp[1] = ((Ymax - Ymin) / 2)*(temp[1] + 1);
		temp_vertex[i] = temp;
		temp_vertex[i].push_back(-1);
	}
	return temp_vertex;
}

//========================================================================================
//draw all the ploygon vertex with opengl function of draw line
//========================================================================================

//draw lines between visible vertex
void ViewingTransform::drawModel(){

	vector<vector<float>> temp = viewTransform(); // get vertex data in the view space
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for (int i = 0; i < (int)m_polys.size(); i++)
	{
		//draw line from one vertex to the next vertex
		for (int j = 1; j < m_polys[i][0]; j++)
		{
			glBegin(GL_LINES);
			glVertex2f(temp[m_polys[i][j] - 1][0], temp[m_polys[i][j] - 1][1]);
			glVertex2f(temp[m_polys[i][j + 1] - 1][0], temp[m_polys[i][j + 1] - 1][1]);
			glEnd();
		}
		//connect last vertex and first vertex
		glBegin(GL_LINES);
		glVertex2f(temp[m_polys[i][m_polys[i][0]] - 1][0], temp[m_polys[i][m_polys[i][0]] - 1][1]);
		glVertex2f(temp[m_polys[i][1] - 1][0], temp[m_polys[i][1] - 1][1]);
		glEnd();
	}
	glutSwapBuffers();

}
*/
ViewingTransform::~ViewingTransform(){}
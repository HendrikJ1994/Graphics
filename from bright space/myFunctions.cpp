/** This file contains five functions that get
 called when the user presses 1-5 and a draw( Mesh m )
 function that gets called everytime the mesh needs to
 be redrawn. 

 All functions get passed a Mesh object, which contains
 the list of vertices and triangles amongst other things.

 In particular, m.vertices is a vector that contains
 a Vertex object for each vertex of the mesh. The vertex
 object simply holds the 3d world coordinates.
 The order of a vertex in this vector determines its
 index.
 m.triangles is a vector that contains one Triangle
 object for each triangle of the mesh, which then
 simply holds three integers, pointing to the indices
 of the vertices that this triangle is made of.

 The draw(Mesh m) function below gets called
 everytime the screen has to be redrawn (e.g.
 when the mouse has been used). For now it calls
 a very simple draw function implemented in the
 Mesh class, but you can change it in order to
 visualize the results of the exercises.
 A redraw can be force by calling
 glutPostRedisplay(), which has to be done
 once your visualization changes.

 These two lists and the draw() function
 should be all you need to solve the
 exercises.

 Feel free to implement your own auxilary functions,
 you are not restricted to the functions shown below.*/

#include "Vec3D.h"
#include "mesh.h"
#include <GL/glut.h>
#include <vector>
using namespace std;

int firstTimeDrawing = 0;
vector<Vec3Df> colors;

vector<vector<int> > neighborsVertices;

void drawCenters(Mesh& m);
void drawMiddleLine(Mesh& m);
void colorTriangles(Mesh& m);
void defaultcolor(Mesh& m);
void getVerticesNeighbors(Mesh& m);
void drawVertexNormals(Mesh& m);


int draw_triangle_centers = 0;
int draw_middle_lines = 0;
int color_triangles = 0;
int draw_vertex_normals = 0;

/** Function that gets called on keypress 1 */
void myFunction1(Mesh& m) {
	draw_triangle_centers = 1 - draw_triangle_centers;
	glutPostRedisplay();
	
}

/** Function that gets called on keypress 2 */
void myFunction2(Mesh& m) {
	draw_middle_lines = 1 - draw_middle_lines;
	glutPostRedisplay();

}

/** Function that gets called on keypress 3 */
void myFunction3(Mesh& m) {
	color_triangles = 1 - color_triangles;
	glutPostRedisplay();

}

/** Function that gets called on keypress 4 */
void myFunction4(Mesh& m) {
	draw_vertex_normals = 1 - draw_vertex_normals;
	glutPostRedisplay();

}

/** Function that gets called on keypress 5 */
void myFunction5(Mesh& m) {

}

/** Gets called once the mesh has to be drawn.
 Currently calls the meshs draw function, which just
 draws the faces in plain white.
 With your OpenGL knowledge you could write similar
 draw functions that highlight certain vertices,
 edges or faces for better visualization of the
 results of your functions above. */
void draw(Mesh& m) {
	if (firstTimeDrawing == 0) {
		firstTimeDrawing = 1;
		getVerticesNeighbors(m);
	}

	if (color_triangles == 0) {
		defaultcolor(m);
	}
	if (color_triangles == 1) {
		colorTriangles(m);
	}
	
	m.drawWithColors(colors);

	if (draw_triangle_centers == 1) {
		drawCenters(m);
	}
	if (draw_middle_lines == 1) {
		drawMiddleLine(m);
	}
	if (color_triangles == 1) {
		colorTriangles(m);
	}
	if (draw_vertex_normals == 1) {
		drawVertexNormals(m);
	}

}

void drawCenters(Mesh& m) {
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//???

	glPointSize(3.0);
	glBegin(GL_POINTS);
	glColor3f(0.0, 1.0, 1.0);//turquise

	for (unsigned int i = 0; i < m.triangles.size(); ++i) {
		int i0 = m.triangles[i].v[0];
		int i1 = m.triangles[i].v[1];
		int i2 = m.triangles[i].v[2];

		Vertex v0 = m.vertices[i0];
		Vertex v1 = m.vertices[i1];
		Vertex v2 = m.vertices[i2];

		Vec3Df p0 = v0.p;
		Vec3Df p1 = v1.p;
		Vec3Df p2 = v2.p; //p2[0] for acces the first element of p2

		Vec3Df middlePoint = (p0 + p1 + p2) / 3.0;
		glVertex3f(middlePoint[0], middlePoint[1], middlePoint[2]);
	}
	glEnd();

	glPopAttrib();
}

void drawMiddleLine(Mesh& m) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 1.0);//turquise

	for (unsigned int i = 0; i < m.triangles.size(); ++i) {
		int i0 = m.triangles[i].v[0];
		int i1 = m.triangles[i].v[1];
		int i2 = m.triangles[i].v[2];

		Vertex v0 = m.vertices[i0];
		Vertex v1 = m.vertices[i1];
		Vertex v2 = m.vertices[i2];

		Vec3Df p0 = v0.p;
		Vec3Df p1 = v1.p;
		Vec3Df p2 = v2.p; //p2[0] for acces the first element of p2

		Vec3Df middlePoint = (p0 + p1 + p2) / 3.0;
		glVertex3f(middlePoint[0], middlePoint[1], middlePoint[2]);

		Vec3Df normal = Vec3Df::crossProduct(p0 - p1, p0 - p2);
		normal.normalize();
		Vec3Df endPoint = middlePoint + normal * 0.03f;

		glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
	}
	glEnd();

	glPopAttrib();
}

void colorTriangles(Mesh& m) { //AF
	float red = 1.0, blue=0.0, green=0.0;
	colors.resize(0);
	Vec3Df new_color;// = Vec3Df(0.0f, 0.0f, 0.0f);
	
		
	for (unsigned int i = 0; i < m.vertices.size(); ++i){
		if (i < m.vertices.size() / 5) {
			red = 1.0;
			green = green + (5.0f / (float)m.vertices.size());
		}
		else if ((m.vertices.size() / 5) < i && i < 2 * (m.vertices.size() / 5)) {
			red = red - (5.0f /(float) m.vertices.size());
			green = 1.0;
		}
		else if ((2 * (m.vertices.size() / 5)) < i && i < 3 * (m.vertices.size() / 5)) {
			red = 0.0;
			green = 1.0;
			blue = blue + (5.0f / (float)m.vertices.size());
		}
		else if ((3 * (m.vertices.size() / 5)) < i && i < 4 * (m.vertices.size() / 5)) {
			red = 0.0;
			green = green - (5.0f / (float)m.vertices.size());
			blue = 1.0;	
		}
		else if ((4 * (m.vertices.size()) < i && i < m.vertices.size())) {
			red = red + (5.0f /(float) m.vertices.size());
			green = 0.0;
			blue = 1.0;
		}
		new_color = Vec3Df(red, green, blue);
		colors.push_back(new_color);
	}
	//cout << red << endl;
	

}

void drawVertexNormals(Mesh& m) {
	int p0 = 0, p1 = 0, p2 = 0;
	int indexTriangles= 0;
	Vec3Df normalVertex;

	normalVertex.init(0.0, 0.0, 0.0);

	//vector<int> triangles;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glBegin(GL_LINES);
	glColor3f(0.0, 1.0, 0.0);//turquise



	for (unsigned int i = 0; i < m.vertices.size(); ++i) {
		normalVertex.init(0.0, 0.0, 0.0);
		for (unsigned int j = 0; j < neighborsVertices[i].size(); ++j) {
			//indexTriangles = neighborsVertices[i][j];
			//cout << "test" << endl;
			int i0 = m.triangles[neighborsVertices[i][j]].v[0];
			int i1 = m.triangles[neighborsVertices[i][j]].v[1];
			int i2 = m.triangles[neighborsVertices[i][j]].v[2];

			Vertex v0 = m.vertices[i0];
			Vertex v1 = m.vertices[i1];
			Vertex v2 = m.vertices[i2];

			Vec3Df p0 = v0.p;
			Vec3Df p1 = v1.p;
			Vec3Df p2 = v2.p; //p2[0] for acces the first element of p2

			//Vec3Df normalTriangle = Vec3Df::crossProduct(p0 - p1, p0 - p2);
			Vec3Df normal = Vec3Df::crossProduct(p0 - p1, p0 - p2);
			normal.normalize();
			//
			normalVertex = normalVertex + normal;
			
		}
		normalVertex = normalVertex / (float)neighborsVertices[i].size();
		normalVertex.normalize();

		glVertex3f(m.vertices[i].p[0], m.vertices[i].p[1], m.vertices[i].p[2]);
		Vec3Df endPoint = m.vertices[i].p + normalVertex*0.03f;

		glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
	}
	glEnd();
	glPopAttrib();
}

void getVerticesNeighbors(Mesh& m) {
	int connectedPoints = 0;
	int neighborsFound = 0;
	neighborsVertices.resize(m.vertices.size());

	for (unsigned i = 0; i < m.triangles.size(); ++i) {
		for (unsigned j = 0; j < 3; ++j) {
			neighborsVertices[m.triangles[i].v[j]].push_back(i);
		}
	}
}
 
void defaultcolor(Mesh& m) {
	colors.resize(0);
	Vec3Df new_color = Vec3Df(1.0f, 1.0f, 1.0f);
		for (unsigned int i = 0; i < m.vertices.size(); ++i) {
			colors.push_back(new_color);
		}
}


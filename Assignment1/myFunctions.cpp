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

vector<vector<int> > triangleNeighborsOfVertices;
vector<vector<int> > vertexNeighborsOfVertices;
vector<vector<int>> Components;


vector<int> simplifyComponents(vector<int>& component);
void drawCenters(Mesh& m);
void drawMiddleLine(Mesh& m);
void colorTriangles(Mesh& m);
void defaultcolor(Mesh& m);
void getTriangleNeighboursOfVertices(Mesh& m);
void drawVertexNormals(Mesh& m);
void getVertexNeighboursOfVertices(Mesh& m);
void getConnectedComponents(Mesh& m);
void addToComponent(vector<int>& new_component, vector<int>& alreadyDone, int vertex);
void colorComponents(Mesh& m);
int checkForDoubles(int needcheck, int v);
int listCheck(vector<int> checklist, int toBeChecked);


int draw_triangle_centers = 0;
int draw_middle_lines = 0;
int color_triangles = 0;
int draw_vertex_normals = 0;
int color_components = 0;

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
	color_components = 1 - color_components;
	glutPostRedisplay();
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
		getTriangleNeighboursOfVertices(m);
		getVertexNeighboursOfVertices(m);
		getConnectedComponents(m);
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
	if (color_components == 1) {
		colorComponents(m);
	}

}

void drawCenters(Mesh& m) {
	
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//???

	glPointSize(1.0);
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
		for (unsigned int j = 0; j < triangleNeighborsOfVertices[i].size(); ++j) {
			//indexTriangles = neighborsVertices[i][j];
			//cout << "test" << endl;
			int i0 = m.triangles[triangleNeighborsOfVertices[i][j]].v[0];
			int i1 = m.triangles[triangleNeighborsOfVertices[i][j]].v[1];
			int i2 = m.triangles[triangleNeighborsOfVertices[i][j]].v[2];

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
		normalVertex = normalVertex / (float)triangleNeighborsOfVertices[i].size();
		normalVertex.normalize();

		glVertex3f(m.vertices[i].p[0], m.vertices[i].p[1], m.vertices[i].p[2]);
		Vec3Df endPoint = m.vertices[i].p + normalVertex*0.03f;

		glVertex3f(endPoint[0], endPoint[1], endPoint[2]);
	}
	glEnd();
	glPopAttrib();
}

void getTriangleNeighboursOfVertices(Mesh& m) {
	int connectedPoints = 0;
	int neighborsFound = 0;
	triangleNeighborsOfVertices.resize(m.vertices.size());

	for (unsigned int i = 0; i < m.triangles.size(); ++i) {
		for (unsigned int j = 0; j < 3; ++j) {
			triangleNeighborsOfVertices[m.triangles[i].v[j]].push_back(i);
		}
	}
}

void getVertexNeighboursOfVertices(Mesh& m) {
	vertexNeighborsOfVertices.resize(m.vertices.size());
	for (unsigned int i = 0; i < m.vertices.size(); ++i) {
		for (unsigned int j = 0; j < triangleNeighborsOfVertices[i].size(); ++j) {
			
			int v0 = m.triangles[triangleNeighborsOfVertices[i][j]].v[0];
			int v1 = m.triangles[triangleNeighborsOfVertices[i][j]].v[1];
			int v2 = m.triangles[triangleNeighborsOfVertices[i][j]].v[2];

			int checkv0 = checkForDoubles(v0, i);
			int checkv1 = checkForDoubles(v1, i);
			int checkv2 = checkForDoubles(v2, i);
			//cout << "v1 = " << v1 << "v2 = " << v2 << "v0 = " << v0 << endl;
			//cout << checkv0 << checkv1 << checkv2 << endl;
			// 1 van v0,1,2 == j, dus andere 2 toevoegen aan vector, maar dan krijg je dubbele dingen dus checken of degene die toegevoegd word er al in zit.
			if (i == v0) {
				if (checkv1 == 0) {
					vertexNeighborsOfVertices[i].push_back(v1);
					
				}
				if (checkv2 == 0) {
					vertexNeighborsOfVertices[i].push_back(v2);
				}
			}
			else if (i == v1) {
				if (checkv0 == 0) {
					vertexNeighborsOfVertices[i].push_back(v0);
				}
				if (checkv2 == 0) {
					vertexNeighborsOfVertices[i].push_back(v2);
				}
			}
			else if (i == v2) {
				if (checkv0 == 0) {
					vertexNeighborsOfVertices[i].push_back(v0);
				}
				if (checkv1 == 0) {
					vertexNeighborsOfVertices[i].push_back(v1);
				}
			}
		}
	}
}

int checkForDoubles(int needcheck, int v) {
	int check = 0;
	if (vertexNeighborsOfVertices[v].size() == 0) {
		check = 0;
	}
	else {
		for (unsigned int loopvar = 0; loopvar < vertexNeighborsOfVertices[v].size(); ++loopvar) {

			if (needcheck == vertexNeighborsOfVertices[v][loopvar]) {
				check = 1;
				break;
			}
			else check = 0;
		}
	}

	return check;
}
 
void defaultcolor(Mesh& m) {
	colors.resize(0);
	Vec3Df new_color = Vec3Df(1.0f, 1.0f, 1.0f);
		for (unsigned int i = 0; i < m.vertices.size(); ++i) {
			colors.push_back(new_color);
		}
}

void getConnectedComponents(Mesh& m) {
	//vector<vector<int>> connected;
	//vector<int> Components;
	vector<int> newComponent;
	vector<int> alreadyDone;
	//vector<int> StillToDo;
	int currentVertex = 0;

	alreadyDone.resize(m.vertices.size());
	newComponent.resize(m.vertices.size());
	fill(alreadyDone.begin(), alreadyDone.end(), 0);

	for (unsigned int i = 0; i < m.vertices.size(); ++i) {
		if (alreadyDone[i] == 0) {
			fill(newComponent.begin(), newComponent.end(), 0);
			addToComponent(newComponent, alreadyDone, i);
			Components.push_back(newComponent);
		}
	}

	for (unsigned int i = 0; i < Components.size(); ++i) {
		Components[i] = simplifyComponents(Components[i]);
	}
}

vector<int> simplifyComponents(vector<int>& component) {
	vector<int> newComponent;
	for (unsigned int i = 0; i < component.size(); ++i) {
		if (component[i] == 0) {
			newComponent.push_back(i);
		}
	}
	return newComponent;
}

void addToComponent(vector<int>& Component, vector<int>& alreadyDone, int vertex) {
	Component[vertex] = 1;
	alreadyDone[vertex] = 1;
	for (unsigned int i = 0; i < vertexNeighborsOfVertices[vertex].size(); ++i) {
		if (Component[vertexNeighborsOfVertices[vertex][i]] == 0) {
			addToComponent(Component, alreadyDone, vertexNeighborsOfVertices[vertex][i]);
		}
	}
}






	
	
	



	//while (StillToDo.size() != 0) {
	//	currentVertex = StillToDo[0];
	//	for (unsigned int i = 0; i < triangleNeighborsOfVertices[currentVertex].size(); ++i) {
	//		// checken of the triangle er al in connectedTriangles zit
	//		//needs to be checked for multiple times the same triangle
	//		if (listCheck(connectedTriangles, triangleNeighborsOfVertices[currentVertex][i]) == 0) {
	//			connectedTriangles.push_back(triangleNeighborsOfVertices[currentVertex][i]);
	//		}
	//	}
	//	//needs to be checked if the vertex alrealy has been done
	//	for (unsigned int j = 0; j < vertexNeighborsOfVertices[currentVertex].size(); ++j) {
	//		if (listCheck(alreadyDone, vertexNeighborsOfVertices[currentVertex][j]) == 0) {
	//			StillToDo.push_back(vertexNeighborsOfVertices[currentVertex][j]);
	//		}
	//		else continue;
	//	}
	//	alreadyDone.push_back(StillToDo[0]);
	//	StillToDo.erase(StillToDo.begin());
	//}
	//cout << "quit while loop" << endl;

	
	/*for (unsigned int i = 0; i < triangleNeighborsOfVertices.size(); ++i) {
		for (unsigned int j = 0; j < triangleNeighborsOfVertices[i].size(); ++j) {
			alreadyDone.push_back(triangleNeighborsOfVertices[i][j]);
			
			connected[i].push_back(triangleNeighborsOfVertices[i][j]);

		}
	}*/

	
	//vector<vector<int>> copy_vertexNeighborsOfVertices = vertexNeighborsOfVertices;
	//vector<vector<int>> copy_triangleNeighborsOfVertices = triangleNeighborsOfVertices;
	////vector<int> StillToDo;
	//vector<int> justDone;

	////StillToDo.resize(0);
	//

	//int currentVertex = 0;
	////StillToDo.push_back(currentVertex);
	////justDone.push_back(4);
	///*for (int j = justDone.size()-1; j >= 0; --j) {
	//	cout << justDone[j]<< endl;
	//}*/
	/////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////
	//while (copy_vertexNeighborsOfVertices.size() != 0) {
	//	justDone.resize(0);
	//	for (unsigned int h = 0; h < copy_vertexNeighborsOfVertices[currentVertex].size(); ++h) {
	//		for (unsigned int i = 0; i < copy_vertexNeighborsOfVertices[currentVertex].size(); ++i) {
	//			connectedTriangles.push_back(copy_vertexNeighborsOfVertices[currentVertex][i]);
	//			justDone.push_back(copy_vertexNeighborsOfVertices[currentVertex][i]);
	//		}

	//		for (unsigned int i = 0; i < copy_vertexNeighborsOfVertices[currentVertex].size(); ++i) { //loop through all the neighbors vertices of the last added vertex
	//			int vertexneighborIndex = copy_vertexNeighborsOfVertices[currentVertex][i];
	//			copy_triangleNeighborsOfVertices[vertexneighborIndex].begin();
	//			for (unsigned int j = 0; j < justDone.size(); ++j) {// loop through all te triangles connected to the neighbor vertex
	//				if (listCheck(copy_triangleNeighborsOfVertices[vertexneighborIndex], justDone[j]) == 1) {
	//					copy_triangleNeighborsOfVertices[vertexneighborIndex].erase(copy_triangleNeighborsOfVertices[vertexneighborIndex].begin() + j);
	//				}
	//				//				if(copy_triangleNeighborsOfVertices[copy_vertexNeighborsOfVertices[i][j]]==
	//			}
	//		}
	//		//currentVertex = copy_vertexNeighborsOfVertices[currentVertex][h];
	//	}
	//}



	


int listCheck(vector<int> checklist, int toBeChecked) {
	int check = 0;
	for (unsigned int i = 0; i < checklist.size(); i++) {
		if (checklist[i] == toBeChecked) {
			check = 1;
		}
		else continue;
	}
	return check;
}

void colorComponents(Mesh& m) {
	int nrOfComponent = 0;
	fill(colors.begin(), colors.end(), 0);
	for (unsigned int i = 0; i <Components.size(); ++i) {
		if (unsigned int j = 0; j < Components[i].size(); ++j) {
			if ((nrOfComponent+1)%6 == 0) {
				colors[Components[i][j]] = (1, 0, 0);
			}
			else if ((nrOfComponent + 1) % 6 == 1) {
				colors[Components[i][j]] = (1, 0, 0);
			}
			else if ((nrOfComponent + 1) % 6 == 2) {
				colors[Components[i][j]] = (1, 0, 0);
			}
			else if ((nrOfComponent + 1) % 6 == 3) {
				colors[Components[i][j]] = (1, 0, 0);
			}
			else if ((nrOfComponent + 1) % 6 == 4) {
				colors[Components[i][j]] = (1, 0, 0);
			}
			else {
				colors[Components[i][j]] = (1, 0, 0);
			}
		}
	}
}


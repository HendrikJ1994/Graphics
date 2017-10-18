#include <vector>
#include "mesh.h"
#include <math.h>

//THIS IS THE ONLY FILE YOU NEED TO MODIFY and SUBMIT!!! NOTHING ELSE!!!
//Please RENAME as described below and then send it to assignments.eisemann@gmail.com 
//(I will not use this email for any other purposes than collecting your files)
//You should send per student only ONE file (this one).
//Each file should be renamed using your studentids.
//E.g., Imagine two people worked together and their ids are 12 and 14, 
//then you should send two files called "12.h" and "14.h"
//Any deviation from this naming convention will lead to 0 points because the exercise is verified automatically!!!
//Good Luck!


//Global array to store mesh material properties and algorithmic parameters
std::vector<Vec3Df> Kd;//diffuse coefficient per vertex
std::vector<Vec3Df> Ks;//specularity coefficient per vertex
std::vector<float> Shininess;//exponent for phong and blinn-phong specularities
int ToonDiscretize=4;//number of levels in toon shading
float ToonSpecularThreshold=0.49;//threshold for specularity

//Mesh - will be filled and loaded outside.
Mesh MyMesh;


//Helper function that you can ignore!
void initStudentVariables()
{
	//this is needed so that your table has the same size as the number of vertices.
	//later, the vertex indices received in the functions correspond to the same location in your vector.
	//in other words, you can store some information per vertex here.
	Kd.resize(MyMesh.vertices.size(), Vec3Df(0.5,0.5,0.5));
	Ks.resize(MyMesh.vertices.size(), Vec3Df(0.5,0.5,0.5));
	Shininess.resize(MyMesh.vertices.size(), 3);
}


//for debugging purposes or variable changes (e.g., modify the toon threshold as done below)
//please notice that some keys are already in use!
void yourKeyboardFunction(unsigned char key)
{
	cout<<"Key not used so far, so executing your code!"<<endl;
	
	//recolor the mesh 
	switch(key)
	{
		case 't': 
			ToonSpecularThreshold-=0.001;
		break;
		case 'T': 
			ToonSpecularThreshold+=0.001;
		break;
		case 'd': 
			ToonDiscretize-=1;
			if (ToonDiscretize<=0)
				ToonDiscretize=1;
		break;
		case 'D': 
			ToonDiscretize+=1;
		break;
		
		//in case you want to add colors! - Not mandatory

		case 'r': //decrase diffuse Kd coefficient in the red channel by 0.01
		break;
		case 'R': //increase diffuse Kd coefficient in the red channel by 0.01
		break;
		case 'g'://same for green
		break;
		case 'G':
		break;
		case 'b'://same for blue
		break;
		case 'B':
		break;
	}
	
	cout<<"ToonSpecular"<<ToonSpecularThreshold<<endl;
	cout<<"Toon Discretization levels"<<ToonDiscretize<<endl;

}


//Debug function
Vec3Df debugColor(unsigned int index)
{	//this function you can use in any way you like!
	//e.g., for debugging purposes!
	return MyMesh.vertices[index].n;
	//or 
	//return Kd[index];
}


///////////////
///Shading
///////////////
//standard lambertian shading: Kd * dot(N,L), clamped to zero when negative. Where L is the light vector
//
Vec3Df diffuseOnly(const Vec3Df & vertexPos, Vec3Df & normal, const Vec3Df & lightPos, unsigned int index)
{	
	///////____lambertian shading_____//////////////
	Vec3Df lightDirection = lightPos - vertexPos;
	Vec3Df normalEndpoint = normal ;
	lightDirection.normalize();

	float dotProduct = Vec3Df:: dotProduct(normal, lightDirection);

	if (dotProduct < 0) {
		dotProduct = 0;
	}
	float red = Kd[index][0] * dotProduct;
	float green = Kd[index][1] * dotProduct;
	float blue = Kd[index][2] * dotProduct;

	//std::cout << vector << endl;

	return Vec3Df (red, green, blue);
}


//Phong (!) Shading Specularity (http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model)
//Follow the course, only calculate Ks pow(dot(V,R),shininess), where V is the view vector and R 
//is the Reflection vector of the light (like in pool billard computed from the LightPos, vertexPos and normal).
//When computing specularities like this, verify that the light is on the right side of the surface, with respect to the normal
//E.g., for a plane, the light source below the plane cannot cast light on the top, hence, there can also not be any specularity. 
Vec3Df phongSpecularOnly(const Vec3Df & vertexPos, Vec3Df & normal, const Vec3Df & lightPos, const Vec3Df & cameraPos, unsigned int index)
{
	// R is undefined Reflection vector
	int shininess = 40;
	float red = 0, green = 0, blue = 0;
	Vec3Df b = cameraPos - vertexPos;	//camera pos vector
	Vec3Df c = lightPos - vertexPos;	//light vector

	float angleCameraNormal = Vec3Df::dotProduct(normal, b);
	float angleLightNormal = Vec3Df::dotProduct(normal, c);
	float dotProduct = 0;

	if (angleCameraNormal <= 0 || angleLightNormal <= 0) {
		dotProduct = 0;
	}
	else {
		Vec3Df L = lightPos - vertexPos;
		Vec3Df V = cameraPos - vertexPos; //view vector
		Vec3Df R = 2 * (L*normal)*normal - L;

		R.normalize();
		V.normalize();

		dotProduct = pow(Vec3Df::dotProduct(R, V), shininess);

	}
	

	if (dotProduct < 0) {
		dotProduct = 0;
	}


	red = Ks[index][0] * dotProduct;
	green = Ks[index][1] * dotProduct;
	blue = Ks[index][2] * dotProduct;


	return Vec3Df(red, green, blue);
}

//Blinn-Phong Shading Specularity (http://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model)
//Be careful!!! The pseudo code does some additional modifications to the formula seen in the course
//Follow the course version and calculate ONLY Ks * pow(dot(N,H), shininess). The definition of H is given on the page above and in the course.
//The same test as before should be used
Vec3Df blinnPhongSpecularOnly(const Vec3Df & vertexPos, Vec3Df & normal, const Vec3Df & lightPos, const Vec3Df & cameraPos, unsigned int index)
{
	int shininess = 10;
	Vec3Df L = lightPos - vertexPos;
	Vec3Df V = cameraPos - vertexPos; //view vector

	Vec3Df normalEndVector = normal + vertexPos; //normal end point
	//Vec3Df b = cameraPos - vertexPos;	//camera pos vector
	//Vec3Df c = lightPos - vertexPos;	//light vector

	float angleCameraNormal = Vec3Df::dotProduct(normal, V); //if the angle between the camera and the normal is > 90 than the camera is beneath the surface so it should be dark
	float angleLightNormal = Vec3Df::dotProduct(normal, L); // if the light engle is beneath the surface it sould be dark
	float dotProduct = 0;

	if (angleCameraNormal <= 0 || angleLightNormal <= 0) {
		dotProduct = 0;
	}
	else {
		//L.normalize();
		//V.normalize();

		Vec3Df H = L + V;
		H.normalize();
		dotProduct = pow(Vec3Df::dotProduct(normal, H), shininess);
	}

	

	if (dotProduct < 0) {
		dotProduct = 0;
	}

	float red = Ks[index][0] * dotProduct;
	float green = Ks[index][1] * dotProduct;
	float blue = Ks[index][2] * dotProduct;


	return Vec3Df(red, green, blue);
}


///////////////
//Toon Shading
///////////////
//use the variable ToonDiscretize.
//Normal diffuse shading has values between 0 and Kd (the diffuse coefficient).
//In toon shading, these values are discretized.
//This means, there are N (ToonDiscretize) uniform intervals between 0 and Kd - in this example, we assume a single color channel,
//you should use the values from the vector Kd 
//Let c(0)=0, c(1) ...c(N), c(N+1)=Kd be the boundary values of these intervals.
//For a value v in [c(i), c(i+1)), the function should return (c(i)+c(i+1))/2.
//For v=Kd, return (c(N)+c(N+1))/2, else 0.
Vec3Df toonShadingNoSpecular(const Vec3Df & vertexPos, Vec3Df & normal, const Vec3Df & lightPos, unsigned int index)
{
	//float red, green, blue;
	Vec3Df lightDiffusion = diffuseOnly(vertexPos, normal, lightPos, index);
	std::vector<float> c;
	int N = ToonDiscretize;
	Vec3Df color;
	c.resize(ToonDiscretize + 2);
	c[0] = 0;

	for (int i = 0; i <= 2; ++i) {
		for (int j = 1; j < N + 2; ++j) {
			c[j] = (j*Kd[index][i]) / (N + 1);			
		}
		//std::cout << i<< endl;
		color[i] = 0;
		for (int j = 0; j < N + 1; ++j) {
			
			if (Kd[index][i] == lightDiffusion[i]) {
				color[i] = (c[N] + c[N + 1]) / 2;
			}
			else if (c[j] < lightDiffusion[i] && lightDiffusion[i]< c[j + 1]) {
				color[i] = (c[j] + c[j + 1]) / 2;
			}
			else continue;
		}
	}
	return color;
}

//Toon shading specularity
//The goal is to add white highlights.
//If a channel of Blinn-Phong Specularity has a value bigger or equal to ToonSpecularThreshold, then set it to 1, else to 0.
Vec3Df toonShadingOnlySpecular(const Vec3Df & vertexPos, Vec3Df & normal, const Vec3Df & lightPos, const Vec3Df & cameraPos, unsigned int index)
{

	Vec3Df color = blinnPhongSpecularOnly(vertexPos, normal, lightPos, cameraPos, index);
	for (int i = 0; i <= 2; ++i) {
		if (color[i] <ToonSpecularThreshold) {
			color[i] = 0;
		}
		else {
			color[i] = 1.0;
		}
	}
	return color;
}


///////////////
///INTERACTION
///////////////
Vec3Df userInteractionSphere(const Vec3Df & selectedPos, const Vec3Df & camPos)
{
	//RETURN the new light position, defined as follows.
	//selectedPos is a location on the mesh. Use this location to place the light source to cover the location as seen from camPos.
	//Further, the light should be at a distance of 1.5 from the origin of the scene - in other words, located on a sphere of radius 
	//1.5 around the origin.
	Vec3Df lightPos;
	float r = 1.5;
	Vec3Df distance = camPos - selectedPos;
	float a = distance.getSquaredLength();
	float b = 2 * Vec3Df::dotProduct(selectedPos, distance);
	float c = selectedPos.getSquaredLength() - pow(r, 2);
	float lambda1 = (-b + sqrt(pow(b, 2) - (4 * a*c))) / (2 * a);
	float lambda2 = (-b - sqrt(pow(b, 2) - (4 * a*c))) / (2 * a);
	if (lambda1 > 0) {
		lightPos = lambda1*distance + selectedPos;
	}
	else if(lambda2 > 0) {
		lightPos = lambda2*distance + selectedPos;
	}
	return lightPos;
}

Vec3Df userInteractionShadow(const Vec3Df & selectedPos, const Vec3Df & selectedNormal, const Vec3Df & lightPos)
{
	//RETURN the new light position such that the light towards the selectedPos is orthgonal to the normal at that location
	//--- in this way, the shading boundary will be exactly at this location.
	//there are several ways to do this, choose one you deem appropriate given the current light position
	//no panic, I will not judge what solution you chose, as long as the above condition is met.

	double dotProd = Vec3Df::dotProduct(selectedNormal, lightPos-selectedPos);
	double cosine = dotProd / (selectedNormal.getLength() * lightPos.getLength());
	Vec3Df proj = (lightPos.getLength()*cosine) * selectedNormal;
	Vec3Df orthogonal = ((proj - lightPos) + selectedPos)*1.5;
	return orthogonal;
}

Vec3Df userInteractionSpecular(const Vec3Df & selectedPos, const Vec3Df & selectedNormal, const Vec3Df & lightPos, const Vec3Df & cameraPos)
{
	//RETURN the new light position such that a specularity (highlight) will be located at selectedPos, when viewed from cameraPos and lit from ligthPos.
	//please ensure also that the light is at a distance of 1 from selectedpos! If the camera is on the wrong side of the surface (normal pointing the other way),
	//then just return the original light position.
	//There is only ONE way of doing this!
	Vec3Df d = selectedPos - cameraPos;
	Vec3Df n = selectedNormal;
	n.normalize();
	Vec3Df r = d - 2 * Vec3Df::dotProduct(d, n) * n;
	r.normalize();
	Vec3Df result = r + selectedPos;

	return result;
}
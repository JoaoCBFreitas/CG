#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <tinyxml2.h>
#include <string> 
#include <fstream>
#include <iostream>
#include <vector>

using namespace tinyxml2;
using namespace std;

float pi = M_PI;
struct Point {
	float x;
	float y;
	float z;
};
float alfa = 1, beta = 1, raio = 10;
//coordenadas de todos os triangulos
vector<Point> vertices;
void changeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void drawVertices(void){
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);

	for (size_t i = 0; i < vertices.size(); i++) {
		glVertex3f(vertices[i].x,
			vertices[i].y,
			vertices[i].z);
	}

	glEnd();
}

void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(raio * cos(beta) * sin(alfa), raio * sin(beta), raio * cos(beta) * cos(alfa),
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	drawVertices();

	// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char c, int xx, int yy) {
	switch (c) {
	case 'w':
		raio -= 1;
		break;
	case 's':
		raio += 1;
		break;
	}

	glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		alfa += 0.05;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		alfa -= 0.05;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		if (beta < pi / 2)
			beta += 0.05;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		if (beta > -pi / 2)
			beta -= 0.05;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}



void readFile(char* fich) {
	std::string ficheiro(fich);
	string linha;
	string novo;
	string delimiter = " ";
	int pos;
	float xx, yy, zz;
	Point p;
	ifstream file;
	file.open(ficheiro);
	if (file.is_open()) {

		while (getline(file, linha)) {

			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			xx = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.x = xx;

			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			yy = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.y = yy;

			pos = linha.find(delimiter);
			novo = linha.substr(0, pos);
			zz = atof(novo.c_str());
			linha.erase(0, pos + delimiter.length());
			p.z = zz;

			vertices.push_back(p);
		}
		file.close();
	}
	else {
		cout << "ERRO AO LER FICHEIRO" << endl;
	}
}
void parser(char* xmlDoc){
	XMLDocument doc;
	XMLError error = doc.LoadFile(xmlDoc);

	int i = 0;

	char** files = (char**)malloc(sizeof(char*) * 15);

	char pre[4] = "../";
	char file[15];

	if (!error) {
		XMLElement* elem = doc.FirstChildElement();
		for (const XMLElement* child = elem->FirstChildElement(); child; i++, child = child->NextSiblingElement()) {

			char* tmp = (char*)child->Attribute("file");
			strcpy(file, tmp);

			char ready[30];
			ready[0] = '\0';

			strcat(ready, pre);
			strcat(ready, file);

			files[i] = (char*)malloc(sizeof(char) * 15);
			strcpy(files[i], ready);

			memset(file, 0, 15);
			memset(ready, 0, 30);
		}
	}
	for (int j = 0; j < i; j++) {
		readFile(files[j]);
	}
}

int main(int argc, char **argv) {
	parser("../config.xml");

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

// put here the registration of the keyboard callbacks
	//funções da camera
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}

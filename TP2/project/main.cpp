#include <stdlib.h>
#include<stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <string> 
#include <fstream>
#include <vector>
#include "parser.h"

using namespace std;

float pi = M_PI;
float camX = 00, camY = 200, camZ = 200;
float alfa = 1, beta = 1, raio = 800;


//coordenadas de todos os triangulos
vector<Point> vertices;
//vector que contem os ficheiros+transformaçoes
vector<Ficheiros*> ficheiros;

vector<Transforma*> transf;
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
bool ver(Point a, Point b, Point c) {
	if (a.x ==2020 && a.y ==2020 && a.z == 2020 && b.x ==2020 && b.y ==2020 && b.z == 2020 && c.x ==2020 && c.y ==2020 && c.z == 2020) return true;
	else return false;
	
}
void drawVertices(void){
	bool f;
	vector<Point>::iterator iPonto=vertices.begin();
	vector<Ficheiros*>::iterator iFicheiros=ficheiros.begin();
	vector<Transforma*>::iterator iTransforma;
	while (iPonto != vertices.end()){
		f = true;
		Ficheiros* fich = *iFicheiros;

		iTransforma = fich->transfs.begin();

		glPushMatrix();
		while (iTransforma != fich->transfs.end()) {
			Transforma* trans = *iTransforma;
			if (strcmp((trans->transf), "scale") == 0) glScalef(trans->params[0], trans->params[1], trans->params[2]);
			if (strcmp((trans->transf), "rotate") == 0) glRotatef(trans->params[3], trans->params[0], trans->params[1], trans->params[2]);
			if (strcmp((trans->transf), "translate") == 0) 	glTranslatef(trans->params[0], trans->params[1], trans->params[2]);
			if (strcmp((trans->transf), "colour") == 0) glColor3f(trans->params[0], trans->params[1], trans->params[2]);
			iTransforma++;
		}
		while (iPonto != vertices.end() && f != false) {
			Point prim = *iPonto;
			iPonto++;
			Point sec = *iPonto;
			iPonto++;
			Point tre = *iPonto;
			iPonto++;
			if (!ver(prim, sec, tre)) {
				glBegin(GL_TRIANGLES);
				glVertex3f(prim.x, prim.y, prim.z);
				glVertex3f(sec.x, sec.y, sec.z);
				glVertex3f(tre.x, tre.y, tre.z);
				glEnd();
			}
			else {
				f = false;
			}
		}
		glPopMatrix();
		iFicheiros++;
	}
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


int main(int argc, char **argv) {
	parser("../config.xml",ficheiros,vertices);

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

#include <stdlib.h>
#include<stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <string> 
#include <fstream>
#include <vector>
#include "parser.h"
#include "iostream"

using namespace std;


float pi = M_PI;
float camX = 00, camY = 200, camZ = 200;
float alfa = 1, beta = 1, raio = 800;


//coordenadas de todos os triangulos
vector<Point> vertices;
//vector que contem os ficheiros+transformaçoes
vector<Ficheiros*> ficheiros;

vector<Transforma*> transf;


double** vbo;
GLuint buffers[20];

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
void mult1(double mT[1][4], double mCR[4][4], double res[1][4])
{
	res[0][0] = mT[0][0] * mCR[0][0] + mT[0][1] * mCR[1][0] + mT[0][2] * mCR[2][0] + mT[0][3] * mCR[3][0];
	res[0][1] = mT[0][0] * mCR[0][1] + mT[0][1] * mCR[1][1] + mT[0][2] * mCR[2][1] + mT[0][3] * mCR[3][1];
	res[0][2] = mT[0][0] * mCR[0][2] + mT[0][1] * mCR[1][2] + mT[0][2] * mCR[2][2] + mT[0][3] * mCR[3][2];
	res[0][3] = mT[0][0] * mCR[0][3] + mT[0][1] * mCR[1][3] + mT[0][2] * mCR[2][3] + mT[0][3] * mCR[3][3];
}
void mult2(double m[1][4], double n[4][1], double* res)
{
	*res = m[0][0] * n[0][0] + m[0][1] * n[1][0] + m[0][2] * n[2][0] + m[0][3] * n[3][0];
}
void mult3(float* m, float* v, float* res)
{
	for (int i = 0; i < 4; ++i) {

		res[i] = 0;
		for (int j = 0; j < 4; ++j) {

			res[i] += v[j] * m[i * 4 + j];

		}
	}
}
void getCatmullRomPoint(double t, vector<double> p0, vector<double> p1, vector<double> p2, vector<double> p3, double* pos, float* derivada)
{
	double x, y, z;
	double res[1][4];
	double matrixT[1][4] = { {powf(t,3),powf(t,2),t,1} };
	// Matrix Catmull-Rom
	double m[4][4] = { {-0.5f,  1.5f, -1.5f,  0.5f},
					   { 1.0f, -2.5f,  2.0f, -0.5f},
					   {-0.5f,  0.0f,  0.5f,  0.0f},
					   { 0.0f,  1.0f,  0.0f,  0.0f} };
	double px[4][1]= { { p0[0] }, { p1[0] }, { p2[0] }, { p3[0] } };
	double py[4][1] = { { p0[1] }, { p1[1] }, { p2[1] }, { p3[1] } };
	double pz[4][1] = { { p0[2] }, { p1[2] }, { p2[2] }, { p3[2] } };
	//Compute res=matrixT*m
	mult1(matrixT, m, res);

	//Compute x=res*px
	mult2(res, px, &x);
	//Compute y=res*py
	mult2(res, py, &y);
	//Compute z=res*pz
	mult2(res, pz, &z);
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;

	//Compute derivada=dt*q
	float q[4][4] = { 0 };
	float dt[4] = { 3 * powf(t,2), static_cast<float>(2 * t), 1, 0 };//derivada de matrixT
	float aX[4];
	float aY[4];
	float aZ[4];
	aX[0] = px[0][0];aX[1] = px[1][0];aX[2] = px[2][0];aX[3] = px[3][0];
	aY[0] = py[0][0];aY[1] = py[1][0];aY[2] = py[2][0];aY[3] = py[3][0];
	aZ[0] = pz[0][0];aZ[1] = pz[1][0];aZ[2] = pz[2][0];aZ[3] = pz[3][0];
	mult3(reinterpret_cast<float*>(*m), aX, q[0]);
	mult3(reinterpret_cast<float*>(*m), aY, q[1]);
	mult3(reinterpret_cast<float*>(*m), aZ, q[2]);
	mult3(*q, dt, derivada);
	
}
void getGlobalCatmullRomPoint(float gt, double pos[4], float* derivada, vector<vector<double>> curva, int tamanho)
{
	double t = gt * tamanho;
	int index = floor(t);
	t = t - index;
	int ind[4];
	ind[0] = (index + tamanho - 1) % tamanho;
	ind[1] = (ind[0] + 1) % tamanho;
	ind[2] = (ind[1] + 1) % tamanho;
	ind[3] = (ind[2] + 1) % tamanho;

	getCatmullRomPoint(t, curva[ind[0]], curva[ind[1]], curva[ind[2]], curva[ind[3]], pos, derivada);
}
int fill(Orbitas* orbita, vector<vector<double>>& curva) {
	int res = 0;
	vector<Point>::iterator p;
	for (p = orbita->pontos.begin(); p != orbita->pontos.end(); p++) {
		Point a = *p;
		vector<double> tmp;
		tmp.push_back(a.x);
		tmp.push_back(a.y);
		tmp.push_back(a.z);
		curva.push_back(tmp);
		res++;
	}
	return res;
}
void renderCurve(int x, Transforma* transforma) {
	int tamanho;//numero de pontos definidos no config
	static double tempo = 0;
	static double tempoPass[20];
	static double tt[20];
	Orbitas* orbita = transforma->orbita;

	vector<vector<double>> curva;
	tamanho = fill(orbita, curva);
	double position[4];
	float derivada[4];
	int npontos = 100;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < npontos; i++) {
		getGlobalCatmullRomPoint((double)i / npontos, position, derivada, curva, tamanho);
		glColor3f(0.1, 0.1, 0.1);
		glVertex3f(position[0], position[1], position[2]);
	}
	glEnd();
	
	getGlobalCatmullRomPoint(tt[x], position, derivada, curva, tamanho);
	if (tempo < ficheiros.size()) {
		tempoPass[x] = 1 / (orbita->tempo * 1000);
		tempo++;
	}
	tt[x] += tempoPass[x];
	glTranslatef(position[0], position[1], position[2]);
}
void rotation(int i,double x,double y ,double z, double tempo) {
	double static angulo[20];
	angulo[i] += 360 / (tempo * 1000);
	glRotatef(angulo[i], x,y,z);
}
void drawVBOs(void) {
	int x = 0;
	vector<Ficheiros*>::iterator iFicheiros; 
	iFicheiros= ficheiros.begin();
	vector<Transforma*>::iterator iTransforma;


	while (iFicheiros != ficheiros.end()) 
	{
		Ficheiros* fich = *iFicheiros;
		glPushMatrix();
		iTransforma = fich->transfs.begin();
		while (iTransforma != fich->transfs.end()) 
		{
			Transforma* trans = *iTransforma;
			if (strcmp((trans->transf), "scale") == 0) glScalef(trans->params[0], trans->params[1], trans->params[2]);
			if (strcmp((trans->transf), "rotate") == 0) {
				if(trans->transf == nullptr)glRotatef(trans->params[3], trans->params[0], trans->params[1], trans->params[2]);
				else {
					double t = trans->orbita->tempo;
					rotation(x, trans->params[0], trans->params[1], trans->params[2],t);
				}
			}
			if (strcmp((trans->transf), "translate") == 0) {
				if (trans->orbita == nullptr)glTranslatef(trans->params[0], trans->params[1], trans->params[2]);
				else renderCurve(x, trans);
			}
			if (strcmp((trans->transf), "colour") == 0) glColor3f(trans->params[0], trans->params[1], trans->params[2]);
			iTransforma++;
		}
		glBindBuffer(GL_ARRAY_BUFFER, buffers[x]);
		glBufferData(GL_ARRAY_BUFFER, fich->numpontos * 8 * 3, vbo[x], GL_STATIC_DRAW);
		glVertexPointer(3, GL_DOUBLE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, fich->numpontos);
		glPopMatrix();
		x++;
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
	glEnableClientState(GL_VERTEX_ARRAY);
	int tam = ficheiros.size();
	glGenBuffers(tam, buffers);

	drawVBOs();
	// End of frame
	glDeleteBuffers(tam, buffers);
	glutSwapBuffers();
}



void processKeys(unsigned char c, int xx, int yy) {
	switch (c) {
	case 'w':
		raio -= 100;
		break;
	case 's':
		raio += 100;
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
void initVBO() {
	bool f;
	int x = 0, pontos = 0;
	vbo = (double**)malloc(sizeof(double*) * ficheiros.size());
	vector<Point>::iterator iPonto = vertices.begin();
	vector<Ficheiros*>::iterator iFicheiros = ficheiros.begin();
	while (iPonto != vertices.end()) {
		Ficheiros* fich = *iFicheiros;
		vbo[x] = (double*)malloc(sizeof(double) * fich->numpontos * 3);
		f = true;
		pontos = 0;
		while (iPonto != vertices.end() && f!=false) {
			Point prim = *iPonto;
			iPonto++;
			Point sec = *iPonto;
			iPonto++;
			Point tre = *iPonto;
			iPonto++;
			if (!ver(prim, sec, tre)) {
				vbo[x][pontos++] = prim.x;
				vbo[x][pontos++] = prim.y;
				vbo[x][pontos++] = prim.z;
				vbo[x][pontos++] = sec.x;
				vbo[x][pontos++] = sec.y;
				vbo[x][pontos++] = sec.z;
				vbo[x][pontos++] = tre.x;
				vbo[x][pontos++] = tre.y;
				vbo[x][pontos++] = tre.z;
			}
			else {
				f = false;
				x++;
			}
		}
		iFicheiros++;
	}
}
void debug(vector<Ficheiros*>& files)
{
	for (vector<Ficheiros*>::iterator iFicheiros = files.begin(); iFicheiros != files.end(); iFicheiros++) {

		Ficheiros* ficheiro = *iFicheiros;
		cout << "File: " << ficheiro->file << endl;

		vector<Transforma*> v1 = ficheiro->transfs;

		for (vector<Transforma*>::iterator iTransforma = v1.begin(); iTransforma != v1.end(); iTransforma++) {
			Transforma* transformacao = *iTransforma;
			cout << "   Transformation: " << transformacao->transf << endl;

			Orbitas* orbita = transformacao->orbita;

			if (orbita) {

				cout << "   Time: " << orbita->tempo << endl;

				vector<Point> v2 = orbita->pontos;
				cout << "   Number of points: " << orbita->pontos.size() << endl;

				for (vector<Point>::iterator iOrbitas = v2.begin(); iOrbitas != v2.end(); iOrbitas++) {

					Point c = *iOrbitas;
					cout << "       :: X = " << c.x << " Y = " << c.y << " Z = " << c.z << endl;

				}
			}
			else
				cout << "   X = " << transformacao->params[0] << " Y = " << transformacao->params[1] << " Z = " << transformacao->params[2] << " Angle = " << transformacao->params[3] << endl;

			cout << endl;

		}

		cout << "------------------------------------------" << endl;
		cout << "" << endl;

	}
}

int main(int argc, char **argv) {
	parser("../config.xml",ficheiros,vertices);
	//debug(ficheiros);
	initVBO();
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);
// put here the registration of the keyboard callbacks
	//funções da camera
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);
#ifndef __APPLE__
	glewInit();
#endif
//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}

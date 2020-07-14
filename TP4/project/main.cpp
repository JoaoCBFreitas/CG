#include <stdlib.h>
#include<stdio.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <IL/il.h>
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
vector<Point> normals;
vector<Point> texturas;
vector<Luz*> luz;
vector<Transforma*> transf;
int nVertices=0;

double** vbo;
double* vs, * ns, * ts;
GLuint buffers[1];
GLuint norms[1];
GLuint txts[1];
GLuint* ids;
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
	mult1(matrixT, m, res);
	mult2(res, px, &x);
	mult2(res, py, &y);
	mult2(res, pz, &z);
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	float q[4][4] = { 0 };
	float dt[4] = { 3 * powf(t,2), static_cast<float>(2 * t), 1, 0 };//derivada de matrixT
	float aX[4];
	float aY[4];
	float aZ[4];
	aX[0] = px[0][0];
	aX[1] = px[1][0];
	aX[2] = px[2][0];
	aX[3] = px[3][0];

	aY[0] = py[0][0];
	aY[1] = py[1][0];
	aY[2] = py[2][0];
	aY[3] = py[3][0];

	aZ[0] = pz[0][0];
	aZ[1] = pz[1][0];
	aZ[2] = pz[2][0];
	aZ[3] = pz[3][0];
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
	int tamanho;
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


void adicionaCor(Cor* cor) {
	GLfloat amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat dif[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat spec[4] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat emis[4] = { 0.0, 0.0, 0.0, 1.0 };

	if (strcmp(cor->type, "diffuse") == 0) {
		dif[0] = static_cast<GLfloat>(cor->colors[0]);
		dif[1] = static_cast<GLfloat>(cor->colors[1]);
		dif[2] = static_cast<GLfloat>(cor->colors[2]);
		dif[3] = 1;

		glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	}

	if (strcmp(cor->type, "specular") == 0) {
		spec[0] = static_cast<GLfloat>(cor->colors[0]);
		spec[1] = static_cast<GLfloat>(cor->colors[1]);
		spec[2] = static_cast<GLfloat>(cor->colors[2]);
		spec[3] = 1;

		glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	}

	if (strcmp(cor->type, "emissive") == 0) {
		emis[0] = static_cast<GLfloat>(cor->colors[0]);
		emis[1] = static_cast<GLfloat>(cor->colors[1]);
		emis[2] = static_cast<GLfloat>(cor->colors[2]);
		emis[3] = 1;

		glMaterialfv(GL_FRONT, GL_EMISSION, emis);
	}

	if (strcmp(cor->type, "ambient") == 0) {
		amb[0] = static_cast<GLfloat>(cor->colors[0]);
		amb[1] = static_cast<GLfloat>(cor->colors[1]);
		amb[2] = static_cast<GLfloat>(cor->colors[2]);
		amb[3] = 1;

		glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	}
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
		
		if (fich->cor != NULL) adicionaCor(fich->cor);

		glDrawArrays(GL_TRIANGLES, 0, fich->numpontos); 
		glPopMatrix();
		x++;
		iFicheiros++;
	}
}
void adicionaLuz() {
	GLfloat pos[4] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diff[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat spotDir[3] = { 0.0, 0.0, -1.0 };

	Luz* l = luz.front();

	pos[0] = static_cast<GLfloat>(l->params[0]);
	pos[1] = static_cast<GLfloat>(l->params[1]);
	pos[2] = static_cast<GLfloat>(l->params[2]);

	if (strcmp(l->type, "POINT") == 0) {
		pos[3] = 1;
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

	}

	else if (strcmp(l->type, "DIRECTIONAL") == 0) {
		pos[3] = 0;
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

	}

	else if (strcmp(l->type, "SPOTLIGHT") == 0) {
		pos[3] = 1;
		spotDir[0] = static_cast<GLfloat>(l->params[0]);
		spotDir[1] = static_cast<GLfloat>(l->params[1]);
		spotDir[2] = static_cast<GLfloat>(l->params[2]);

		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
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

	adicionaLuz();

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glVertexPointer(3, GL_DOUBLE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, norms[0]);
	glNormalPointer(GL_DOUBLE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, ids[0]);
	glTexCoordPointer(2, GL_DOUBLE, 0, 0);
	
	drawVBOs();
	float amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	float dif[4] = { 1.0, 1.0, 1.0, 1.0 };
	float spec[4] = { 0.0, 0.0, 0.0, 1.0 };
	float emi[4] = { 0.0, 0.0, 0.0, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialfv(GL_FRONT, GL_EMISSION, emi);
	
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
int load(string txt) {
	unsigned int t, tw, th;
	unsigned char* data;
	unsigned int id;

	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilGenImages(1, &t);
	ilBindImage(t);
	ilLoadImage((ILstring)txt.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	data = ilGetData();

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return id;
}
void fillArrays() {
	glGenBuffers(1, buffers);
	glGenBuffers(1, norms);
	glGenBuffers(1, txts);
	int nv = 0, nn = 0, nt = 0;
	bool f;
	vs = (double*)malloc(sizeof(double) * nVertices * 3);
	ns = (double*)malloc(sizeof(double) * nVertices * 3);
	ts = (double*)malloc(sizeof(double) * nVertices * 2);
	vector<Point>::iterator iPonto;
	vector<Point>::iterator iNormais;
	vector<Point>::iterator iTexturas;
	iPonto = vertices.begin();
	iNormais = normals.begin();
	iTexturas = texturas.begin();
	while (iPonto != vertices.end()) {
		f = false;
		while (!f) {
			Point ponto1 = *iPonto;
			iPonto++;
			Point ponto2 = *iPonto;
			iPonto++;
			Point ponto3 = *iPonto;
			iPonto++;
			
			Point normal1 = *iNormais;
			iNormais++;
			Point normal2 = *iNormais;
			iNormais++;
			Point normal3=*iNormais;
			iNormais++;
			
			Point text1 = *iTexturas;
			iTexturas++;
			Point text2 = *iTexturas;
			iTexturas++;
			Point text3 = *iTexturas;
			iTexturas++;

			if (!ver(ponto1, ponto2, ponto3)) {
				vs[nv++] = ponto1.x;
				vs[nv++] = ponto1.y;
				vs[nv++] = ponto1.z;
				vs[nv++] = ponto2.x;
				vs[nv++] = ponto2.y;
				vs[nv++] = ponto2.z;
				vs[nv++] = ponto3.x;
				vs[nv++] = ponto3.y;
				vs[nv++] = ponto3.z;
			
				ns[nn++] = normal1.x;
				ns[nn++] = normal1.y;
				ns[nn++] = normal1.z;
				ns[nn++] = normal2.x;
				ns[nn++] = normal2.y;
				ns[nn++] = normal2.z;
				ns[nn++] = normal3.x;
				ns[nn++] = normal3.y;
				ns[nn++] = normal3.z;
				
				ts[nt++] = text1.x;
				ts[nt++] = text1.y;
				//ts[nt++] = text1.z;
				ts[nt++] = text2.x;
				ts[nt++] = text2.y;
				//ts[nt++] = text2.z;
				ts[nt++] = text3.x;
				ts[nt++] = text3.y;
				//ts[nt++] = text3.z;
				
			}
			else f = true;
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, nVertices * 8 * 3, vs, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, norms[0]);
	glBufferData(GL_ARRAY_BUFFER, nVertices * 8 * 3, ns, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, txts[0]);
	glBufferData(GL_ARRAY_BUFFER, nVertices * 8 * 2, ts, GL_STATIC_DRAW);
	free(vs);
	free(ns);
}
int main(int argc, char **argv) {
	parser("../config.xml",ficheiros,vertices,luz,normals,texturas,&nVertices);
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
	fillArrays();
	int i = 0;
	ids = (GLuint*)malloc(sizeof(GLuint) * ficheiros.size());
	vector<Ficheiros*>::iterator iFicheiros;
	for (iFicheiros = ficheiros.begin(); iFicheiros != ficheiros.end(); iFicheiros++) {
		Ficheiros* fich = *iFicheiros;
		if (fich->textura != NULL) {
			char file[50] = "../texturas/";
			char* txt = strcat(file, fich->textura);
			ids[i] = load(string(txt));
		}
		i++;
	}
//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}

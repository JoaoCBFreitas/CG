

#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <tuple>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <IL/il.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif


float camX = 20, camY =3, camZ = 20,nPontos;
int startX, startY, tracking = 0;
float lx = 0.0f, lz = -1.0f;
float angle = 0.0;
float pi = M_PI;
int alpha = 0, beta = 45, r = 50;
GLuint buffer[1];
float* vertexB;
float* aux;
float rotacao = 5;
using namespace std;
typedef tuple<double, double> coords;
vector<coords> flor;
unsigned int t, tw, th;
unsigned char* imageData;

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective
	gluPerspective(45,ratio,1,1000);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

float height(float a, float b) {
	for (int i = 0; aux[i] > -128 && aux[i] < 129; i+=3) {
		if (aux[i] == a && aux[i + 2] == b) return aux[i + 1];
	}
	return -2020;
}
void drawOuterTeapots(float a, float b, float c) {
	glPushMatrix();
	glTranslatef(a, 2, b);
	glRotatef(c, 0, 1, 0);
	glColor3f(1, 0, 0);
	glutSolidTeapot(2);
	glPopMatrix();
}
void drawInnerTeapots(float a, float b, float c) {
	glPushMatrix();
	glTranslatef(a, 2, b);
	glRotatef(c, 0, 1, 0);
	glColor3f(0, 0, 1);
	glutSolidTeapot(2);
	glPopMatrix();
}
void drawOuterCircle(void) {
	int aini = 360 / 15;
	float c = 0;
	for (int a = 0; a + aini <= 360; a += aini) {
		double h1 = a * pi / 180;
		drawOuterTeapots(sin(h1) * 35, cos(h1) * 35, c);
		c += aini;
		if (c == 360)c = 0;
	}
}
void drawInnerCircle(void) {
	int aini = 360 / 8;
	float c = 270;
	for (int a = 0; a + aini <= 360; a += aini) {
		double h1 = a * pi / 180;
		drawInnerTeapots(sin(h1) * 15, cos(h1) * 15, c);
		c += aini;
		if (c == 360) c = 0;
	}
}
void drawTaurus(void) {
	glPushMatrix();
	glColor3f(0.91, 0.33, 0.5);
	int h = height(0, 0);
	glTranslatef(0, h, 0);
	glutSolidTorus(1, 2, 40, 40);
	glPopMatrix();
}

void drawTree(float x,float y, float z) {
	glPushMatrix();
	//tronco
	glTranslatef(x, y, z);
	glRotatef(90, -1, 0, 0);
	glColor3f(0.5f, 0.35f, 0.05f);
	glutSolidCone(1, 4, 40, 40);
	glPopMatrix();
	//folhas
	glPushMatrix();
	glTranslatef(x, y+3, z);
	glRotatef(90, -1, 0, 0);
	glColor3f(0, 1, 0);
	glutSolidCone(3, 6, 40, 40);
	glPopMatrix();
}
void drawForest(void) {
	vector<coords>::iterator i;
	i = flor.begin();
	while (i != flor.end()) {
		coords p = *i;
		float x = get<0>(p);
		float z = get<1>(p);
		float y=height(x, z);
		if(y!=-2020) drawTree(x,y, z);
		i++;
	}
}
void terrainVBO(unsigned int th, unsigned int tw, unsigned char* imageData) {
	//numero de vertices*3 coordenadas
	nPontos = th * tw*3;
	vertexB = (float*)malloc(nPontos*12*4* sizeof(float));
	aux = (float*)malloc(nPontos * 12 * 4 * sizeof(float));
	int pos = 0;
	int k=0;
	//Criar o plano (posx,0,posy)

	for (int i = 1; i <= th;i++) {
		for (int j = 1; j <= tw;j++) {
			int p1, p2;
			p1 = imageData[k]*30/255;
			p2 = imageData[k+2]*30/255;		
			vertexB[pos] =(float) j - (tw/2); //-128
			vertexB[pos + 1] =(float)p1;
			vertexB[pos + 2] =(float) i-(th/2); //-128

			vertexB[pos + 3] = (float) j - (tw / 2); //-128
			vertexB[pos + 4] = (float) p2;
			vertexB[pos + 5] = (float) i + 1 - (th / 2); //-127
			pos += 6;
			k++;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	//Aqui dizemos qual é GLuint que vamos usar e quandos buffers tem
	glGenBuffers(1, buffer);

	// Informamos qual vai ser o buffer que vamos usar para guardar a VBO
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	//Temos 2 campos importantes (2º e 3º), no 2º metemos a memória necessária para guardar todas as coordenadas, e no 3º informamos o array que tem as coordenadas
	glBufferData(GL_ARRAY_BUFFER, nPontos*12*4 * sizeof(float), vertexB, GL_STATIC_DRAW);
	for (int i = 0;vertexB[i]>-128 && vertexB[i]<129 ; i++) {
		aux[i] = vertexB[i];
	}
	free(vertexB);

}

void drawTerrain() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	for (int i = 0; i < th - 1; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, (th)*2*i, (tw)*2);
	}
	glPopMatrix();
    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLoadIdentity();
	float h = height(round(camX), round(camZ));
	if (h == -2020) {
		h = 0;
		camY = 0;
	}
	gluLookAt(camX, camY+h, camZ, 
		      camX+lx,camY+h,camZ+lz,
			  0.0f,1.0f,0.0f);
	drawTerrain();



	
	drawTaurus();
	glPushMatrix();
	glRotatef(rotacao, 0, 1, 0);
	drawOuterCircle();
	glPopMatrix();
	glutPostRedisplay();

	glPushMatrix();
	glRotatef(-rotacao, 0, 1, 0);
	drawInnerCircle();
	glPopMatrix();
	glutPostRedisplay();

	rotacao += 5;
	drawForest();
	
// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

	float fraction = 2;	
	if (key == 'a') {
		if ((lx > 0 && lz > 0)||(lx<0 && lz<0)) {
			camX -= lz * fraction;
			camZ -= lx * fraction;
		}
		else {
			camX += lz * fraction;
			camZ += lx * fraction;
		}
	}
	if(key=='d'){
		if ((lx > 0 && lz > 0) || (lx < 0 && lz < 0)) {
			camX += lz * fraction;
			camZ += lx * fraction;
		}
		else {
			camX -= lz * fraction;
			camZ -= lx * fraction;
		}
	}
}
void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 2;

	switch (key) {
	case GLUT_KEY_LEFT:
		angle -= 0.1f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_RIGHT:
		angle += 0.1f;
		lx = sin(angle);
		lz = -cos(angle);
		break;
	case GLUT_KEY_UP:
		camX += lx * fraction;
		camZ += lz * fraction;
		break;
	case GLUT_KEY_DOWN:
		camX -= lx * fraction;
		camZ -= lz * fraction;
		break;
	}
}


void processMouseButtons(int button, int state, int xx, int yy) {
	
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
		else
			tracking = 0;
	}
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha += (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			
			r -= yy - startY;
			if (r < 3)
				r = 3.0;
		}
		tracking = 0;
	}
}


void processMouseMotion(int xx, int yy) {

	int deltaX, deltaY;
	int alphaAux, betaAux;
	int rAux;

	if (!tracking)
		return;

	deltaX = xx - startX;
	deltaY = yy - startY;

	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0)
			betaAux = 85.0;
		else if (betaAux < -85.0)
			betaAux = -85.0;

		rAux = r;
	}
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r - deltaY;
		if (rAux < 3)
			rAux = 3;
	}
	camX = rAux * sin(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camZ = rAux * cos(alphaAux * 3.14 / 180.0) * cos(betaAux * 3.14 / 180.0);
	camY = rAux * 							     sin(betaAux * 3.14 / 180.0);
	if (camY < height(camX, camZ))camY = height(camX, camZ);
}


void init() {
// 	Load the height map "terreno.jpg"
	ilGenImages(1, &t);
	ilBindImage(t);
	// terreno.jpg is the image containing our height map
	ilLoadImage((ILstring) "terreno.jpg");
	// convert the image to single channel per pixel
	// with values ranging between 0 and 255
	ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);
// 	Build the vertex arrays
	imageData = ilGetData();
// 	OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}


int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1024,768);
	glutCreateWindow("CG@DI-UM");
		

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);


	glEnable(GL_DEPTH_TEST);

	glewInit();
	ilInit();

	init();
	terrainVBO(th, tw,imageData);
	std::srand(time(NULL));
	for (int i = 0; i < 200; i++) {
		int x = -100 + rand() % (100 + 100 + 1);
		int z = -100 + rand() % (100 + 100 + 1);
		if (sqrt(pow(x, 2) + pow(z, 2)) > 50) {
			coords point = make_tuple(x, z);
			flor.push_back(point);
		}
		else i--;
	}
// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}


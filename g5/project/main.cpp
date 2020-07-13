
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tuple>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

float alfa = 0.0f, beta = 0.5f, radius = 100.0f;
float camX, camY, camZ;
float pi = M_PI;
float rotacao = 5;


using namespace std;
typedef tuple<double, double> coords;
vector<coords> flor;
void spherical2Cartesian() {

	camX = radius * cos(beta) * sin(alfa);
	camY = radius * sin(beta);
	camZ = radius * cos(beta) * cos(alfa);
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

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
void drawPlane(void) {
	glColor3f(0.2f, 0.8f, 0.2f);
	glBegin(GL_TRIANGLES);
	glVertex3f(100.0f, 0, -100.0f);
	glVertex3f(-100.0f, 0, -100.0f);
	glVertex3f(-100.0f, 0, 100.0f);

	glVertex3f(100.0f, 0, -100.0f);
	glVertex3f(-100.0f, 0, 100.0f);
	glVertex3f(100.0f, 0, 100.0f);
	glEnd();
}
void drawOuterTeapots(float a, float b,float c) {
	glPushMatrix();
	glTranslatef(a, 2, b);
	glRotatef(c, 0, 1, 0);
	glColor3f(1, 0, 0);
	glutSolidTeapot(2);
	glPopMatrix();
}
void drawOuterCircle(void) {
	int aini = 360 / 15;
	float c = 0;
	for (int a = 0; a+aini <= 360; a +=aini) {
		double h1 = a * pi / 180;
		drawOuterTeapots(sin(h1)*35, cos(h1)*35,c);
		c += aini;
		if (c == 360)c = 0;
	}
}

void drawInnerTeapots(float a, float b,float c) {
	glPushMatrix();
	glTranslatef(a, 2, b);
	glRotatef(c, 0, 1, 0);
	glColor3f(0, 0, 1);
	glutSolidTeapot(2);
	glPopMatrix();
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
	glColor3f(0.91, 0.33, 0.5);
	glutSolidTorus(1, 2, 40, 40);
}
void drawTree(int x,int z){
	glPushMatrix();
	//tronco
	glTranslatef(x, 0, z);
	glRotatef(90, -1, 0, 0);
	glColor3f(0.5f, 0.35f, 0.05f);
	glutSolidCone(1, 4, 40, 40);
	glPopMatrix();
	//folhas
	glPushMatrix();
	glTranslatef(x, 3, z);
	glRotatef(90, -1, 0, 0);
	glColor3f(0,1,0);
	glutSolidCone(3,6,40,40);
	glPopMatrix();
}

void drawForest(void) {
	vector<coords>::iterator i;
	i = flor.begin();
	while(i!=flor.end()){
		coords p = *i;
		drawTree(get<0>(p), get<1>(p));
		i++;
	}
}

void renderScene(void) {
	int i = 0;
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(camX, camY, camZ,
		0.0, 0.0, 0.0,
		0.0f, 1.0f, 0.0f);

	drawPlane();
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


void processKeys(unsigned char c, int xx, int yy) {

// put code to process regular keys in here

}


void processSpecialKeys(int key, int xx, int yy) {

	switch (key) {

	case GLUT_KEY_RIGHT:
		alfa -= 0.1; break;

	case GLUT_KEY_LEFT:
		alfa += 0.1; break;

	case GLUT_KEY_UP:
		beta += 0.1f;
		if (beta > 1.5f)
			beta = 1.5f;
		break;

	case GLUT_KEY_DOWN:
		beta -= 0.1f;
		if (beta < -1.5f)
			beta = -1.5f;
		break;

	case GLUT_KEY_PAGE_DOWN: radius -= 1.0f;
		if (radius < 1.0f)
			radius = 1.0f;
		break;

	case GLUT_KEY_PAGE_UP: radius += 1.0f; break;
	}
	spherical2Cartesian();
	glutPostRedisplay();

}


void printInfo() {

	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));

	printf("\nUse Arrows to move the camera up/down and left/right\n");
	printf("Home and End control the distance from the camera to the origin");
}


int main(int argc, char **argv) {
// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutSpecialFunc(processSpecialKeys);

//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	spherical2Cartesian();

	printInfo();

	srand(time(NULL));
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
	
	return 1;
}
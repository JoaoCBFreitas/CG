#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
float pi = M_PI;


float alfa = 1, beta = 1, raio = 20;
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
//1,10,10
void drawSphere(int radius, int slices, int stacks) {
	double alpha = 0;//horizontal
	double deltaAlpha = (2 * pi) / slices; 
	double beta = 0;//vertical
	double deltaBeta = pi / stacks;
	double p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, p4x, p4y, p4z;
	glBegin(GL_TRIANGLES);
	for (int i = 0; i <=stacks; i++) {//preenche horizontalmente
		alpha = 0;
		for (int j = 0; j <= slices; j++) {//preenche de cima para baixo
			p1x = radius * sin(beta) * sin(alpha);
			p1y = radius * cos(beta);
			p1z = radius * sin(beta) * cos(alpha);

			p2x = radius * sin(beta + deltaBeta) * sin(alpha);
			p2y = radius * cos(beta + deltaBeta);
			p2z = radius * sin(beta + deltaBeta) * cos(alpha);

			p3x = radius * sin(beta) * sin(alpha + deltaAlpha);
			p3y = radius * cos(beta);
			p3z = radius * sin(beta) * cos(alpha + deltaAlpha);

			p4x = radius * sin(beta + deltaBeta) * sin(alpha + deltaAlpha);
			p4y = radius * cos(beta + deltaBeta);
			p4z = radius * sin(beta + deltaBeta) * cos(alpha + deltaAlpha);
			//1-4-3
			glColor3f(0, 1, 0);
			glVertex3f(p1x, p1y, p1z);
			glVertex3f(p4x, p4y, p4z);
			glVertex3f(p3x, p3y, p3z);
			//1-2-4
			glColor3f(0, 0, 1);
			glVertex3f(p1x,p1y,p1z);
			glVertex3f(p2x, p2y, p2z);
			glVertex3f(p4x, p4y, p4z);
			
			

			
			alpha += deltaAlpha;
		}
		beta += deltaBeta;
	}
	glEnd();
}
void drawCylinder(float radius, float height, int slices) {
	//circulo inferior
	glBegin(GL_TRIANGLES);
	glColor3f(1,1,1);
	for (int a = 0; a < 360; a += 360 / slices){
		double h1 = a * pi / 180;
		int n = a + 360 / slices;
		double h2 = n  * pi / 180;
		glVertex3f(sin(h1) * radius, 0, cos(h1) * radius);
		glVertex3f(0, 0, 0);
		glVertex3f(sin(h2) * radius, 0, cos(h2) * radius);
	}
	glEnd();


	//circulo superior
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0);
	for (int a = 0; a < 360; a += 360 / slices) {
		double h1 = a * pi / 180;
		int n = a + 360 / slices;
		double h2 = n * pi / 180;
		
		glVertex3f(0, height, 0);
		glVertex3f(sin(h1) * radius, height, cos(h1) * radius);
		glVertex3f(sin(h2) * radius, height, cos(h2) * radius);
	}
	glEnd();

	//faces
	glBegin(GL_TRIANGLES);
	for (int a = 0; a < 360; a += 360 / slices) {
		double h1 = a * pi / 180;
		int n = a + 360 / slices;
		double h2 = n * pi / 180;
		glColor3f(1, 1, 0);
		glVertex3f(sin(h1) * radius, height, cos(h1) * radius);
		glVertex3f(sin(h1) * radius, 0, cos(h1) * radius);
		glVertex3f(sin(h2) * radius, 0, cos(h2) * radius);

		
		glColor3f(1, 0, 1);
		glVertex3f(sin(h2) * radius, height, cos(h2) * radius);
		glVertex3f(sin(h1) * radius, height, cos(h1) * radius);
		glVertex3f(sin(h2) * radius, 0, cos(h2) * radius);
		
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
	
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1, 1, 1);
	//drawCylinder(1,2,10);
	drawSphere(1,10,10);
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
	glEnable(GL_CULL_FACE);
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}

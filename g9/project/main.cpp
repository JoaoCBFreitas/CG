#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <math.h>

#define _PI_ 3.14159

float alfa = 0.0f, beta = 0.0f, radius = 5.0f;
float camX, camY, camZ;

GLuint vertexCount, vertices;
GLuint buffer[2];
float* vertexB, * normalB;
int timebase = 0, frame = 0;
float amb[3] = { 0,0,0.5 },
diff[3] = { 1,1,1 },
matt[4] = { 0,0,1,1 };
int n_pontos;
float pos[4] = { 1.0, 1.0, 1.0, 0.0 };
void sphericalToCartesian() {

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



void prepareCilinder(float height, float radius, int sides) {

	float *v;

	v = (float *)malloc(sizeof(float) * 4 * 3 * 3 * sides);

	int vertex = 0;
	float delta = 2.0f * _PI_ / sides;

	for (int i = 0; i < sides; ++i) {
		// top
		// central point
		v[vertex*3 + 0] = 0.0f; 
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = 0.0f;

		vertex++;
		v[vertex*3 + 0] = radius * sin( i * delta);
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = radius * cos( i * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( (i+1) * delta);
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = radius * cos( (i+1) * delta);

		// body
		// triângulo 1
		vertex++;
		v[vertex*3 + 0] = radius * sin( (i+1) * delta);
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = radius * cos( (i+1) * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( i * delta);
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = radius * cos( i * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( i * delta);
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = radius * cos( i * delta);

		// triangle 2
		vertex++;
		v[vertex*3 + 0] = radius * sin( (i+1) * delta);
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = radius * cos( (i+1) * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( (i+1) * delta);
		v[vertex*3 + 1] = height /2.0f;
		v[vertex*3 + 2] = radius * cos( (i+1) * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( i * delta);
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = radius * cos( i * delta);

		// base
		// central point
		vertex++;
		v[vertex*3 + 0] = 0.0f; 
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = 0.0f;

		vertex++;
		v[vertex*3 + 0] = radius * sin( (i+1) * delta);
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = radius * cos( (i+1) * delta);

		vertex++;
		v[vertex*3 + 0] = radius * sin( i * delta);
		v[vertex*3 + 1] = -height /2.0f;
		v[vertex*3 + 2] = radius * cos( i * delta);

		vertex++;
	}

	vertexCount = vertex;

	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER,vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexCount * 3, v,     GL_STATIC_DRAW);

	free(v);

}
//Contrução da VBO do cilindro. Esta função só é chamada uma única vez.
void cilindroVBO(float raio, float alt, int lados) {

	int i = 0, n = 0;
	float* vertexB = NULL, * normalB = NULL;
	float angulo = 2 * _PI_ / lados, laux1, laux2 = 0;

	//Numero de coordenadas no array (4 triangulos * lados * nº de pontos de cada triangulo + nº de coordenadas de cada ponto)
	n_pontos = 4 * lados * 3 * 3;
	vertexB = (float*)malloc(n_pontos * sizeof(float));
	normalB = (float*)malloc(n_pontos * sizeof(float));


	//Activar Buffers
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (; lados > 0; lados--) {
		laux1 = laux2; laux2 += angulo;

		//Base Superior
		vertexB[i++] = 0; vertexB[i++] = alt; vertexB[i++] = 0;
		vertexB[i++] = sin(laux1); vertexB[i++] = alt; vertexB[i++] = cos(laux1);
		vertexB[i++] = sin(laux2); vertexB[i++] = alt; vertexB[i++] = cos(laux2);

		normalB[n++] = 0; normalB[n++] = 1; normalB[n++] = 0;
		normalB[n++] = 0; normalB[n++] = 1; normalB[n++] = 0;
		normalB[n++] = 0; normalB[n++] = 1; normalB[n++] = 0;

		//Base Inferior
		vertexB[i++] = 0; vertexB[i++] = 0; vertexB[i++] = 0;
		vertexB[i++] = sin(laux2); vertexB[i++] = 0; vertexB[i++] = cos(laux2);
		vertexB[i++] = sin(laux1); vertexB[i++] = 0; vertexB[i++] = cos(laux1);

		normalB[n++] = 0; normalB[n++] = -1; normalB[n++] = 0;
		normalB[n++] = 0; normalB[n++] = -1; normalB[n++] = 0;
		normalB[n++] = 0; normalB[n++] = -1; normalB[n++] = 0;


		//Dois triângulos para os lados
		vertexB[i++] = sin(laux1); vertexB[i++] = alt; vertexB[i++] = cos(laux1);
		vertexB[i++] = sin(laux1); vertexB[i++] = 0; vertexB[i++] = cos(laux1);
		vertexB[i++] = sin(laux2); vertexB[i++] = 0; vertexB[i++] = cos(laux2);

		normalB[n++] = sin(laux1); normalB[n++] = 0; normalB[n++] = cos(laux1);
		normalB[n++] = sin(laux1); normalB[n++] = 0; normalB[n++] = cos(laux1);
		normalB[n++] = sin(laux2); normalB[n++] = 0; normalB[n++] = cos(laux2);

		vertexB[i++] = sin(laux1); vertexB[i++] = alt; vertexB[i++] = cos(laux1);
		vertexB[i++] = sin(laux2); vertexB[i++] = 0; vertexB[i++] = cos(laux2);
		vertexB[i++] = sin(laux2); vertexB[i++] = alt; vertexB[i++] = cos(laux2);

		normalB[n++] = sin(laux1); normalB[n++] = 0; normalB[n++] = cos(laux1);
		normalB[n++] = sin(laux2); normalB[n++] = 0; normalB[n++] = cos(laux2);
		normalB[n++] = sin(laux2); normalB[n++] = 0; normalB[n++] = cos(laux2);

	}

	glGenBuffers(2, buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	glBufferData(GL_ARRAY_BUFFER, n_pontos * sizeof(float), vertexB, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, n_pontos * sizeof(float), normalB, GL_STATIC_DRAW);

	free(vertexB);
	free(normalB);
}
void desenhaVBO() {
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glNormalPointer(GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, n_pontos);
}
void drawCilinder() {
		
	glBindBuffer(GL_ARRAY_BUFFER,vertices);
	glVertexPointer(3,GL_FLOAT,0,0);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}


void renderScene(void) {


	float fps;
	int time;
	char s[64];
	float dark[] = { 0.2, 0.2, 0.2, 1.0 };
	float white[] = { 0.8, 0.8, 0.8, 1.0 };
	float red[] = { 0.8, 0.2, 0.2, 1.0 };
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(camX,camY,camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_AMBIENT, dark);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);


	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 128);

	desenhaVBO();

	frame++;
	time=glutGet(GLUT_ELAPSED_TIME); 
	if (time - timebase > 1000) { 
		fps = frame*1000.0/(time-timebase); 
		timebase = time; 
		frame = 0; 
		sprintf(s, "FPS: %f6.2", fps);
		glutSetWindowTitle(s);
	} 

// End of frame
	glutSwapBuffers();
}



void processKeys(int key, int xx, int yy) 
{
	switch(key) {
	
		case GLUT_KEY_RIGHT: 
						alfa -=0.1; break;

		case GLUT_KEY_LEFT: 
						alfa += 0.1; break;

		case GLUT_KEY_UP : 
						beta += 0.1f;
						if (beta > 1.5f)
							beta = 1.5f;
						break;

		case GLUT_KEY_DOWN: 
						beta -= 0.1f; 
						if (beta < -1.5f)
							beta = -1.5f;
						break;

		case GLUT_KEY_PAGE_DOWN : radius -= 0.1f; 
			if (radius < 0.1f)
				radius = 0.1f;
			break;

		case GLUT_KEY_PAGE_UP: radius += 0.1f; break;

	}
	sphericalToCartesian();

}



void initGL() {

// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	GLfloat dark[4] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat white[4] = { 1.0, 1.0, 1.0, 1.0 };
	// light colors
	glLightfv(GL_LIGHT0, GL_AMBIENT, dark);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
// init
	sphericalToCartesian();
	glEnableClientState(GL_VERTEX_ARRAY);

	//prepareCilinder(2,1,16);
}


int main(int argc, char **argv) {

// init
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("CG@DI-UM");
		
// callback registration
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	
// keyboard callback registration
	glutSpecialFunc(processKeys);

#ifndef __APPLE__	
// init GLEW
	glewInit();
#endif	


	initGL();
	cilindroVBO(2, 1, 30);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glutMainLoop();
	
	return 1;
}




#include<stdio.h>
#include<stdlib.h>

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


float camX = 00, camY = 200, camZ = 200,nPontos,first=0;
int startX, startY, tracking = 0;

int alpha = 0, beta = 45, r = 50;
GLuint buffer[1];
float* vertexB;


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

void terrainVBO(unsigned int th, unsigned int tw, unsigned char* imageData) {
	//numero de vertices*3 coordenadas
	nPontos = th * tw*3;
	vertexB = (float*)malloc(nPontos*12*4* sizeof(float));

	int pos = 0;
	int k=0;
	//Criar o plano (posx,0,posy)

	for (int i = 1; i <= th;i++) {
		for (int j = 1; j <= tw;j++) {
			int p1, p2;
			if (imageData[k] >= 100 || imageData[k] < 0)p1 = 0;
			else p1 = imageData[k];
			if (imageData[k+2] >= 100 ||imageData[k+2]<0)p2 = 0;
			else p2 = imageData[k];

			vertexB[pos] =(float) j - (tw/2); //-128
			vertexB[pos + 1] =p1;
			vertexB[pos + 2] =(float) i-(th/2); //-128

			vertexB[pos + 3] = (float) j - (tw / 2); //-128
			vertexB[pos + 4] = p2;
			vertexB[pos + 5] = (float) i + 1 - (th / 2); //-127
			pos += 6;
			k++;
		}

	}
	int i = 0;
	for (i; imageData[i] != NULL; i++);
	std::cout << "ImageData " << i << std::endl;
	std::cout << "vertexB " << pos << std::endl;
	std::cout << "ImageData no vertex " << k << std::endl;
	glEnableClientState(GL_VERTEX_ARRAY);
	//Aqui dizemos qual é GLuint que vamos usar e quandos buffers tem
	glGenBuffers(1, buffer);

	// Informamos qual vai ser o buffer que vamos usar para guardar a VBO
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	//Temos 2 campos importantes (2º e 3º), no 2º metemos a memória necessária para guardar todas as coordenadas, e no 3º informamos o array que tem as coordenadas
	glBufferData(GL_ARRAY_BUFFER, nPontos*12*4 * sizeof(float), vertexB, GL_STATIC_DRAW);
	free(vertexB);

}

void drawTerrain() {
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, first, nPontos);
    // colocar aqui o código de desnho do terreno usando VBOs com TRIANGLE_STRIPS
}



void renderScene(void) {

	float pos[4] = {-1.0, 1.0, 1.0, 0.0};

	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLoadIdentity();
	gluLookAt(camX, camY, camZ, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

	drawTerrain();


// End of frame
	glutSwapBuffers();
}



void processKeys(unsigned char key, int xx, int yy) {

// put code to process regular keys in here
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
	glutInitWindowSize(320,320);
	glutCreateWindow("CG@DI-UM");
		

// Required callback registry 
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

// Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glewInit();
	ilInit();

	init();
	terrainVBO(th, tw,imageData);
// enter GLUT's main cycle
	glutMainLoop();
	
	return 0;
}


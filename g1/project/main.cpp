#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdio.h>
#include <time.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>



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



void renderScene(void) {
	time_t seconds;
	seconds = time(NULL);
	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(0.0,0.0,5.0, 
		      0.0,0.0,-1.0,
			  0.0f,1.0f,0.0f);

// put drawing instructions here
	float fr = sin(seconds);
	
	//if (fr < 0) fr += 1;
	fr += 1;
	glutWireTeapot(fr);
	// End of frame
	glutSwapBuffers();
}




int main(int argc, char **argv) {
// put GLUT init here
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutCreateWindow("CG@DI");
// put callback registration here
	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
// OpenGL settings 
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f,0.0f,0.0f,0.0f);

// enter GLUT's main loop
	glutMainLoop();
	
	return 1;
}


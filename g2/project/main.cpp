#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>
float x = 1;
float y = 1;
float z = 1;
float angulo=0;
const char* com = "nada";
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

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(5.0,5.0,5.0, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put the geometric transformations here
	if (com == "aumenta") {
		x += 0.1;
		y += 0.1;
		z += 0.1;
		glScalef(x, y, z);
	}
	if (com == "diminui") {
		x -= 0.1;
		y -= 0.1;
		z -= 0.1;
		glScalef(x, y,z);
	}
	if (com == "esquerda") {
		x -= 0.1;
		glTranslatef(x, 0, 0);
	}
	if (com == "direita") {
		x += 0.1;
		glTranslatef(x, 0, 0);
	}
	if (com == "acima") {
		z -= 0.1;
		glTranslatef(0, 0, z);
	}
	if (com == "abaixo") {
		z += 0.1;
		glTranslatef(0, 0, z);
	}
	if (com == "rEsq") {
		angulo += 90;
		if (angulo > 360) angulo -= 360;
		glRotatef(angulo, 0, 1, 0);
	}
	if (com == "rDir") {
		angulo -= 90;
		if (angulo < 0) angulo += 360;
		glRotatef(angulo, 0, 1, 0);
	}
	if (com == "fill") {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (com == "line") {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (com == "point") {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
// put drawing instructions here
	

	glBegin(GL_TRIANGLES);
		//t1
		glColor3f(0.5f,0.5f,0.0f);
		glVertex3f(x-1, y-1, z);
		glVertex3f(x, y-1, z);
		glVertex3f(x-0.5, y, z-0.5);
		//t2
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(x, y-1, z);
		glVertex3f(x, y-1, z-1);
		glVertex3f(x-0.5, y, z-0.5);
		//t3
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x, y-1, z-1);
		glVertex3f(x-1,y-1, z-1);
		glVertex3f(x-0.5, y, z-0.5);
		//t4
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x-1, y-1, z-1); //problema aqui no x
		glVertex3f(x-1, y-1, z);
		glVertex3f(x-0.5, y, z-0.5);

	glEnd();
	com = "";
	// End of frame
	glutSwapBuffers();
}



// write function to process keyboard events
void norm(unsigned char key, int x, int y) {
	switch (key) {
		case 'k': 
			com = "aumenta";
			break;
		case 'l':
			com = "diminui";
			break;
		case 'c':
			com = "rEsq";
			break;
		case 'v':
			com = "rDir";
			break;
		case 'a':
			com = "fill";
			break;
		case 's':
			com = "line";
			break;
		case 'd':
			com = "point";
			break;
		default:
			break;
	}
	glutPostRedisplay();
}
void spec(int key_code, int x, int y) {
	switch(key_code){
		case GLUT_KEY_LEFT:
			com = "esquerda";
			break;
		case GLUT_KEY_RIGHT:
			com = "direita";
			break;
		case GLUT_KEY_UP:
			com = "acima";
			break;
		case GLUT_KEY_DOWN:
			com = "abaixo";
			break;
		default:
			break;
	}
	glutPostRedisplay();
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

// put here the registration of the keyboard callbacks
	glutKeyboardFunc(norm);
	glutSpecialFunc(spec);


//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}

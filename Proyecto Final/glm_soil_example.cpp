#include <stdlib.h>
#include <stdio.h>
#include "glut.h"
#include "glm/glm.h"
#include "soil/SOIL.h"
#include <math.h>

struct Limite{
	float left=-2, right=2, bottom = -2.5,top=1.5;
	
	Limite(float a, float b, float c, float d) : left(a),right(b),bottom(c),top(d){

	}
	Limite() {
		
	}

	Limite construir(float a, float b, float c, float d) {
		Limite l;
		l.left = a;
		l.right = b;
		l.bottom = c;
		l.right = d;
		return l;
	}
};

struct Player {
	float x=0, y=0, z = 0;
	Limite lim = lim.construir( 2.0f,-2.0f,2.0f,-2.0f );
	GLMmodel *model;

	void mover(float a, float b) {
		x += a;
		y += b;
		if (x < lim.left) {
			x = lim.left;
		}
		if (x > lim.right) {
			x = lim.right;
		}
		if (y < lim.bottom) {
			y = lim.bottom;
		}
		if (y > lim.top) {
			y = lim.top;
		}
		
	}

	void ajustarLimite(Limite limite) {
		lim = limite;
	}

	void mostrar() {
		glLoadIdentity();
		glTranslatef(x, y, z);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
	}


};


Player player;
Limite limites;

GLuint tex1;
GLfloat ang=0;
GLfloat movx = 0.015;
GLfloat movy = 0.01;
GLMmodel *model;

bool press_a = false;
bool press_w = false;
bool press_s = false;
bool press_d = false;


void Init();
void Display();
void Reshape(int w, int h);
void keyboard ( unsigned char key, int x, int y );

void Init()
{
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	player.ajustarLimite(limites);
	player.model = glmReadOBJ("SpaceShip.obj");
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);

	if (press_a) {
		player.mover(-movx, 0);
	}
	if (press_d) {
		player.mover(movx, 0);
	}
	if (press_w) {
		player.mover(0, movy);
	}
	if (press_s) {
		player.mover(0, -movy);
	}

	player.mostrar();
	
	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard ( unsigned char key, int x, int y ){
	if (key == 'a') {
		press_a = true;
	}
	if (key == 'd') {
		press_d = true;
	}
	if (key == 'w') {
		press_w = true;
	}
	if (key == 's') {
		press_s = true;
	}

	
}

void keyboardup(unsigned char key, int x, int y) {
	if (key == 'a') {
		press_a = false;
	}
	if (key == 'd') {
		press_d = false;
	}
	if (key == 'w') {
		press_w = false;
	}
	if (key == 's') {
		press_s = false;
	}

}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();             
	glOrtho(-2, 2, -2, 2, -10, 10);
	limites = limites.construir(2.0f, -2.0f, 1.5f, -2.5f);
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Proyecto");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutIgnoreKeyRepeat(1);
	Init();
	glutMainLoop();
	return 0;        
	
}
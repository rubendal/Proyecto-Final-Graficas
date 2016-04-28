#include <stdlib.h>
#include <stdio.h>
#include "glut.h"
#include "soil/SOIL.h"
#include "glm/glm.h"

#include <math.h>


GLuint tex1;
GLuint tex2;
GLuint tex3;
GLfloat p1 = 0;
GLfloat p2 = 0;
GLfloat p3 = 0;

struct Limite{
	float left=-2, right=2, bottom = -2,top=2;
	
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
		glRotatef(90, 0, 1, 0);
		glScalef(0.2, 0.2, 0.2);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
	}


};


Player player;
Limite limites;
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
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	player.ajustarLimite(limites);
	player.model = glmReadOBJ("SpaceShip.obj");
	tex1 = SOIL_load_OGL_texture(
		"tex1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO 
	);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);

	glClearDepth(1.0f);

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

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	player.mostrar();
	
	glEnable(GL_TEXTURE_2D);
	//Parallax
	glLoadIdentity();
	
	//glColor3f(1, 0, 0);
	p1 -= 0.001f;
	glBindTexture(GL_TEXTURE_2D, tex1);

	glBegin(GL_QUADS);
	glTexCoord2f(-2.0 + p1, 0);
	glVertex3f(2, 2, 0);

	glTexCoord2f(0 + p1, 0);
	glVertex3f(-2, 2, 0);

	glTexCoord2f(0 + p1, -1.0);
	glVertex3f(-2, -2, 0);

	glTexCoord2f(-2.0 + p1, -1.0);
	glVertex3f(2, -2, 0);
	glEnd();

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
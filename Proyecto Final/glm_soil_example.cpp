#include <stdlib.h>
#include <stdio.h>
#include "glut.h"
#include "soil/SOIL.h"
#include "glm/glm.h"
#include <vector>
#include <math.h>

GLuint tex1;
GLuint tex2;
GLuint tex3;
GLfloat p1 = 0;
GLfloat p2 = 0;
GLfloat p3 = 0;
GLfloat mov_disparos = 0.03;

struct Limite{
	float left=-2, right=2, bottom = -2,top=2;
	
	Limite(float a, float b, float c, float d) : left(a),right(b),bottom(c),top(d){

	}
	Limite() {
		
	}

	struct Limite construir(float a, float b, float c, float d) {
		Limite l;
		l.left = a;
		l.right = b;
		l.bottom = c;
		l.right = d;
		return l;
	}
};

struct Disparo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int daño = 1;

	Disparo(float px, float py) : x(px), y(py){

	}
	void mover(GLfloat m) {
		x += m;
		if (x > 2 + 2) {
			activo = false;
		}
	}

	void mostrar() {
		if (activo) {
			glLoadIdentity();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(x, y, z);
			glScalef(1, 0.3, 0.3);
			glutSolidCube(0.5);
		}
	}

	void calcularColision(struct Enemigo &enemigo) {
		
	}
};

struct Enemigo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int hp = 5;
};

struct Player {
	float x=0, y=0, z = 1;
	struct Limite lim = Limite( -2.0f,2.0f,-2.0f,2.0f );
	GLMmodel *model;
	int hp = 100;
	int municiones = 100;
	std::vector<Disparo> disparos;

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

	void ajustarLimite(struct Limite &limite) {
		lim.bottom = limite.bottom;
		lim.top = limite.top;
		lim.left = limite.left;
		lim.right = limite.right;
	}

	void mostrar() {
		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(x, y, z);
		glRotatef(90, 0, 1, 0);
		glScalef(0.2, 0.2, 0.2);
		//glDisable(GL_TEXTURE_2D);
		//glutSolidCube(1);

		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
		
		for (int i = 0;i < disparos.size();i++) {
			disparos[i].mover(mov_disparos);
			disparos[i].mostrar();
		}
		//glEnable(GL_TEXTURE_2D);

	}

	void disparar() {
		if (municiones > 0) {
			disparos.push_back(Disparo(x,y));
			municiones--;
			printf("Municiones: %d\n", municiones);
		}
	}


};




struct Player player;
GLfloat ang=0;
GLfloat movx = 0.015;
GLfloat movy = 0.01;
GLint shoot_wait = 400;
GLint shoot_time = -1; //ms ultima vez que disparo

bool press_a = false;
bool press_w = false;
bool press_s = false;
bool press_d = false;
bool press_space = false;


void Init();
void Display();
void Reshape(int w, int h);
void keyboard ( unsigned char key, int x, int y );

void Init()
{
	glClearColor(1.0,1.0,1.0,1.0);
	glEnable(GL_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	GLfloat lDiff[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat lSpec[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat lpos[] = { 0.5,8.5,0.0,1 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	
	tex1 = SOIL_load_OGL_texture(
		"tex1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
	);

	tex2 = SOIL_load_OGL_texture(
		"tex2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
	);

	tex3 = SOIL_load_OGL_texture(
		"tex3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_INVERT_Y 
	);


	player.model = glmReadOBJ("SpaceShip.obj");
	glutReportErrors();
}

void Parallax() {
	
	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.4);

	//glShadeModel(GL_SMOOTH);

	GLfloat mDiff[] = { 0.5f,0.5f,0.5f,1.0f };
	GLfloat mSpec[] = { 0.5f,0.5f,0.5f,1.0f };

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mDiff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mSpec);
	//glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glColor3f(1.0, 1.0, 1.0);

	glLoadIdentity();

	p1 -= 0.001f;
	glBindTexture(GL_TEXTURE_2D, tex1);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p1, 0);
	glVertex3f(2, 2, -2);

	glTexCoord2f(0 + p1, 0);
	glVertex3f(-2, 2, -2);

	glTexCoord2f(0 + p1, -1.0);
	glVertex3f(-2, -2, -2);

	glTexCoord2f(-1.0 + p1, -1.0);
	glVertex3f(2, -2, -2);
	glEnd();

	//2
	glLoadIdentity();

	p2 -= 0.0015f;
	glBindTexture(GL_TEXTURE_2D, tex2);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p2, 0);
	glVertex3f(2, 2, -1);

	glTexCoord2f(0 + p2, 0);
	glVertex3f(-2, 2, -1);

	glTexCoord2f(0 + p2, -1.0);
	glVertex3f(-2, -2, -1);

	glTexCoord2f(-1.0 + p2, -1.0);
	glVertex3f(2, -2, -1);
	glEnd();

	//3
	glLoadIdentity();

	p3 -= 0.002f;
	glBindTexture(GL_TEXTURE_2D, tex3);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p3, 0);
	glVertex3f(2, 2, 0);

	glTexCoord2f(0 + p3, 0);
	glVertex3f(-2, 2, 0);

	glTexCoord2f(0 + p3, -1.0);
	glVertex3f(-2, -2, 0);

	glTexCoord2f(-1.0 + p3, -1.0);
	glVertex3f(2, -2, 0);
	glEnd();


	glDisable(GL_ALPHA_TEST);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	Parallax();

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
	if (press_space) {
		GLint time = glutGet(GLUT_ELAPSED_TIME);
		if (time > shoot_time + shoot_wait) {
			//Puede disparar de nuevo
			shoot_time = time;
			player.disparar();

		}
	}


	//glDisable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
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
	if (key == ' ') {
		press_space = true;
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
	if (key == ' ') {
		press_space = false;
	}

}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();             

	glOrtho(-2, 2, -2, 2, -10, 10);
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
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
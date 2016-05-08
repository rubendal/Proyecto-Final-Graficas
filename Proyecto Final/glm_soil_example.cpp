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
GLfloat ammo_length = 4.0f;
GLfloat hp_length = 4.0f;
GLint time = 0;
GLint prev_time = 0;
GLint tiempo_nivel = 30000;
GLint score = 0;
GLMmodel *asteroideModel;

bool colisionan(float x, float y, float r, float x2, float y2, float r2) {
	float dx = x - x2;
	float dy = y - y2;
	float d = sqrt((dx*dx) + (dy*dy));
	return d < r + r2;
}

GLint deltaTime() {
	return time - prev_time;
}

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

struct Limite limites;

struct Powerup {
	float x = 0, y = 0, z = 1;
	int hpup = 0;
	int municionup = 70;
	int appear = 0;
	bool activo = true;
	float r = 0.2;

	Powerup() {

	}

	Powerup(float a, float b, int h, int m, int ms): x(a), y(b), hpup(h), municionup(m), appear(ms) {

	}

	void mover(float n) {
		x -= n*deltaTime();
		if (x < limites.left - 1) {
			activo = false;
		}
	}

	void mostrar() {
		if (activo) {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glLoadIdentity();
			glColor3f(0.0, 1.0, 1.0);
			glTranslatef(x, y, z);
			glutSolidCube(0.3);

		}
	}
};



struct Enemigo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int hp = 5;
	float r = 0.14;
	int appear = 0;
	GLMmodel *model;
	float material[4] = { 1.0f,1.0f,1.0f,1.0f };

	Enemigo() {

	}

	Enemigo(float a, float b, int ms, GLMmodel *m) : x(a), y(b), appear(ms), model(m) {

	}

	void mover(float n) {
		x -= n*deltaTime();
		if (x < limites.left - 1) {
			activo = false;
		}
	}

	void reducirMaterial() {
		material[1] -= 0.2;
		material[2] -= 0.2;
	}
};

struct Asteroide {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int hp = 5;
	float r = 0.2;
	int appear = 0;
	GLMmodel *model;
	float material[4] = {1.0f,1.0f,1.0f,1.0f };
	GLfloat rot = 0;

	Asteroide() {

	}

	Asteroide(float a, float b, int ms,GLMmodel *m, float r2) : x(a), y(b), appear(ms),model(m),r(r2) {

	}

	void mover(float n) {
		if (activo) {
			x -= n*deltaTime();
			if (x < limites.left - 1) {
				activo = false;
			}
		}
	}

	void reducirMaterial() {
		material[1] -= 0.2;
		material[2] -= 0.2;
	}

	void mostrar() {

		glLoadIdentity();
		glEnable(GL_TEXTURE_2D);
		
		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(x, y, z);
		rot += 0.2;
		glRotatef(rot, 1, 1, 1);
		glScalef(r, r, r);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, material);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
	}


};

struct Disparo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int daño = 1;
	float r = 0.5;

	Disparo(float px, float py) : x(px), y(py) {

	}
	void mover(GLfloat m) {
		x += m* deltaTime();
		if (x > limites.right + 1) {
			activo = false;
		}
	}

	void mostrar() {
		if (activo) {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glLoadIdentity();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(x, y, z);
			glScalef(0.5, 0.2, 0.2);
			glutSolidCube(0.5);

		}
	}

	void calcularColision(struct Enemigo &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, r, enemigo.x, enemigo.y, enemigo.r)) {
				enemigo.hp--;
				if (enemigo.hp == 0) {
					enemigo.activo = false;
					score += 3000;
				}
				else {
					enemigo.reducirMaterial();
				}
				activo = false;
			}
		}
	}

	void calcularColision(struct Asteroide &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, r, enemigo.x, enemigo.y, enemigo.r)) {
				enemigo.hp--;
				if (enemigo.hp == 0) {
					enemigo.activo = false;
					score += 2000;
				}
				else {
					enemigo.reducirMaterial();
				}
				activo = false;
			}
		}
	}
};

std::vector<Powerup> powerups;
std::vector<Asteroide> asteroides;
std::vector<Enemigo> enemigos;

struct Player {
	float x=0, y=0, z = 1;
	//struct Limite limites = Limite( -2.0f,2.0f,-2.0f,2.0f );
	GLMmodel *model;
	int hp = 100;
	const int max_hp = 100;
	const int max_municiones = 100;
	int municiones = 100;
	std::vector<Disparo> disparos;
	float r = 1;
	GLfloat rotx = 0;
	GLfloat roty = 0;

	void mover(float a, float b) {
		x += a * deltaTime();
		y += b* deltaTime();
		if (x < limites.left) {
			x = limites.left;
		}
		if (x > limites.right) {
			x = limites.right;
		}
		if (y < limites.bottom) {
			y = limites.bottom;
		}
		if (y > limites.top) {
			y = limites.top;
		}
		
	}

	void mostrar() {
		
		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		for (int i = 0;i < disparos.size();i++) {
			disparos[i].mover(mov_disparos);
			disparos[i].mostrar();
		}
		glEnable(GL_TEXTURE_2D);

		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(x, y, z);
		glRotatef(90, 0, 1, 0);
		glRotatef(rotx, 1, 0, 0);
		glRotatef(roty, 0, 0, 1);
		glScalef(0.1, 0.1, 0.1);
		//glDisable(GL_TEXTURE_2D);
		//glutSolidCube(1);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
	}

	void disparar() {
		if (municiones > 0) {
			disparos.push_back(Disparo(x,y));
			municiones--;
			ammo_length = municiones * (limites.right/max_municiones);
			printf("Ammo length: %f\n", ammo_length);
			
		}
	}

	void calcularColision(struct Enemigo &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, r, enemigo.x, enemigo.y, enemigo.r)) {
				hp -= 25;
				hp_length = hp * (limites.right / max_hp);
				if (hp <= 0) {
					hp = 0;
					exit(1);
				}
			}
		}
	}

	void calcularColision(struct Asteroide &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, r, enemigo.x, enemigo.y, enemigo.r)) {
				if (enemigo.activo) {
					//exit(1); //Muere el jugador
					hp -= 25;
					hp_length = hp * (limites.right / max_hp);
					if (hp <= 0) {
						hp = 0;
						exit(1);
					}
					enemigo.activo = false;
				}
			}
		}
	}

	void calcularColision(struct Powerup &powerup) {
		if (powerup.activo) {
			if (colisionan(x, y, r, powerup.x, powerup.y, powerup.r)) {
				powerup.activo = false;
				municiones += powerup.municionup;
				if (municiones > max_municiones) {
					municiones = max_municiones;
				}
				ammo_length = municiones * (limites.right / max_municiones);
				hp += powerup.hpup;
				if (hp > max_hp) {
					hp = max_hp;
				}
				hp_length = hp * (limites.right / max_hp);
				score += 100;
				printf("Score: %d\n", score);
				printf("Powerup obtenido\n");
			}else{
			
			}
		}
	}


};


struct Player player;

GLfloat ang=0;
GLfloat movx = 0.015;
GLfloat movy = 0.01;
GLint shoot_wait = 100;
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
	GLfloat lpos[] = { 0.0,2.5,0.0,1 };

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

	player.model = glmReadOBJ("ship.obj");
	asteroideModel = glmReadOBJ("Asteroid.obj");

	time = glutGet(GLUT_ELAPSED_TIME);

	powerups.push_back(Powerup(4, 1, 0, 70, time + 9000));

	asteroides.push_back(Asteroide(4, 1, time + 4000,asteroideModel,0.3));
	asteroides.push_back(Asteroide(4, -0.4, time + 13000, asteroideModel,0.4));
	asteroides.push_back(Asteroide(4, 0, time + 15000, asteroideModel, 0.2));
	asteroides.push_back(Asteroide(4, -2, time + 18000, asteroideModel, 0.2));

	ammo_length = limites.right * 2;
	hp_length = limites.right * 2;

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
	glAlphaFunc(GL_NOTEQUAL, 0);

	glDisable(GL_LIGHTING);

	glColor3f(1.0, 1.0, 1.0);

	glLoadIdentity();

	p1 -= 0.001f* deltaTime();
	glBindTexture(GL_TEXTURE_2D, tex1);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p1, 0);
	glVertex3f(limites.right, limites.top, -2);

	glTexCoord2f(0 + p1, 0);
	glVertex3f(limites.left, limites.top, -2);

	glTexCoord2f(0 + p1, -1.0);
	glVertex3f(limites.left, limites.bottom, -2);

	glTexCoord2f(-1.0 + p1, -1.0);
	glVertex3f(limites.right, limites.bottom, -2);
	glEnd();

	//2
	glLoadIdentity();

	p2 -= 0.0015f * deltaTime();
	glBindTexture(GL_TEXTURE_2D, tex2);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p2, 0);
	glVertex3f(limites.right, limites.top, -2);

	glTexCoord2f(0 + p2, 0);
	glVertex3f(limites.left, limites.top, -2);

	glTexCoord2f(0 + p2, -1.0);
	glVertex3f(limites.left, limites.bottom, -2);

	glTexCoord2f(-1.0 + p2, -1.0);
	glVertex3f(limites.right, limites.bottom, -2);
	glEnd();

	//3
	glLoadIdentity();

	p3 -= 0.002f* deltaTime();
	glBindTexture(GL_TEXTURE_2D, tex3);

	glBegin(GL_QUADS);
	glTexCoord2f(-1.0 + p3, 0);
	glVertex3f(limites.right, limites.top, -2);

	glTexCoord2f(0 + p3, 0);
	glVertex3f(limites.left, limites.top, -2);

	glTexCoord2f(0 + p3, -1.0);
	glVertex3f(limites.left, limites.bottom, -2);

	glTexCoord2f(-1.0 + p3, -1.0);
	glVertex3f(limites.right, limites.bottom, -2);
	glEnd();


	glDisable(GL_ALPHA_TEST);
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	prev_time = time;
	time = glutGet(GLUT_ELAPSED_TIME);

	glEnable(GL_TEXTURE_2D);
	Parallax();

	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glColor3f(0, 0, 1.0);

	glVertex3f(0,limites.top - 0.05f,2);
	glVertex3f(0,limites.top - (limites.top / 10.0f),2);
	glVertex3f(0+ammo_length, limites.top - (limites.top / 10.0f), 2);
	glVertex3f(0+ammo_length, limites.top - 0.05f, 2);

	glEnd();

	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_QUADS);
	glColor3f(1.00, 0, 0);

	glVertex3f(0, limites.bottom - 0.05f, 2);
	glVertex3f(0, limites.bottom - (limites.bottom / 10.0f), 2);
	glVertex3f(0 + hp_length, limites.bottom - (limites.bottom / 10.0f), 2);
	glVertex3f(0 + hp_length, limites.bottom - 0.05f, 2);

	glEnd();

	glEnable(GL_LIGHTING);



	for (int i = 0;i < powerups.size();i++) {
		if (powerups[i].activo) {
			if (time > powerups[i].appear) {
				powerups[i].mover(0.0003);
				powerups[i].mostrar();
				player.calcularColision(powerups[i]);
			}
		}
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (int i = 0;i < asteroides.size();i++) {
		if (asteroides[i].activo) {
			if (time > asteroides[i].appear) {
				asteroides[i].mover(0.0003);
				asteroides[i].mostrar();
				player.calcularColision(asteroides[i]);
				for (int n = 0;n < player.disparos.size();n++) {
					if (player.disparos[n].activo) {
						player.disparos[n].calcularColision(asteroides[i]);
					}
				}
			}
		}
	}
	GLfloat lDiff2[] = { 1.0f,1.0f,1.0f,1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiff2);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	GLfloat mat[] = { 1.0f,1.0f,1.0f,1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);


	player.rotx = 0;
	player.roty = 0;


	if (press_a) {
		player.rotx = -15;
		player.mover(-movx, 0);
	}
	if (press_d) {
		player.roty += 15;
		player.mover(movx, 0);
	}
	if (press_w) {
		player.roty = -25;
		player.mover(0, movy);
	}
	if (press_s) {
		player.roty += 25;
		player.mover(0, -movy);
	}
	if (press_space) {
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
	if (key == 'a' || key == 'A') {
		press_a = true;
	}
	if (key == 'd' || key == 'D') {
		press_d = true;
	}
	if (key == 'w' || key == 'W') {
		press_w = true;
	}
	if (key == 's' || key == 'S') {
		press_s = true;
	}
	if (key == ' ') {
		press_space = true;
	}

	
}

void keyboardup(unsigned char key, int x, int y) {
	if (key == 'a' || key == 'A') {
		press_a = false;
	}
	if (key == 'd' || key == 'D') {
		press_d = false;
	}
	if (key == 'w' || key == 'W') {
		press_w = false;
	}
	if (key == 's' || key == 'S') {
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
	limites = Limite(-4.0f, 4.0f, -4.0f, 4.0f);
	glOrtho(limites.left, limites.right, limites.bottom, limites.top, -10, 10);
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
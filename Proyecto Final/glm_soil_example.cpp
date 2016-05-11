#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "glut.h"
#include "soil/SOIL.h"
#include "glm/glm.h"
#include <vector>
#include <math.h>
#include <string>

GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint tex4;
GLuint tex5;
GLuint tex6;
GLfloat p1 = 0;
GLfloat p2 = 0;
GLfloat p3 = 0;
GLfloat mov_disparos = 0.03;
GLfloat ammo_length = 4.0f;
GLfloat hp_length = 4.0f;
GLint tiempo = 0;
GLint prev_time = 0;
GLint tiempo_nivel = 37000;
GLint score = 0;
GLint time_begin = 0;
GLMmodel *asteroideModel;
GLMmodel *enemigoModel;
bool noPerdido = true;
bool pantInit = true;

bool colisionan(float x, float y, float r, float x2, float y2, float r2) {
	float dx = x - x2;
	float dy = y - y2;
	float d = sqrt((dx*dx) + (dy*dy));
	return d < r + r2;
}

bool colisionan(float x, float y, float rx, float ry, float x2, float y2, float rx2, float ry2) {
	float dx = x - x2;
	float dy = y - y2;
	float d = sqrt((dx*dx) + (dy*dy));
	return d < rx + rx2 || d < ry + ry2;
}

float random(int min, int max) {
	float x = (float)rand() / (float)(RAND_MAX);
	return min + (x * (max-min));
}

int randomTime(int max) {
	return ((rand() * 1000) + 2000) % max;
}

GLint deltaTime() {
	return tiempo - prev_time;
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
	float r = 0.3;

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
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glLoadIdentity();
			float m = municionup > 0 ? 1.0 : 0;
			float h = hpup > 0 ? 1.0 : 0;
			glColor3f(h, 0, m);
			glTranslatef(x, y, z);
			glutSolidCube(r);

		}
	}
};

struct DisparoEnemigo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int daño = 10;
	float rx = 0.1;
	float ry = 0.05;

	DisparoEnemigo(float px, float py) : x(px), y(py) {

	}

	void mover(GLfloat m) {
		x -= m*deltaTime();
		if (x < limites.left - 1) {
			activo = false;
		}
	}

	void mostrar() {
		if (activo) {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glLoadIdentity();
			glColor3f(1.0, 0.0, 0.0);
			glTranslatef(x, y, z);
			glScalef(0.5, 0.2, 0.2);
			glutSolidCube(0.5);
		}
	}
};

struct Enemigo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int hp = 5;
	float rx = 0.2;
	float ry = 0.4;
	int appear = 0;
	GLMmodel *model;
	float material[4] = { 1.0f,1.0f,1.0f,1.0f };
	std::vector<DisparoEnemigo> disparos;
	int tiempo_disparo = 900;
	int ultimo_disparo = 0;

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

	void mostrar() {

		glLoadIdentity();
		glDisable(GL_TEXTURE_2D);
		for (int i = 0; i < disparos.size(); i++) {
			disparos[i].mover(mov_disparos);
			disparos[i].mostrar();
		}
		glEnable(GL_TEXTURE_2D);

		glLoadIdentity();
		glColor3f(1.0, 1.0, 1.0);
		glTranslatef(x, y, z);
		//rot += 0.2;
		glRotatef(90, 0, -1, 0);
		//glScalef(r, r, r);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, material);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, material);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, material);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
	}

	void disparar(int time) {
		if (time >= ultimo_disparo + tiempo_disparo) {
			disparos.push_back(DisparoEnemigo(x, y));
			ultimo_disparo = time;
		}
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
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, material);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, material);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, material);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
	}


};

struct Disparo {
	float x = 0, y = 0, z = 1;
	bool activo = true;
	int daño = 1;
	float rx = 0.4;
	float ry = 0.2;

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
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			glLoadIdentity();
			glColor3f(0.0, 1.0, 0.0);
			glTranslatef(x, y, z);
			glScalef(0.5, 0.2, 0.2);
			glutSolidCube(0.5);

		}
	}

	void calcularColision(struct Enemigo &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, rx, ry, enemigo.x, enemigo.y, enemigo.rx, enemigo.ry)) {
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
			if (colisionan(x, y, rx, ry, enemigo.x, enemigo.y, enemigo.r, enemigo.r)) {
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
	float rx = 0.6;
	float ry = 0.4;
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
		//glColor3f(1.0, 0, 0);
		//glutSolidSphere(rx*10,10,10);
		//glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		glmDraw(model, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
		glLoadIdentity();
	}

	void disparar() {
		if (municiones > 0) {
			disparos.push_back(Disparo(x,y));
			municiones--;
			ammo_length = municiones * (limites.right/max_municiones);
			//printf("Ammo length: %f\n", ammo_length);
			
		}
	}

	void calcularColision(struct Enemigo &enemigo) {

		if (enemigo.activo) {
			if (colisionan(x, y, rx, ry, enemigo.x, enemigo.y, enemigo.rx, enemigo.ry)) {
				if (enemigo.activo) {
					
					hp -= 25;
					hp_length = hp * (limites.right / max_hp);
					if (hp <= 0) {
						hp = 0;
						//exit(1);
						noPerdido = false;
					}
					enemigo.activo = false;
				}
			}
		}

	}

	void calcularColision(struct Asteroide &enemigo) {
		if (enemigo.activo) {
			if (colisionan(x, y, rx, ry, enemigo.x, enemigo.y, enemigo.r,enemigo.r)) {
				if (enemigo.activo) {
					//exit(1);
					noPerdido = false;
				}
			}
		}
	}

	void calcularColision(struct Powerup &powerup) {
		if (powerup.activo) {
			if (colisionan(x, y, rx, ry, powerup.x, powerup.y, powerup.r, powerup.r)) {
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
				//printf("Score: %d\n", score);
				//printf("Powerup obtenido\n");
			}else{
			
			}
		}
	}

	void calcularColision(struct DisparoEnemigo &enemigo) {
		if (colisionan(x, y, rx,ry, enemigo.x, enemigo.y, enemigo.rx,enemigo.ry)) {
			hp -= enemigo.daño;
			hp_length = hp * (limites.right / max_hp);
			if (hp <= 0) {
				hp = 0;
				//exit(1);
				noPerdido = false;
			}
			enemigo.activo = false;
		}

	}


};


struct Player player;

GLfloat ang=0;
GLfloat movx = 0.015;
GLfloat movy = 0.01;
GLint shoot_wait = 100;
GLint time_fin = 0;
GLint shoot_time = -1; //ms ultima vez que disparo

bool press_a = false;
bool press_w = false;
bool press_s = false;
bool press_d = false;
bool press_space = false;
bool press_enter = false;


void Init();
void Display();
void Reshape(int w, int h);
void keyboard ( unsigned char key, int x, int y );

void pantallas(GLuint texx) {
	glEnable(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, texx);
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex3f(limites.right, limites.top, -1);

	glTexCoord2f(0, 0);
	glVertex3f(limites.left, limites.top, -1);

	glTexCoord2f(0, 1.0);
	glVertex3f(limites.left, limites.bottom, -1);

	glTexCoord2f(1, 1.0);
	glVertex3f(limites.right, limites.bottom, -1);
	glEnd();
}

void Init()
{
	glClearColor(1.0,1.0,1.0,1.0);
	glEnable(GL_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHTING);

	GLfloat lDiff[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat lSpec[] = { 0.0f,0.0f,1.0f,1.0f };
	GLfloat lpos[] = { 0.0,2.5,0.0,1 };

	GLfloat lDiff1[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat lSpec1[] = { 1.0f,0.0f,0.0f,1.0f };
	GLfloat lpos1[] = {0.0f,-2.5,1.0,1 };

	GLfloat lDiff2[] = { 1.0f,1.0f,1.0f,1.0f };
	GLfloat lSpec2[] = { 1.0f,0.0f,1.0f,1.0f };
	GLfloat lpos2[] = { 1.5f,0.0,1.0,1 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lSpec);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	glLightfv(GL_LIGHT1, GL_DIFFUSE, lDiff1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lSpec1);
	glLightfv(GL_LIGHT1, GL_POSITION, lpos1);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, lDiff2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lSpec2);
	glLightfv(GL_LIGHT2, GL_POSITION, lpos2);
	

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

	tex4 = SOIL_load_OGL_texture(
		"pantalla2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
		);

	tex5 = SOIL_load_OGL_texture(
		"pantalla1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
		);

	tex6 = SOIL_load_OGL_texture(
		"pantalla3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
		);

	player.model = glmReadOBJ("ship.obj");
	asteroideModel = glmReadOBJ("Asteroid.obj");
	enemigoModel = glmReadOBJ("rtm_metroid_v2.obj");

	time_begin = glutGet(GLUT_ELAPSED_TIME);

	srand(time(NULL));

	powerups.push_back(Powerup(limites.right +3, random(limites.bottom,limites.top), 0, 70, time_begin + randomTime(tiempo_nivel - 5000)));
	powerups.push_back(Powerup(limites.right +3, random(limites.bottom, limites.top), 50, 0, time_begin + randomTime(tiempo_nivel - 5000)));

	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000),asteroideModel,0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), asteroideModel,0.4));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), asteroideModel, 0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), asteroideModel, 0.5));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), asteroideModel, 0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), asteroideModel, 0.4));

	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel-5000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), time_begin + randomTime(tiempo_nivel - 5000), enemigoModel));

	ammo_length = limites.right * 2;
	hp_length = limites.right * 2;

	time_fin = time_begin + tiempo_nivel;

	player.x = limites.left/2;
	player.y = 0;
	player.z = 1;

	glutReportErrors();
}

void Reiniciar() {
	enemigos.clear();
	powerups.clear();
	asteroides.clear();
	score = 0;
	Init();
}

void NuevoNivel() {
	int prev_nivel = tiempo_nivel;
	tiempo_nivel += 20000;


	for (int n = 0;n < enemigos.size();n++) {
		enemigos[n].activo = true;
		enemigos[n].x = limites.right+1;
		enemigos[n].hp = 5;
		enemigos[n].material[0] = 1.0f;
		enemigos[n].material[1] = 1.0f;
		enemigos[n].material[2] = 1.0f;
		enemigos[n].material[3] = 1.0f;
		enemigos[n].disparos.clear();
		enemigos[n].ultimo_disparo = 0;
		enemigos[n].tiempo_disparo -= 20;
	}
	for (int n = 0;n < asteroides.size();n++) {
		asteroides[n].activo = true;
		asteroides[n].x = limites.right + 0.5;
		asteroides[n].hp = 5;
		asteroides[n].material[0] = 1.0f;
		asteroides[n].material[1] = 1.0f;
		asteroides[n].material[2] = 1.0f;
		asteroides[n].material[3] = 1.0f;
	}
	for (int n = 0;n < powerups.size();n++) {
		powerups[n].activo = true;
		powerups[n].x = limites.right + 0.5;
	}

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

	tex4 = SOIL_load_OGL_texture(
		"pantalla2.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
	);

	tex5 = SOIL_load_OGL_texture(
		"pantalla1.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
	);

	tex6 = SOIL_load_OGL_texture(
		"pantalla3.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_POWER_OF_TWO
	);

	srand(time(NULL));

	powerups.push_back(Powerup(limites.right +3, random(limites.bottom, limites.top), 0, 70, prev_nivel + randomTime(15000)));
	powerups.push_back(Powerup(limites.right +3, random(limites.bottom, limites.top), 50, 0, prev_nivel + randomTime(15000)));

	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.4));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.2));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.2));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.5));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.3));
	asteroides.push_back(Asteroide(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), asteroideModel, 0.4));

	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), enemigoModel));
	enemigos.push_back(Enemigo(limites.right +3, random(limites.bottom, limites.top), prev_nivel + randomTime(15000), enemigoModel));
	
	player.hp = 100;
	player.municiones = 100;

	ammo_length = limites.right * 2;
	hp_length = limites.right * 2;

	time_begin = glutGet(GLUT_ELAPSED_TIME);
	time_fin = time_begin + tiempo_nivel;

	player.x = limites.left / 2;
	player.y = 0;
	player.z = 1;
	

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

	p1 += 0.0004f* deltaTime();
	glBindTexture(GL_TEXTURE_2D, tex1);

	glBegin(GL_QUADS);
	glTexCoord2f(0.5 + p1, 0);
	glVertex3f(limites.right, limites.top, -3);

	glTexCoord2f(0 + p1, 0);
	glVertex3f(limites.left, limites.top, -3);

	glTexCoord2f(0 + p1, 1.0);
	glVertex3f(limites.left, limites.bottom, -3);

	glTexCoord2f(0.5 + p1, 1.0);
	glVertex3f(limites.right, limites.bottom, -3);
	glEnd();

	//2
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glLoadIdentity();

	p2 += 0.00006f * deltaTime();
	glBindTexture(GL_TEXTURE_2D, tex2);

	glBegin(GL_QUADS);
	glTexCoord2f(0.5 + p2, 0);
	glVertex3f(limites.right, limites.top, -2);

	glTexCoord2f(0 + p2, 0);
	glVertex3f(limites.left, limites.top, -2);

	glTexCoord2f(0 + p2, 1.0);
	glVertex3f(limites.left, limites.bottom, -2);

	glTexCoord2f(0.5 + p2, 1.0);
	glVertex3f(limites.right, limites.bottom, -2);
	glEnd();

	//3
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glLoadIdentity();

	p3 += 0.00002f * deltaTime();

	glBindTexture(GL_TEXTURE_2D, tex3);

	glBegin(GL_QUADS);
	glTexCoord2f(0.5 + p3, 0);
	glVertex3f(limites.right, limites.top, -1);

	glTexCoord2f(0 + p3, 0);
	glVertex3f(limites.left, limites.top, -1);

	glTexCoord2f(0 + p3, 1.0);
	glVertex3f(limites.left, limites.bottom, -1);

	glTexCoord2f(0.5 + p3, 1.0);
	glVertex3f(limites.right, limites.bottom, -1);
	glEnd();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glDisable(GL_ALPHA_TEST);
}

GLfloat vel_asteroide = 0.0007;

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	prev_time = tiempo;
	tiempo = glutGet(GLUT_ELAPSED_TIME);

	if (pantInit) {

		pantallas(tex6);

	} else if (tiempo >= time_fin && noPerdido) {

		pantallas(tex4);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor3f(1.00, 0, 0);
		glRasterPos3f(limites.left + 0.25f, limites.top - 0.375f, 4);
		std::string scoreString = "Score: " + std::to_string(score);
		char *scoreString2 = new char[scoreString.length() + 1];
		strcpy(scoreString2, scoreString.c_str());
		for (int i = 0; scoreString2[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scoreString2[i]);
		}
		delete[] scoreString2;
		if (press_enter) {
			NuevoNivel();
		}

	}
	else if (noPerdido){

		if (tiempo  >= time_fin / 2) {
			vel_asteroide = 0.001;
		}
		else if (tiempo >= time_fin * 7 / 10 && time_fin < 50000) {
			vel_asteroide = 0.003;
		}
		else if (tiempo >= time_fin * 7 / 10 && time_fin >= 50000) {
			vel_asteroide = 0.005;
		}

		glEnable(GL_TEXTURE_2D);
		Parallax();

		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);
		glColor3f(0, 0, 1.0);

		glVertex3f(0, limites.top - 0.05f, 2);
		glVertex3f(0, limites.top - (limites.top / 10.0f), 2);
		glVertex3f(0 + ammo_length, limites.top - (limites.top / 10.0f), 2);
		glVertex3f(0 + ammo_length, limites.top - 0.05f, 2);

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

		glRasterPos3f(limites.left + 0.25f, limites.top - 0.375f, 3);
		std::string scoreString = "Score: " + std::to_string(score);
		char *scoreString2 = new char[scoreString.length()+1];
		strcpy(scoreString2, scoreString.c_str());
		for (int i = 0; scoreString2[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scoreString2[i]);
		}
		delete[] scoreString2;
		glEnable(GL_LIGHTING);



		for (int i = 0;i < powerups.size();i++) {
			if (powerups[i].activo) {
				if (tiempo - time_begin > powerups[i].appear) {
					powerups[i].mover(0.0006);
					powerups[i].mostrar();
					player.calcularColision(powerups[i]);
				}
			}
		}

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		for (int i = 0;i < asteroides.size();i++) {
			if (asteroides[i].activo) {
				if (tiempo - time_begin > asteroides[i].appear) {
					asteroides[i].mover(vel_asteroide);
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

		for (int i = 0; i < enemigos.size(); i++) {
			if (enemigos[i].activo) {
				if (tiempo - time_begin > enemigos[i].appear) {
					enemigos[i].mover(vel_asteroide);
					enemigos[i].mostrar();
					enemigos[i].disparar(tiempo - time_begin);
					player.calcularColision(enemigos[i]);
					for (int n = 0; n < player.disparos.size(); n++) {
						if (player.disparos[n].activo) {
							player.disparos[n].calcularColision(enemigos[i]);
						}
					}
					for (int n = 0; n < enemigos[i].disparos.size(); n++) {
						if (enemigos[i].disparos[n].activo) {
							player.calcularColision(enemigos[i].disparos[n]);
						}
					}
				}
			}
		}

		GLfloat lDiff2[] = { 1.0f,1.0f,1.0f,1.0f };
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lDiff2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lDiff2);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, lDiff2);
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
			if (tiempo > shoot_time + shoot_wait) {
				//Puede disparar de nuevo
				shoot_time = tiempo;
				player.disparar();

			}

		}


		//glDisable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, 0);
		player.mostrar();

	} else {
		glColor3f(1.00, 0, 0);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);

		glRasterPos3f(limites.left + 0.25f, limites.top - 0.375f, 3);
		std::string scoreString = "Score: " + std::to_string(score);
		char *scoreString2 = new char[scoreString.length() + 1];
		strcpy(scoreString2, scoreString.c_str());
		for (int i = 0; scoreString2[i] != '\0'; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, scoreString2[i]);
		}
		delete[] scoreString2;
		pantallas(tex5);
		if (press_enter) {
			Reiniciar();
			noPerdido = true;
		}
	}
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
	if (key == '\r' || key == '\n') {
		press_enter = true;
		pantInit = false;
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
	if (key == '\r' || key == '\n') {
		press_enter = false;
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
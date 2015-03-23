#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "terrain.cpp"
#include "lattu.cpp"
#include "target.cpp"
#include "imageloader.h"
#include "vec3f.h"
using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)

Lattu lt;
Target tr;

Vec3f g(0.0, -1.0, 0.0);
float dott;
Vec3f gt;

int sp = 1;

//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	delete image;
	t->computeNormals();
	return t;
}

float ang = 0.0f;
float ang2 = -30.0f;
float ang3 =  0.0f;
float ang4 = 0.0f;
Terrain* ter;

void cleanTerrain() {
	delete ter;
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanTerrain();
			exit(0);
		case 'a':
			ang2 = -30.0;
			ang += 1.0f;
			if (ang > 360)
				ang -= 360;
			break;
		case 'd':
			ang2 = -30.0;
			ang-=1.0f;
			if(ang < 0)
				ang +=360;
			break;
		case 'w':
			ang2 = -90.0f;
			break;
		case 's':
			ang2 = 90.0f;
			break;
		case 'x':
			if(ang3!=-90)
				ang3-=5;
			break;
		case 'z':
			if(ang3!=90)
				ang3 += 5;
			break;
		case ' ':
			lt.speed = sp*0.1;
			lt.latvz = lt.speed*cos(DEG2RAD(-ang3));
			lt.latvx = lt.speed*sin(DEG2RAD(-ang3));
			break;
		case 'r':
			tr.tarx = rand()%128;
			tr.tarz = rand()%128;
			if(tr.tarz >= 64)
				tr.tarz -= 50;
			break;
		default:
			break;
	}
}

void speedControl(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
	{
		if(sp!=10)
			sp++;
	}
	else if(key == GLUT_KEY_DOWN)
	{
		if(sp!=0)
			sp--;
	}
	else if(key == GLUT_KEY_LEFT)
	{
		if(lt.latx!=0.0)
			lt.latx-=1.0;
	}
	else if(key == GLUT_KEY_RIGHT)
	{
		if(lt.latx!=127.0)
			lt.latx+=1.0;
	}
}

void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

int checkEnvironmentCollision() {
	if(pow((lt.latx - tr.tarx), 2) < 0.001 && pow((lt.latz - tr.tarz), 2) < 0.001)
		return 1;
	return 0;
}

int checkEnvironmentBoundary() {
	if(lt.latx >= 127 ||  lt.latz <= 0 || lt.latx <=0)
		return 1;
	return 0;
}

void drawTarget() {
	glPushMatrix();
	glTranslatef(tr.tarx, ter->getHeight(tr.tarx, tr.tarz)+tr.tarr1+1, tr.tarz);
	glColor3f(1.0, 0.0, 0.0);
	glutSolidTorus(1, tr.tarr2, 10, 10);
	glColor3f(1.0, 1.0, 1.0);
	glutSolidTorus(1, tr.tarr1, 10, 10);
	glColor3f(0.0, 0.0, 1.0);
	glutSolidSphere(tr.tarr1-1, 32, 32);
	glPopMatrix();
}

void drawArrow() {
	glPushMatrix();
	glColor3f(100.0/256.0, 201.0/256.0, 235.0/256.0);
	GLUquadricObj *quadratic3;
	quadratic3 = gluNewQuadric();
	glTranslatef(-10.0, 20.0, 127.0 - 20.0);
	glTranslatef(0.0, 0.0, 20.0);
	glRotatef(ang3, 0.0, 1.0, 0.0);
	glTranslatef(0.0, 0.0, -20.0);
	gluCylinder(quadratic3, 1.0, 1.0, 15.0, 32, 32);
	glPushMatrix();
	glRotatef(45.0, 0.0, 1.0, 0.0);
	gluCylinder(quadratic3, 1.0, 1.0, 5.0, 32, 32);
	glPopMatrix();
	glPushMatrix();
	glRotatef(-45.0, 0.0, 1.0, 0.0);
	gluCylinder(quadratic3, 1.0, 1.0, 5.0, 32, 32);
	glPopMatrix();
	glPopMatrix();
}

void drawCube(float z, int i) {
	glPushMatrix();
	glTranslatef(135.0, 5.0, z-4.0 );
	glColor3f((i*28.4)/256.0, ((9-i)*28.4)/256.0, 0.0);
	glutSolidSphere(2.0, 32, 32);
	glPopMatrix();
}

void drawTop(Vec3f nor) {
	glPushMatrix();
	//lower tip
	glTranslatef(lt.latx, ter->getHeight(lt.latx,lt.latz)+lt.lath2, lt.latz);
	glRotatef(90.0, nor[0], nor[1], nor[2]);
	glRotatef(90.0f, 1.0, 0.0, 0.0);
	glRotatef(ang4, 0.0, 0.0, -1.0);
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	glColor3f(188/256.0, 97/256.0, 37/256.0);
	gluCylinder(quadratic, lt.latn2, lt.latn1, lt.lath2, 32, 32);
	//middle tip
	glTranslatef(0.0, 0.0, -lt.lath1);
	glColor3f(148.0/256.0, 100.0/256.0, 34.0/256.0);
	GLUquadricObj *quadratic2;
	quadratic2 = gluNewQuadric();
	gluCylinder(quadratic2, lt.latr1, lt.latn2, lt.lath1, 32, 32);
	//upper torus
	glColor3f(187.0/256.0, 123.0/256.0, 21.0/256.0);
	glutSolidTorus(lt.latr2, lt.latr1, 10, 10);
	glPopMatrix();
}

void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -7.8f);
	glRotatef(-ang2, 1.0f, 0.0f, 0.0f);
	glRotatef(-ang, 0.0f, 1.0f, 0.0f);

	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {0.6f, 1.0f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	float scale = 6.0f / max(ter->width() - 1, ter->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(ter->width() - 1) / 2,
			0.0f,
			-(float)(ter->length() - 1) / 2);

	glColor3f(122.0/244, 111.0/244, 211.0/244);
	for(int z = 0; z < ter->length() - 1; z++)
	{
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < ter->width(); x++) {
			Vec3f normal = ter->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, ter->getHeight(x, z), z);
			normal = ter->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, ter->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}
	Vec3f nor = ter->getNormal(lt.latx, lt.latz);
	dott = nor.dot(g);
	printf("%d\n", lt.score);
	gt = g + nor*dott;
	drawTop(nor);
	for (int i = 0; i < sp; ++i)
	{
		drawCube(127.0 - i*10.0, i);
	}
	drawTarget();

	glutSwapBuffers();
}

void update(int value) {

	glutPostRedisplay();
	ang4+=10;

	if(checkEnvironmentCollision())
	{
		lt.score++;
		lt.latx = 64.0f;
		lt.latz = 127.0f;
		lt.latvx = 0.0f;
		lt.latvz = 0.0f;
		tr.tarx = rand()%128;
		tr.tarz = rand()%128;
		if(tr.tarz >= 64)
			tr.tarz -= 50;
	}
	if(checkEnvironmentBoundary())
	{
		lt.latx = 64.0f;
		lt.latz = 127.0f;
		lt.latvx = 0.0f;
		lt.latvz = 0.0f;
		tr.tarx = rand()%128;
		tr.tarz = rand()%128;
		if(tr.tarz >= 64)
			tr.tarz -= 50;
	}
	lt.latx+=lt.latvx;
	lt.latz-=lt.latvz;
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	int windowWidth = glutGet(GLUT_SCREEN_WIDTH);
	int windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(0, 0);

	glutCreateWindow("A Game of Lattu");
	initRendering();
	tr.tarx = rand()%128;
	tr.tarz = rand()%128;
	if(tr.tarz >= 64)
		tr.tarz -= 50;
	ter = loadTerrain("terrainImage.bmp", 20);
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(speedControl);
	glutReshapeFunc(handleResize);

	glutTimerFunc(25, update, 0);

	glutMainLoop();
	return 0;
}

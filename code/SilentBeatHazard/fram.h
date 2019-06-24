
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <iostream>

#include <sys/types.h>
#include <GL/glut.h>
#include <vector>
#include "sprites.h"

Extent world = { -1, 1, -1, 1 };
GLint viewport[4];

BackGround* backGround;
FriendLink* friendLink;
EneLink* eneLink;
RockLink* rockLink;
FriendFireLink* friendFireLink;
EneFireLink* eneFireLink;
SpritesLink* panelLink;
int* scOrE;
SpritesLink* supplyLink;
GameProgram* prog;


const float dt = 1;
const int dtMs = 20;

//puase
void displaySpare();
void timerFuncSpare(int value);
void mouse_motionSpare(int x, int y);
void myMouseSpare(int button, int state, int x, int y);
void onKeyPressSpare(unsigned char key, int x, int y);
void onKeyReleaseSpare(unsigned char key, int x, int y);
void specialKeyPressSpare(int key, int x, int y);


//gaming
void display()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	backGround->display();
	rockLink->display();
	eneLink->display();
	eneFireLink->display();
	friendFireLink->display();
	supplyLink->display();
	friendLink->display();
	panelLink->display();

	glFlush();
	glutSwapBuffers();
}

void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	viewport[2] = w;
	viewport[3] = h;
}

void myMouse(int button, int state, int x, int y)
{
	//left hold
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN)
	{
		friendFireLink->mousePressed = true;
	}
	else if (button == GLUT_LEFT_BUTTON&&state == GLUT_UP)
	{
		friendFireLink->mousePressed = false;
	}

	//right click
	if (button == GLUT_RIGHT_BUTTON&&state == GLUT_DOWN)
	{
		friendFireLink->mouseRightPressed = true;
		//friendFireLink->mouseRightReleased = false;
	}
	else if (button == GLUT_RIGHT_BUTTON&&state == GLUT_UP)
	{
		friendFireLink->mouseRightPressed = false;
		friendFireLink->mouseRightReleased = true;
	}

}

void mouse_motion(int x, int y)
{

	friendLink->head->sprite->mousePos[X] = (float)x / viewport[2] * (world.r - world.l) + world.l;
	friendLink->head->sprite->mousePos[Y] = world.t - (float)y / viewport[3] * (world.t - world.b);
	//Vec2fPrint(mousePos);
}

void onKeyPress(unsigned char key, int x, int y)
{

	printf("keyin %c\n", key);
	if (key == 'w')
		friendLink->head->sprite->acc[Y] = 0.004;
	if (key == 's')
		friendLink->head->sprite->acc[Y] = -0.004;
	if (key == 'd')
		friendLink->head->sprite->acc[X] = 0.004;
	if (key == 'a')
		friendLink->head->sprite->acc[X] = -0.004;

	//glutPostRedisplay();
}
void onKeyRelease(unsigned char key, int x, int y)
{

	printf("keyup %c\n", key);
	if (key == 'w')
		friendLink->head->sprite->acc[Y] = 0.;
	if (key == 's')
		friendLink->head->sprite->acc[Y] = 0.;
	if (key == 'd')
		friendLink->head->sprite->acc[X] = 0.;
	if (key == 'a')
		friendLink->head->sprite->acc[X] = 0.;


	//glutPostRedisplay();
}

void specialKeyPress(int key, int x, int y)
{
	printf("keyin %c\n", key);
	if (key == GLUT_KEY_UP)
	{
		//spritesList[1]->acc[Y] = 0.01;
		printf("keyspecial\n");
	}

}

void myTimerFunc(int value)
{
	if (friendLink->head->sprite->isDead && prog->state == 0)
	{
		glutDisplayFunc(displaySpare);
		glutPostRedisplay();
		glutMouseFunc(myMouseSpare);
		glutMotionFunc(mouse_motionSpare);
		glutPassiveMotionFunc(mouse_motionSpare);
		glutKeyboardFunc(onKeyPressSpare);
		glutKeyboardUpFunc(onKeyReleaseSpare);
		//glutSpecialFunc(specialKeyPressSpare);
		glutTimerFunc(20, timerFuncSpare, 0);
		prog->state = 1;
		prog->timer = 0;
	}
	else if (prog->state == 0)
	{
		prog->update();
		eneFireLink->update();
		friendLink->update();
		friendFireLink->update(friendLink->head->sprite->pos, friendLink->head->sprite->angle - 90.f);
		rockLink->update();
		eneLink->update();
		supplyLink->update();
		glutPostRedisplay();
		glutTimerFunc(dtMs, myTimerFunc, 0);

	}
}


//puase
void displaySpare()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	backGround->display();
	rockLink->display();
	eneLink->display();
	eneFireLink->display();
	friendFireLink->display();
	supplyLink->display();
	friendLink->display();
	panelLink->display();


	glColor3d(1, 1, 1);

	glPushMatrix();
	{
		glTranslatef(-.54f, .2f, 0.f);
		glScalef(1. / 900., 1. / 900., 0.);
		char chars[64];
		sprintf(chars, "YOU ARE DEAD");
		for (int i = 0; i < 64; i++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, chars[i]);

	}
	glPopMatrix();
	glPushMatrix();
	{
		glTranslatef(-.46f, -.2f, 0.f);
		glScalef(1. / 1200., 1. / 1200., 0.);
		char chars[64];
		sprintf(chars, "Press Enter Key...");
		for (int i = 0; i < 64; i++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, chars[i]);

	}
	glPopMatrix();
	glPushMatrix();
	{
		glTranslatef(0.5f, -.95f, 0.f);
		glScalef(1. / 2400., 1. / 2400., 0.);
		char chars[64];
		sprintf(chars, "A Jack's Game");
		for (int i = 0; i < 64; i++)
			glutStrokeCharacter(GLUT_STROKE_ROMAN, chars[i]);

	}
	glPopMatrix();
	glFlush();
	glutSwapBuffers();
}

void timerFuncSpare(int value)
{
	//glutPostRedisplay();
	//glutTimerFunc(dtMs, timerFuncSpare, 0);
}

void mouse_motionSpare(int x, int y)
{
}

void myMouseSpare(int button, int state, int x, int y)
{
}

void onKeyPressSpare(unsigned char key, int x, int y)
{
	if (key == 13)
	{
		glutDisplayFunc(display);
		glutMouseFunc(myMouse);
		glutMotionFunc(mouse_motion);
		glutPassiveMotionFunc(mouse_motion);
		glutKeyboardFunc(onKeyPress);
		glutKeyboardUpFunc(onKeyRelease);
		//glutSpecialFunc(specialKeyPress);
		glutTimerFunc(dtMs, myTimerFunc, 0);
		prog->state = 0;
		init();
	}
}

void onKeyReleaseSpare(unsigned char key, int x, int y)
{
}

void specialKeyPressSpare(int key, int x, int y)
{
}

//
GameProgram::GameProgram()
{
	timer = 0;
	state = 0;
}

void GameProgram::update()
{
	timer++;
	if (timer == 1500)// 40 seconds
	{
		eneLink->cdSEneTotal=400;
	}
	else if (timer == 2000)
	{
		eneLink->cdTotal = 200;
	}
	else if (timer == 2500)
	{
		eneLink->cdSEneTotal = 300;
	}
	else if (timer == 3000)
	{
		eneLink->cdTotal = 100;
	}
	else if (timer == 3500)
	{
		eneLink->cdSEneTotal = 200;
	}
	else if (timer == 4000)
	{
		eneLink->cdSEneTotal = 100;
	}
}

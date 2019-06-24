// ConsoleApplication1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <GL/glut.h>
#include "fram.h"

int main(int argc, char *argv[]) {
	// initialize image magic:
	// create instance of an ImageWindow (global variable)

	// window size parameters
	int w = 800; int h = 800;

	// glut initialization functions:
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SilentBeatHazard");

	test();

	// display, onMouseButton, mouse_motion, onKeyPress, and resize are functions defined above
	glutDisplayFunc(display);
	glutMouseFunc(myMouse);
	glutMotionFunc(mouse_motion);
	glutPassiveMotionFunc( mouse_motion );
	glutKeyboardFunc(onKeyPress);
	glutKeyboardUpFunc(onKeyRelease);
	//glutSpecialFunc(specialKeyPress);
	glutTimerFunc(dtMs, myTimerFunc, 0);
	glutReshapeFunc(resize);

	// start glutMainLoop -- infinite loop
	glutMainLoop();
	return 0;
}




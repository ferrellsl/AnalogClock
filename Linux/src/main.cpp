/*
 * main.cpp
 *
 * Example of using the sprite class to create an Analog Clock.
 *
 *  Created on: 2010-12-26
 *      Author: Michael Yagudaev
 *      Copyright: yagudaev.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 3 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * ---------------------------------------------------------------------
 */

#include <GL/glut.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <time.h>
#include "Sprite.h"

#define ESCAPE_KEY 27

using namespace std;

static const int TIME_INTERVAL_MS = 1000; // in milliseconds

// Fixed logical size the clock is designed at. glOrtho always uses these
// values so the sprites' coordinates stay meaningful; glViewport is what
// actually scales that fixed logical space to fill the real window.
static const int INITIAL_WINDOW_WIDTH = 524;
static const int INITIAL_WINDOW_HEIGHT = 524;

static int windowWidth = INITIAL_WINDOW_WIDTH;
static int windowHeight = INITIAL_WINDOW_HEIGHT;

static Sprite *clockFace = NULL;
static Sprite *hoursHand = NULL;
static Sprite *minutesHand = NULL;
static Sprite *secondsHand = NULL;

void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glRasterPos2i(0, 0);

	// draw the clock
	clockFace->setPivot(0.5, 0.5);
	clockFace->setX(0);
	clockFace->setY(0);
	clockFace->draw();

	hoursHand->setX(0);
	hoursHand->setY(0);
	hoursHand->setPivot(0.5, 0.075);
	hoursHand->draw();

	minutesHand->setX(0);
	minutesHand->setY(0);
	minutesHand->setPivot(0.5, 0.0566);
	minutesHand->draw();

	secondsHand->setX(0);
	secondsHand->setY(0);
	secondsHand->setPivot(0.5, 0.0545);
	secondsHand->draw();

	glFlush();
	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h)
{
	windowWidth = w;
	windowHeight = h;

	// Compute the largest square that fits centered in the window, so the
	// round clock face scales up/down with the window but never gets
	// stretched into an oval on non-square resizes. Any leftover space is
	// letterboxed (left as background) on the shorter axis.
	int squareSize = (w < h) ? w : h;
	int viewportX = (w - squareSize) / 2;
	int viewportY = (h - squareSize) / 2;

	glViewport(viewportX, viewportY, (GLsizei) squareSize, (GLsizei) squareSize);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Always use the fixed original logical size here (not w/h). The
	// viewport above is what maps this fixed logical space onto however
	// large or small the actual window/square region currently is, which
	// is what makes the clock graphics scale with the window.
	glOrtho(-INITIAL_WINDOW_WIDTH/2, INITIAL_WINDOW_WIDTH/2,
	        -INITIAL_WINDOW_HEIGHT/2, INITIAL_WINDOW_HEIGHT/2, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init (void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glShadeModel(GL_FLAT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Sprite::enable2D();

	clockFace = new Sprite("graphics/clockface.bmp");
	hoursHand = new Sprite("graphics/hours_hand.bmp");
	minutesHand = new Sprite("graphics/minutes_hand.bmp");
	secondsHand = new Sprite("graphics/seconds_hand.bmp");

	// clear buffer and display image
	reshape(windowWidth, windowHeight);
	display();
}

// FIX: this is now a GLUT timer callback (invoked via glutTimerFunc) instead
// of an idle callback. GLUT timer callbacks take an int "value" argument.
// The function reschedules itself at the bottom, so it runs once every
// TIME_INTERVAL_MS with the OS sleeping the process in between calls,
// instead of GLUT spinning on it continuously.
void clockAnimation(int value)
{
	time_t unixTime = time(NULL);
	struct tm *currentTime = localtime(&unixTime);

	// note we use negative angles because in math angles are always measured counter-clockwise
	// so by using a negative angle we will get a clockwise angle needed for our clock.
	hoursHand->setAngle(-1 * (30 * currentTime->tm_hour + ((int)(6 * currentTime->tm_min / 90.0)) * 7.5));
	minutesHand->setAngle(-1 * 6 * currentTime->tm_min);
	secondsHand->setAngle(-1 * 6 * currentTime->tm_sec);

	glutPostRedisplay();

	// reschedule ourselves to run again after TIME_INTERVAL_MS
	glutTimerFunc(TIME_INTERVAL_MS, clockAnimation, 0);
}

/**
 * Clean up before exiting the program
 */
void cleanup()
{
	delete clockFace;
	delete hoursHand;
	delete minutesHand;
	delete secondsHand;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ESCAPE_KEY:
		cleanup();
		exit(0);
		break;
	default:
		break;
	}
}

int main (int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Analog Clock");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	// FIX: kick off the timer-based animation loop instead of
	// glutIdleFunc(clockAnimation), which was causing 100% CPU usage.
	glutTimerFunc(TIME_INTERVAL_MS, clockAnimation, 0);

	glutMainLoop();

	return 0;
}


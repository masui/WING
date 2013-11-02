/*
	WING for Win95/NT

	970905 mina
*/

/*
	$Date: 1995/01/31 21:34:04 $
	$Revision: 1.5 $
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/move.c,v 1.5 1995/01/31 21:34:04 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	<math.h>
#include	"move.h"
#include	"main.h"
#include	"draw.h"
#include	"calc.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif


/* global variables */
float	lspeed = 0.05;		/* speed */
float	aspeed = 30.0;		/* speed */
int	moving = FALSE;		/* moving? */



void moveTo(float to[2])
{
float	moveto[3];
float	movefrom[3];
float	tmp_pnt[3];
float	dist;
float	origPhy;
float	orig_eye_x, orig_eye_z;
float	toPhy;
float	diffPhy;
int	numstep;
int	i;
GLfloat	mat[4][4];
double	r_phy;
float	vec1[2], vec2[2];

moving = TRUE;

	origPhy = phy;
	orig_eye_x = eye_x;
	orig_eye_z = eye_z;

	moveto[0] = to[0];
	moveto[2] = to[1];
	toPhy = atan2(moveto[2] - eye_z, moveto[0] - eye_x) * 1800.0 / PI + 900.0;
	if (toPhy > 1800.0)  toPhy -= 3600.0;
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(toPhy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
	calcLookAtPoint(tmp_pnt, FALSE);
	moveto[0] += eye_x - tmp_pnt[0];
	moveto[1] = eye_y;
	moveto[2] += eye_z - tmp_pnt[2];

	diffPhy = toPhy - (float)phy;
	if (fabs(diffPhy) > 1800.0) {
		if (diffPhy > 0) {
			diffPhy -= 3600.0;
		} else {
			diffPhy += 3600.0;
		}
	}

	movefrom[0] = eye_x;
	movefrom[1] = eye_y;
	movefrom[2] = eye_z;
	dist = f_distance(movefrom, moveto);
	numstep = ((dist / lspeed) > fabs(diffPhy / aspeed)) ? dist / lspeed : fabs(diffPhy / aspeed);
	vec1[0] = tmp_pnt[0] - eye_x;
	vec1[1] = tmp_pnt[2] - eye_z;
	vec2[0] = moveto[0] - eye_x;
	vec2[1] = moveto[2] - eye_z;
	if (vec1[0] * vec2[0] + vec1[1] * vec2[1] < 0.0)  dist *= -1.0;

	glutSetWindow(mapwindow);

/* slow in, slow out */
if (numstep > 0) {
	for (i = 0; i < 3; i++) {
		phy = (int)(origPhy + diffPhy * (float)i / (float)numstep / 3.0);
		r_phy = ((double)phy - 900.0) * PI / 1800.0;
		eye_x = orig_eye_x + dist * (float)i / (float)numstep / 3.0 * cos(r_phy);
		eye_z = orig_eye_z + dist * (float)i / (float)numstep / 3.0 * sin(r_phy);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}
}
if (numstep >= 3) {
	for (i = 0; i < 2; i++) {
		phy = (int)(origPhy + diffPhy * (1.0 + (float)i / 2.0) / (float)numstep);
		r_phy = ((double)phy - 900.0) * PI / 1800.0;
		eye_x = orig_eye_x + dist * (1.0 + (float)i / 2.0) / (float)numstep * cos(r_phy);
		eye_z = orig_eye_z + dist * (1.0 + (float)i / 2.0) / (float)numstep * sin(r_phy);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}
}
if (numstep >= 5) {
	for (i = 2; i <= numstep - 3; i++) {
		phy = (int)(origPhy + diffPhy * (float)i / (float)numstep);
		r_phy = ((double)phy - 900.0) * PI / 1800.0;
		eye_x = orig_eye_x + dist * (float)i / (float)numstep * cos(r_phy);
		eye_z = orig_eye_z + dist * (float)i / (float)numstep * sin(r_phy);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}
}
if (numstep >= 4) {
	for (i = 0; i < 2; i++) {
		phy = (int)(origPhy + diffPhy * ((float)(numstep - 2) + (float)i / 2.0) / (float)numstep);
		r_phy = ((double)phy - 900.0) * PI / 1800.0;
		eye_x = orig_eye_x + dist * ((float)(numstep - 2) + (float)i / 2.0) / (float)numstep * cos(r_phy);
		eye_z = orig_eye_z + dist * ((float)(numstep - 2) + (float)i / 2.0) / (float)numstep * sin(r_phy);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}
}
if (numstep >= 2) {
	for (i = 0; i < 3; i++) {
		phy = (int)(origPhy + diffPhy * ((float)(numstep - 1) + (float)i / 3.0) / (float)numstep);
		r_phy = ((double)phy - 900.0) * PI / 1800.0;
		eye_x = orig_eye_x + dist * ((float)(numstep - 1) + (float)i / 3.0) / (float)numstep * cos(r_phy);
		eye_z = orig_eye_z + dist * ((float)(numstep - 1) + (float)i / 3.0) / (float)numstep * sin(r_phy);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}
}

	phy = toPhy;
	eye_x = moveto[0];
	eye_z = moveto[2];
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(toPhy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];

	calcLookAtPoint(lookAt, FALSE);
moving = FALSE;
	drawScene();	/* to animate */
}

void zoomTo(float distance)
{
float	d;
float	eye[3];
float	to[3];
float	moveto[3];
float	delta[3];
int	numstep;
int	i;
	
	eye[0] =eye_x;
	eye[1] =eye_y;
	eye[2] =eye_z;
	to[0] = to_x;
	to[1] = to_y;
	to[2] = to_z;

/* lookAt should be calclated */
	calcLookAtPoint(lookAt, FALSE);
	d = f_distance(eye, lookAt);

	moveto[0] = lookAt[0] + (eye[0] - lookAt[0]) * distance / d;
	moveto[1] = lookAt[1] + (eye[1] - lookAt[1]) * distance / d;
	moveto[2] = lookAt[2] + (eye[2] - lookAt[2]) * distance / d;

	numstep = fabs(d - distance) / lspeed;
	delta[0] = (moveto[0] - eye[0]) / (float)numstep;
	delta[1] = (moveto[1] - eye[1]) / (float)numstep;
	delta[2] = (moveto[2] - eye[2]) / (float)numstep;

	glutSetWindow(mapwindow);

moving = TRUE;
	for (i = 0; i < numstep; i++) {
		eye_x += delta[0];
		eye_y += delta[1];
		eye_z += delta[2];
		to_x += delta[0];
		to_y += delta[1];
		to_z += delta[2];
		calcLookAtPoint(lookAt, TRUE);
		drawScene();	/* to animate */
	}

	eye_x = moveto[0];
	eye_y = moveto[1];
	eye_z = moveto[2];
	to_x = to[0] + moveto[0] - eye[0];
	to_y = to[1] + moveto[1] - eye[1];
	to_z = to[2] + moveto[2] - eye[2];
	calcLookAtPoint(lookAt, FALSE);
moving = FALSE;
	drawScene();	/* to animate */
}


void moveAlongPath(Path *p, Mark *start)
{
/* old */
int	dir;
int	i;
Path	*subpath;

	if ((p->points)[0] == start)  dir = 1;
	else if ((p->points)[p->numpoints - 1] == start)  dir = -1;
	else {
		fprintf(stderr, "cannot find start point\n");
		return;
	}

	if (dir == 1) {
		for (i = 0; i < p->numpoints - 1; i++) {
			if (subpath = searchSubPath((p->points)[i], (p->points)[i + 1]))  moveAlongPath(subpath, (p->points)[i]);
			else moveToMark((p->points)[i + 1], NULL);
		}
	} else if (dir == -1) {
		for (i = p->numpoints - 1; i > 0; i--) {
			if (subpath = searchSubPath((p->points)[i], (p->points)[i - 1]))  moveAlongPath(subpath, (p->points)[i]);
			else moveToMark((p->points)[i - 1], NULL);
		}
	}
}

void moveToMark(Mark *end, Mark *start)
{
/* old */
float	coord[2];
Path	*p = NULL;

	if (start != NULL) {
	/* search the path */
		if (p = searchSubPath(start, end)) {
			/* found */
			moveAlongPath(p, start);
			return;
		}
	}

	coord[0] = (float)(end->location[0]) / mapsize[0];
	coord[1] = (float)(end->location[1]) / mapsize[1];
	moveTo(coord);
		
}


void rotateTo(float to[2])
{
/* old */
float	toPhy;
float	origPhy;
int	i;
GLfloat	mat[4][4];
int	numstep;
float	diffPhy;

moving = TRUE;

/* rotate */
	glutSetWindow(mapwindow);

	toPhy = atan2(to[1] - eye_z, to[0] - eye_x);
	toPhy *= 1800.0 / PI;
	toPhy += 900.0;
	if (toPhy > 1800.0)  toPhy -= 3600.0;
	diffPhy = toPhy - (float)phy;
	if (fabs(diffPhy) > 1800.0) {
		if (diffPhy > 0) {
			diffPhy -= 3600.0;
		} else {
			diffPhy += 3600.0;
		}
	}
	origPhy = (float)phy;
	numstep = fabs(diffPhy / aspeed);
	for (i = 0; i < numstep; i++) {
		phy = (int)(origPhy + diffPhy * (float)i / (float)numstep);
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];
		calcLookAtPoint(lookAt, TRUE);
		glutPostRedisplay();
	}
	phy = (int)toPhy;
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
moving = FALSE;
	calcLookAtPoint(lookAt, FALSE);
	glutPostRedisplay();
}



void translateTo(float to[2])
{
/* old */
int	i;
GLfloat	mat[4][4];
int	numstep;

moving = TRUE;

/* calc matrix */
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
	glPopMatrix();

/* move */
	glutSetWindow(mapwindow);
{
	float	eye_x_bak = eye_x;
	float	eye_z_bak = eye_z;
	float	dist = sqrt((to[0] - eye_x) * (to[0] - eye_x) + (to[1] - eye_z) * (to[1] - eye_z));
	numstep = dist / lspeed;
	for (i = 0; i < numstep; i++) {
		eye_x = eye_x_bak + (to[0] - eye_x_bak) * (double)i / (float)numstep;
		eye_z = eye_z_bak + (to[1] - eye_z_bak) * (double)i / (float)numstep;

		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];

		calcLookAtPoint(lookAt, TRUE);
		glutPostRedisplay();
	}
}

	eye_x = to[0];
	eye_z = to[1];
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
moving = FALSE;
	calcLookAtPoint(lookAt, FALSE);
	glutPostRedisplay();
}

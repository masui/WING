/*
$Date: 1995/01/31 16:23:14 $
$Revision: 1.6 $
*/
static char rcsid[] = "$Header: /usr/people/masui/WING/wing/RCS/draw.c,v 1.6 1995/01/31 16:23:14 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
#include	<GL/glu.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"main.h"
#include	"draw.h"
#include	"mapData.h"
#include	"calc.h"
#include	"geom.h"
#include	"mapitem.h"
#include	"move.h"
#include	"winpos.h"
#include	"rgb.h"

#ifndef TRUE
#define TRUE	(0 == 0)
#endif
#ifndef FALSE
#define FALSE	(0 != 0)
#endif



/*** global variables ***/

/* for shading */
/* materials */
GLfloat	mat_amb_diff[2][4] = 
{
	{0.6, 0.6, 0.6, 0.8},	/* map item */
	{1.0, 0.0, 0.0, 0.8},	/* compass */
};


/* lights */
GLfloat	light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat	light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};

/* modeling properties */
float   mod_prop[MAX_MODELS][10] =
	{	/* model 1 */
		GL_AMBIENT,        0.2, 0.2, 0.2,
		GL_LIGHT_MODEL_LOCAL_VIEWER,    0.0,
		GL_CONSTANT_ATTENUATION,    1.0, 0.0,
	};

/* for texture */
/* texture properties */
float	texprops[MAX_TEXPROPS][7];
/* texture environments */
float	tevprops[MAX_TEVPROPS][2];

/*** end of global variables ***/



int openMapWindow(char *name)
{
int	mapwindow;
float	fog[5] = {0.0, 5.0, 0.0, 0.0, 0.0};

	glutInitWindowPosition(MAPPOSX, MAPPOSY);
	glutInitWindowSize(MAPSIZE, MAPSIZE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	mapwindow = glutCreateWindow(name);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glShadeModel(GL_FLAT);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (fogmode) {
	/* 970903  This may be not used... */
		glFogfv(GL_LINEAR, fog);
		glEnable(GL_FOG);
		glFogfv(1, NULL);
		glEnable(GL_FOG);
	}
	if (antialiasmode) {
		glEnable(GL_LINE_SMOOTH);
	}

	defLights();

	return mapwindow;
}


void defLights(void)
{
/* 970912 mina  for OpenGL */
	glEnable(GL_LIGHTING);

	/* setting for lights */
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glEnable(GL_LIGHT0);
}


int readTexture(char *filename, int prop)
{
/* 970903  texture mode has no longer supported. */
/* old gl source
static int	texture_id = 0;
unsigned long	*texdata;
long		width, height;

	if (strstr(filename, ".bw")) {
		if ((texdata = loadBW(filename, &width, &height)) == NULL)  return(0);
		texture_id++;
		texdef2d(texture_id, 1, width, height, texdata, 0, texprops[prop]);
	} else {
		if ((texdata = loadRGB(filename, &width, &height)) == NULL)  return(0);
		texture_id++;
		texdef2d(texture_id, 4, width, height, texdata, 0, texprops[prop]);
	}

	return(texture_id);
	*/
	return 0;
}


void drawScene()
{
	glutSetWindow(mapwindow);	/* set the current window */
	glClear(GL_COLOR_BUFFER_BIT);	/* clear the drawing area */

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
/*
        gluPerspective(.1*(600),  1.0,  0.0,  10.0);
*/
        gluPerspective(.1*(600),  1.0,  0.01,  100.0);

	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();

		gluLookAt(eye_x,  eye_y,  eye_z,  to_x,  to_y,  to_z, 0.0, 1.0, 0.0);
		if (wiremode)  drawMapWire();
		else if (texturemode)  drawMapTexture();
		else if (mapmode)  drawDigitalMap();
		else  drawMapPlane();

		drawScope();
	glPopMatrix();

	if (compassmode)  drawCompass();
	if (moving)  drawMoving();

	glutSwapBuffers();
}


void drawMapPlane()
{
static float	vertex[4][3] = {
	{-2.0,  0.0, -2.0},
	{ 2.0,  0.0, -2.0},
	{ 2.0,  0.0,  2.0},
	{-2.0,  0.0,  2.0},
	};
static float	n[3] = {0.0, 1.0, 0.0};

	glDisable(GL_LIGHTING);
	glBegin(GL_POLYGON);
		glNormal3fv(n);
		glVertex3fv(vertex[0]);
		glVertex3fv(vertex[1]);
		glVertex3fv(vertex[2]);
		glVertex3fv(vertex[3]);
	glEnd();
}


void drawMapTexture()
{
static float	texture[4][2] = {
	{0.0, 0.0},
	{1.0, 0.0},
	{1.0, 1.0},
	{0.0, 1.0},
};
static float	vertex[4][3] = {
	{-2.0,  0.0,  2.0},
	{ 2.0,  0.0,  2.0},
	{ 2.0,  0.0, -2.0},
	{-2.0,  0.0, -2.0},
};

	glColor3ub(0xff, 0xff, 0xff);

	glBegin(GL_POLYGON);
		glTexCoord2fv(texture[0]);	glVertex3fv(vertex[0]);
		glTexCoord2fv(texture[1]);	glVertex3fv(vertex[1]);
		glTexCoord2fv(texture[2]);	glVertex3fv(vertex[2]);
		glTexCoord2fv(texture[3]);	glVertex3fv(vertex[3]);
	glEnd();
}


void drawMapWire()
{
float	p1[3], p2[3];

	glDisable(GL_LIGHTING);

	p1[1] = 0.0;
	p2[1] = 0.0;

	glColor3ub(0x0, 0xff, 0xff);

	p1[2] = -2.0;
	p2[2] =  2.0;
	for (p1[0] = -2.0; p1[0] <= 2.01; p1[0] += 0.1) {
		p2[0] = p1[0];
		glBegin(GL_LINE_STRIP);
			glVertex3fv(p1);
			glVertex3fv(p2);
		glEnd();
	}
	p1[0] = -2.0;
	p2[0] =  2.0;
	for (p1[2] = -2.0; p1[2] <= 2.01; p1[2] += 0.1) {
		p2[2] = p1[2];
		glBegin(GL_LINE_STRIP);
			glVertex3fv(p1);
			glVertex3fv(p2);
		glEnd();
	}

	p1[0] = 0.0;
	p2[0] = 2.0;
	p1[1] = 0.0;
	p2[1] = 0.0;
	p1[2] = 0.0;
	p2[2] = 0.0;
	glColor3ub(0xff, 0x0, 0x0);
	glBegin(GL_LINE_STRIP);
		glVertex3fv(p1);
		glVertex3fv(p2);
	glEnd();
	p1[0] = 0.0;
	p2[0] = 0.0;
	p1[1] = 0.0;
	p2[1] = 2.0;
	p1[2] = 0.0;
	p2[2] = 0.0;
	glColor3ub(0x0, 0xff, 0x0);
	glBegin(GL_LINE_STRIP);
		glVertex3fv(p1);
		glVertex3fv(p2);
	glEnd();
	p1[0] = 0.0;
	p2[0] = 0.0;
	p1[1] = 0.0;
	p2[1] = 0.0;
	p1[2] = 0.0;
	p2[2] = 2.0;
	glColor3ub(0x0, 0x0, 0xff);
	glBegin(GL_LINE_STRIP);
		glVertex3fv(p1);
		glVertex3fv(p2);
	glEnd();
}

void changeView(int newTheta)
{
int	numstep;
int	i;
int	dangle;
GLfloat	mat[4][4];
float	look[2];

	numstep = abs(newTheta - theta) / 30;
	dangle = newTheta > theta ? 30 : -30;
	look[0] = lookAt[0]; look[1] = lookAt[2];

	moving = TRUE;
	/* slow in, slow out */
	if (numstep > 0) {
	for (i = 1; i < 3; i++) {
		theta += dangle / 3;
		glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];
		drawScene();
	}
	}
	if (numstep >= 3) {
	for (i = 0; i < 2; i++) {
		theta += dangle / 2;
			glPushMatrix();
			glLoadIdentity();
			glRotatef(.1*(theta), 1.0, 0.0, 0.0);
			glRotatef(.1*(phy), 0.0, 1.0, 0.0);
			glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
		glPopMatrix();
		to_x = eye_x - mat[0][2];
		to_y = eye_y - mat[1][2];
		to_z = eye_z - mat[2][2];
		drawScene();
	}
	}
if (numstep >= 5) {
for (i = 2; i <= numstep - 3; i++) {
	theta += dangle;
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
	drawScene();
}
}
if (numstep >= 4) {
for (i = 0; i < 2; i++) {
	theta += dangle / 2;
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
	drawScene();
}
}
if (numstep >= 2) {
for (i = 0; i < 3; i++) {
	theta += dangle / 3;
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
	glPopMatrix();
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
	drawScene();
}
}

theta = newTheta;
glPushMatrix();
	glLoadIdentity();
	glRotatef(.1*(theta), 1.0, 0.0, 0.0);
	glRotatef(.1*(phy), 0.0, 1.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
glPopMatrix();
to_x = eye_x - mat[0][2];
to_y = eye_y - mat[1][2];
to_z = eye_z - mat[2][2];
drawScene();

/* move to the pervious looking point */
moveTo(look);
}


typedef float P[3];

void drawDigitalMap()
{
float	sph[4];
register int	prevcol;
register int	i, j;

	glInitNames();

	/* draw lines */
	glDisable(GL_LIGHTING);
	glColor3ub((colors[lines[0].color]) & 0xff,(colors[lines[0].color])>>8 & 0xff,(colors[lines[0].color])>>16 & 0xff);
	prevcol = lines[0].color;
	for (i = 0; i < num_lines; i++) {
		Line *p = &lines[i];
		P *q;
		if (prevcol != p->color) {
			glColor3ub((colors[p->color]) & 0xff,(colors[p->color])>>8 & 0xff,(colors[p->color])>>16 & 0xff);
			prevcol = p->color;
		}
		/* 行政界（赤線）を書かない場合
		if (prevcol == 1)  continue;
		*/
		glBegin(GL_LINE_STRIP);
			q = p->f_points;
			for (j = 0; j < p->numpoints; j++) {
				glVertex3fv(*(q++));
			}
		glEnd();
	}

	/* draw items */
	for (i = 0; i < num_parts; i++) {
		glLoadName(i);
		drawPart(&(parts[i]));
	}
}


void drawPenCursor(int pad_x, int pad_y, int tilt_x, int tilt_y)
{
/* 970903  pen mode has no longer supported */
/* do nothing */
}

int checkSelection()
{
/* 970903  This function has not been used. */
return -1;
}

void drawPart(Part *p)
{
int	i, j;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff[p->mat]);

	for (i = 0; i < p->num_face; i++) {
		if (i == p->hilight_face) {
			glDisable(GL_LIGHTING);
			glColor3ub((p->col) & 0xff,(p->col)>>8 & 0xff,(p->col)>>16 & 0xff);
		} else {
			glEnable(GL_LIGHTING);
		}
		glBegin(GL_POLYGON);
			j = 0;
			while(p->f[i][j] >= 0) {
				glNormal3fv(p->n[i]);
				glVertex3fv(p->v[p->f[i][j]]);
				j++;
			}
		glEnd();
	}
}


void drawCompass()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0,  0.0,  1.0,  0.0,  0.0,  0.0, 0.0, 1.0, 0.0);
	glTranslatef(0.9,  0.9,  1.0);
	glRotatef(.1*(theta), 1.0, 0.0, 0.0);
	glRotatef(.1*(phy), 0.0, 1.0, 0.0);
	glScalef(2.0, 2.0, 2.0);	/* 970907 mina */
	drawPart(&compass);

	glPopMatrix();
	glPopMatrix();
}

void drawMoving()
{
int	i;
char	str[] = "AUTO MOVING";

	glDisable(GL_LIGHTING);
	glColor3ub(0x0, 0xff, 0xff);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0,  1.0,  0.0,  1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0,  0.0,  1.0,  0.0,  0.0,  0.0, 0.0, 1.0, 0.0);
	glRasterPos2f(0.38,  0.95);	/* この値は適当に調整 */
	for (i = 0; i < 11; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
	glPopMatrix();
	glPopMatrix();
}


void drawScope()
{
/* 970903  Is this enough? */

float	nvec[3] = {0.0, 1.0, 0.0};
float	look[3];
GLUquadricObj *qobj;

/* to draw the scope on the ground */
	look[0] = lookAt[0];
	look[2] = lookAt[2];
	calcAltitude(look);

	glDisable(GL_LIGHTING);
	if (moving)  glColor3ub(0xff, 0xff, 0x00);
	else glColor3ub(0xff, 0xff, 0xff);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glRotatef(.1*(-900), 1.0, 0.0, 0.0);
		glTranslatef(0.0,  0.0,  look[1]);
		glNormal3fv(nvec);
		qobj = gluNewQuadric();
		glPushMatrix();
			glTranslatef(lookAt[0], -lookAt[2], 0.);
			gluDisk(qobj, viewrange * 0.99,  viewrange, 32, 1);
		glPopMatrix();
		gluDeleteQuadric(qobj);
	glPopMatrix();
}


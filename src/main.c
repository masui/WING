/*
	WING for Win95/NT

	97/09/04 mina
*/

/*
	$Date: 1995/02/03 15:35:24 $
	$Revision: 1.27 $

	水口版に増井が若干の変更を加えているもの

	[../doc/Interface.tex] に操作手法の方針について書いてある。

*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/main.c,v 1.27 1995/02/03 15:35:24 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#include	<sys/types.h>
#include	<sys/mman.h>
/*
#include	<alloca.h>
*/

#include	<fcntl.h>
#include	<stdio.h>
#include	<signal.h>
#include	<math.h>
#include	"main.h"
#include	"draw.h"
#include	"move.h"
#include	"mapData.h"
#include	"digitalMap.h"
#include	"mapitem.h"
#include	"geom.h"
#include	"calc.h"
#include	"index.h"
#include	"toc.h"
#include	"dataentry.h"
#include	"showGuide.h"
#include	"sub.h"


#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif


/* global variables */
int		mapwindow;	/* window id */
float		eye_x = -1.639130;	/* eye position */
float		eye_y = 1.0;
float		eye_z = -0.271015;
float		to_x = -0.773105;	/* target position */
float		to_y = 0.5;
float		to_z = -0.271014;
int		phy = 900, theta = 300;
				/* phy: yaw, theta: pitch */
float		lookAt[3];
float		viewrange;		/* the diameter of the search area */
int		wiremode = FALSE;	/* display modes */
int		mapmode = TRUE;
int		texturemode = FALSE;
int		antialiasmode = FALSE;	/* 970904 いいグラフィクスカードなら使えるかも */
int		fogmode = FALSE;
int		penstatusmode = FALSE;
int		compassmode = TRUE;
char		*texturename;	/* texture file */


/* 970904 マウスイベント処理用 */
static int	origx, origy;
static int	mouseLpressed = FALSE;
static int	mouseRpressed = FALSE;
static int	rotating = FALSE;




int main(int argc, char *argv[])
{
int	tid;	/* texture id */
long	x, y;

	glutInit(&argc, argv);
	options(argc, argv);

	if (mapmode) {
	/* read map data */
		readDigitalMap(MAPDIR "nara.map");
		readMeshData(MAPDIR "nara.mem");
		calcAllAltitude();
	}

	mapwindow = openMapWindow(argv[0]);	/* initialize 3D map window */

	/* 970904 イベントハンドラの設定 */
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(mapKeyFunc);
	glutSpecialFunc(mapSpecialFunc);	/* 特殊キー用 */
	glutMouseFunc(mapMouseFunc);
	glutMotionFunc(mapMotionFunc);

	readdata();			/* read guide data */
	initGuide();			/* initialize the guide */
	initIndex();			/* initialize the index */
	initTOC();			/* initialize the TOC */

/* prepare graphics objects */
	setPartNVec(&compass);
	setPartNVec(&pencursor);

/* texture */
	/* 970904 あとで参考にするかもしれない
	if (!wiremode && !mapmode && texturemode) {
		tevdef(1, 0, tevprops[0]);
		tevbind(TV_ENV0, 1);
		if ((tid = readTexture(texturename, texturemode - 1)) == 0) {
			perror("cannot read texture data\n");
			exit(0);
		}
		texbind(TX_TEXTURE_0, tid);
	}
	*/

	calcLookAtPoint(lookAt, FALSE);
	glutSetWindow(mapwindow);
	glutPostRedisplay();

/* main loop */
	glutMainLoop();
}




/* 地図ウィンドウでの処理
case MAP_WINDOW:
handleMap(x, y);
calcLookAtPoint(lookAt);
drawScene();
break;
*/


void options(int argc, char *argv[])
{
	int     i;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'n':
				texturemode = FALSE;
				break;
			case 'w':
				wiremode = TRUE;
				break;
			case 't':
				texturemode = atoi(&(argv[i][2]));
				if (!texturemode)  texturemode = TRUE;
				break;
			case 'm':
				mapmode = TRUE;
				wiremode = FALSE;
				texturemode = FALSE;
				break;
			case 'a':
				antialiasmode = TRUE;
				break;
			case 'f':
				fogmode = TRUE;
				break;
			case 's':
				penstatusmode = TRUE;
				break;
			default:
				usage(argv[0]);
				exit(1);
				break;
			}
		} else {
			texturename = argv[i];
		}
	}
}


void usage(char *commandname)
{
	fprintf(stderr, " Usage : %s [texture filename]\n", commandname);
	fprintf(stderr, "Options: -n : no texture mode\n");
	fprintf(stderr, "         -w : wire mode (grid)\n");
	fprintf(stderr, "         -t : texture mode\n");
	fprintf(stderr, "         -m : digital map mode\n");
	fprintf(stderr, "         -a : draw with anti-aliasing\n");
	fprintf(stderr, "         -f : draw with fog (depth-queing)\n");
	fprintf(stderr, "         -s : draw pen status\n");
}

void showKeyBinds()
{
	fprintf(stderr, "\n");
	fprintf(stderr, "  /   : display this message\n");
	fprintf(stderr, " ESC  : quit this program\n");
	fprintf(stderr, "space : select the pointing object\n");
	fprintf(stderr, " HOME : reset view position\n");
	fprintf(stderr, "  A   : toggle alias/antialias mode\n");
	fprintf(stderr, "  C   : show/hide the compass\n");
	fprintf(stderr, "  S   : show/hide the pen status\n");
	fprintf(stderr, "  V   : change/restore view position\n");
	fprintf(stderr, "  N   : go to the next object\n");
	fprintf(stderr, "  B   : go to the previous object\n");
	fprintf(stderr, "  G   : go to the current object\n");
	fprintf(stderr, "  D   : delete the current object from the list\n");
}


void wingReset()
{

/* 970910 mina  実際に動かして得た値
eye = (-1.639130, 1.000000, -0.271015), to = (-0.773105, 0.500000, -0.271014)
*/
	eye_x = -1.639130; eye_y = 1.0; eye_z = -0.271015;
	to_x = -0.773105; to_y = 0.5; to_z = -0.271014;
	phy = 900, theta = 300;
	calcLookAtPoint(lookAt, FALSE);
	glutSetWindow(mapwindow);
	glutPostRedisplay();

	resetTOC();
	resetIndex();
}

void mapKeyFunc(unsigned char key, int x, int y)
{
	switch(key) {
	case 0x1b:	/* ESC: terminate this program */
		exit(0);
		break;
	case 'a':	/* toggle aliasing/anti-aliasing mode */
	case 'A':
		antialiasmode = !antialiasmode;
		/* 970904 まだちゃんとできていない */
		if (antialiasmode) {
			glEnable(GL_LINE_SMOOTH);
			glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);
		} else {
			glDisable(GL_LINE_SMOOTH);
		}
		glutSetWindow(mapwindow);
		glutPostRedisplay();
		break;
	case 'c':	/* show/hide compass */
	case 'C':
		compassmode = !compassmode;
		glutSetWindow(mapwindow);
		glutPostRedisplay();
		break;
	case '/':	/* show help */
	case '?':
		showKeyBinds();
		break;
	}
}

void mapSpecialFunc(int key, int x, int y)
{
	switch(key) {
	case GLUT_KEY_F1:
		/*
		reshapeviewport();
		*/
		glutSetWindow(mapwindow);
		glutPostRedisplay();
		glutSetWindow(tocwin);
		glutPostRedisplay();
		glutSetWindow(indexwin);
		glutPostRedisplay();
		glutSetWindow(gWid);
		glutPostRedisplay();
		break;
	case GLUT_KEY_HOME:	/* reset view position */
		wingReset();
		break;
	}
}


void mapMouseFunc(int button, int stat, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		mouseLpressed = (stat == GLUT_DOWN);
		rotating = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
	} else if (button == GLUT_RIGHT_BUTTON) {
		mouseRpressed = (stat == GLUT_DOWN);
	}

	if (stat == GLUT_DOWN) {
		origx = x; origy = y;
	}
}

void mapMotionFunc(int x, int y)
{
float	win_sx, win_sy;
double	dangle;
double	dmove;
GLfloat	mat[4][4];

	win_sx = (float)glutGet(GLUT_WINDOW_WIDTH);
	win_sy = (float)glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		glRotatef(.1*(theta), 1.0, 0.0, 0.0);
		glRotatef(.1*(phy), 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, &(mat[0][0]));
	glPopMatrix();


	if (mouseRpressed) {
	/* ズーム */
	float   eye_x_bak = eye_x;
	float   eye_y_bak = eye_y;
	float   eye_z_bak = eye_z;
	float   to_x_tmp, to_y_tmp, to_z_tmp;
	float   lookAt_tmp[3];
		eye_x -= (float)(to_x - eye_x_bak) * (float)(origx - x) / win_sx;
		eye_y -= (float)(to_y - eye_y_bak) * (float)(origx - x) / win_sx;
		eye_z -= (float)(to_z - eye_z_bak) * (float)(origx - x) / win_sx;
		to_x -= (float)(to_x - eye_x_bak) * (float)(origx - x) / win_sx;
		to_y -= (float)(to_y - eye_y_bak) * (float)(origx - x) / win_sx;
		to_z -= (float)(to_z - eye_z_bak) * (float)(origx - x) / win_sx;
		theta += 900.0 * (float)(y - origy) / win_sy;
		if (theta > 890)  theta = 890;
		if (theta < 10)  theta = 10;

		to_x_tmp = eye_x - mat[0][2];
		to_y_tmp = eye_y - mat[1][2];
		to_z_tmp = eye_z - mat[2][2];
		lookAt_tmp[0] = eye_x + (to_x_tmp - eye_x) * (eye_y - 0.1) / (eye_y - to_y_tmp);
		lookAt_tmp[1] = 0.1;
		lookAt_tmp[2] = eye_z + (to_z_tmp - eye_z) * (eye_y - 0.1) / (eye_y - to_y_tmp);
		eye_x += lookAt[0] - lookAt_tmp[0];
		eye_y += lookAt[1] - lookAt_tmp[1];
		eye_z += lookAt[2] - lookAt_tmp[2];

		origx = x; origy = y;
	}

	if (mouseLpressed) {
	/* 平行移動 */
	/* 970912 mina  Shiftを押しながらだと回転 */
	GLfloat	m[4][4];

		if (rotating) {
		/* 回転 */
			phy += origx - x;
		} else {
		/* 平行移動 */
			glPushMatrix();
				glLoadIdentity();
				glRotatef(.1*(phy), 0.0, 1.0, 0.0);
				glGetFloatv(GL_MODELVIEW_MATRIX, &(m[0][0]));
			glPopMatrix();

			eye_x += m[0][0] * (float)(origx - x) / win_sx;
			eye_z += m[2][0] * (float)(origx - x) / win_sx;
			eye_x += m[0][2] * (float)(origy - y) / win_sy;
			eye_z += m[2][2] * (float)(origy - y) / win_sy;
		}

		origx = x; origy = y;
	}

/* calc look at point */
	to_x = eye_x - mat[0][2];
	to_y = eye_y - mat[1][2];
	to_z = eye_z - mat[2][2];
	calcLookAtPoint(lookAt, FALSE);

/* 970910 mina  for test
printf("eye = (%f, %f, %f), to = (%f, %f, %f), theta = %d, phy = %d\n",
*/
	glutSetWindow(mapwindow);
	glutPostRedisplay();
}


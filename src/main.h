/*
	WING for Win95/NT
	97/09/02, mina
*/

#ifndef MAIN_H
#define	MAIN_H

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
#include	<GL/glu.h>
*/
#include	<GLUT/glut.h>

#include	"mapitem.h"


/* global variables */
extern int	mapwindow;
extern float	eye_x, eye_y, eye_z;
extern float	to_x, to_y, to_z;
extern int	phy, theta;
extern float	lookAt[3];
extern float	viewrange;
extern int	wiremode, texturemode, mapmode, padmode;
extern int	antialiasmode;
extern int	fogmode;
extern int	penstatusmode, compassmode;
extern int	snapshotmode, snapflame;	/* mina 951121 */
/* extern Matrix	idmat; */
extern GLfloat	idmat[4][4];

extern float	weight[10];

/* prototypes */
extern void	main(int, char *[]);
extern void	options(int, char *[]);
extern void	usage(char *);
extern void	showKeyBinds();
extern void	setQdevices(void);
/*
extern void	doCommand(long);
*/
extern int	inMapWindow(int, int);
extern int	handleMap(int, int);
extern int	parsePicked();

extern void	mapKeyFunc(unsigned char, int, int);
extern void	mapSpecialFunc(int, int, int);
extern void	mapMouseFunc(int, int, int, int);
extern void	mapMotionFunc(int, int);

#endif


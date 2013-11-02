#ifndef	DRAW_H
#define	DRAW_H

#include	"geom.h"

#define	PI	3.14159265358979323846264338327950288419

/* shading */
#define		MAX_MATERIALS	7
#define		MAX_LIGHTS	1
#define		MAX_MODELS	1
#define		MAX_TEXPROPS	2
#define		MAX_TEVPROPS	1


/* global */
extern float	mod_prop1[MAX_MODELS][10];
extern float	texprops[MAX_TEXPROPS][7];
extern float	tevprops[MAX_TEVPROPS][2];
extern int	backcolor;
extern float	cursor_vert[3][3];
extern float	cursor_nvec[3];


/* prototypes */
extern int	openMapWindow(char *);
extern void	defLights(void);
extern int	readTexture(char *, int);
extern void	drawScene(void);
extern void	drawMapTexture();
extern void	drawMapPlane();
extern void	drawMapWire();
extern void	drawDigitalMap();
extern void	changeView(int);
extern void	drawPenCursor(int, int, int, int);
extern int	checkSelection();
extern void	drawPart(Part *);
extern void	drawCompass();
extern void	drawMoving();
extern void	drawScope();


#endif	/* !DRAW_H */

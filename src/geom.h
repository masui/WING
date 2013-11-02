/*
	WING for Win95/NT

	97/09/04 mina
*/

/*
	$Date: 1995/02/01 13:51:03 $
	$Revision: 1.4 $
*/
#ifndef	GEOM_H
#define	GEOM_H

#include	"mapitem.h"

#define	MAX_VERTICES	16
#define	MAX_FACES	12
#define	MAX_FACEVERTICES	12

#define	MAX_PARTS	1000

typedef struct part {
	int	mat;
	int	col;
	int	num_face;
	int	hilight_face;
	float	v[MAX_VERTICES][3];
	int	f[MAX_FACES][MAX_FACEVERTICES];
	float	n[MAX_FACES][3];
	Item	*item;
} Part;

extern Part	parts[MAX_PARTS];
extern int	num_parts;
extern Part	compass;
extern Part	pencursor;

/* prototypes */
extern int	genBox(float [3], float [3], int, int, int, Item *);
extern int	genCylinder(float [3], float[3], int, int, int, int, Item *);
extern int	genPyramid(float [3], float[3], int, int, int, int, Item *);
extern void	setPartNVec(Part *);

#endif	/* !GEOM_H */

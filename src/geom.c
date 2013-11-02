/*
	WING for Win95/NT

	97/09/04 mina
*/

/*
	$Date: 1995/02/01 13:51:08 $
	$Revision: 1.5 $
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/geom.c,v 1.5 1995/02/01 13:51:08 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	"geom.h"
#include	"calc.h"
#include	"math.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif

/* global variables */
Part	parts[MAX_PARTS];
int	num_parts = 0;

Part	compass = {
	1, 0xffffff, 5, -1,
	{ /* vertex */
	{0.1, 0.0, 0.0}, {-0.1, -0.02, -0.02}, {-0.1, -0.02, 0.02}, 
	{-0.1, 0.02, 0.02}, {-0.1, 0.02, -0.02},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	},
	{ /* face */
	{0, 2, 1, -1, 0, 0, 0, 0},
	{0, 3, 2, -1, 0, 0, 0, 0},
	{0, 4, 3, -1, 0, 0, 0, 0},
	{0, 1, 4, -1, 0, 0, 0, 0},
	{1, 2, 3, 4, -1, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	},
	{ /* normal vector */
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	},
	NULL
};
Part	pencursor = {
	1, 0xffffff, 5, -1,
	{ /* vertex */
	{0.0, 0.0, 0.0}, {-0.02, -0.02, 0.1}, {0.02, -0.02, 0.1}, 
	{0.02, 0.02, 0.1}, {-0.02, 0.02, 0.1},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	},
	{ /* face */
	{0, 2, 1, -1, 0, 0, 0, 0},
	{0, 3, 2, -1, 0, 0, 0, 0},
	{0, 4, 3, -1, 0, 0, 0, 0},
	{0, 1, 4, -1, 0, 0, 0, 0},
	{1, 2, 3, 4, -1, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	{-1, 0, 0, 0, 0, 0, 0, 0},
	},
	{ /* normal vector */
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
	},
	NULL
};



int genBox(float orig[3], float size[3], int mat, int col, int id, Item *item)
{
	if ((id < 0) || (MAX_PARTS < id)) {
		fprintf(stderr, "genBox: id [%d] is out of range\n", id);
		return(FALSE);
	}

	parts[id].mat = mat;
	parts[id].col = col;
	parts[id].num_face = 6;
	parts[id].item = item;
	parts[id].hilight_face = 5;

/* make vertices */
	parts[id].v[0][0] = orig[0] - size[0] / 2.0;
	parts[id].v[0][1] = orig[1] - size[1] / 2.0;
	parts[id].v[0][2] = orig[2] - size[2] / 2.0;

	parts[id].v[1][0] = orig[0] + size[0] / 2.0;
	parts[id].v[1][1] = orig[1] - size[1] / 2.0;
	parts[id].v[1][2] = orig[2] - size[2] / 2.0;

	parts[id].v[2][0] = orig[0] + size[0] / 2.0;
	parts[id].v[2][1] = orig[1] + size[1] / 2.0;
	parts[id].v[2][2] = orig[2] - size[2] / 2.0;

	parts[id].v[3][0] = orig[0] - size[0] / 2.0;
	parts[id].v[3][1] = orig[1] + size[1] / 2.0;
	parts[id].v[3][2] = orig[2] - size[2] / 2.0;

	parts[id].v[4][0] = orig[0] - size[0] / 2.0;
	parts[id].v[4][1] = orig[1] - size[1] / 2.0;
	parts[id].v[4][2] = orig[2] + size[2] / 2.0;

	parts[id].v[5][0] = orig[0] + size[0] / 2.0;
	parts[id].v[5][1] = orig[1] - size[1] / 2.0;
	parts[id].v[5][2] = orig[2] + size[2] / 2.0;

	parts[id].v[6][0] = orig[0] + size[0] / 2.0;
	parts[id].v[6][1] = orig[1] + size[1] / 2.0;
	parts[id].v[6][2] = orig[2] + size[2] / 2.0;

	parts[id].v[7][0] = orig[0] - size[0] / 2.0;
	parts[id].v[7][1] = orig[1] + size[1] / 2.0;
	parts[id].v[7][2] = orig[2] + size[2] / 2.0;

/* make faces */
	parts[id].f[0][0] = 1;
	parts[id].f[0][1] = 0;
	parts[id].f[0][2] = 3;
	parts[id].f[0][3] = 2;
	parts[id].f[0][4] = -1;

	parts[id].f[1][0] = 5;
	parts[id].f[1][1] = 1;
	parts[id].f[1][2] = 2;
	parts[id].f[1][3] = 6;
	parts[id].f[1][4] = -1;

	parts[id].f[2][0] = 4;
	parts[id].f[2][1] = 5;
	parts[id].f[2][2] = 6;
	parts[id].f[2][3] = 7;
	parts[id].f[2][4] = -1;

	parts[id].f[3][0] = 0;
	parts[id].f[3][1] = 4;
	parts[id].f[3][2] = 7;
	parts[id].f[3][3] = 3;
	parts[id].f[3][4] = -1;

	parts[id].f[4][0] = 0;
	parts[id].f[4][1] = 1;
	parts[id].f[4][2] = 5;
	parts[id].f[4][3] = 4;
	parts[id].f[4][4] = -1;

	parts[id].f[5][0] = 7;
	parts[id].f[5][1] = 6;
	parts[id].f[5][2] = 2;
	parts[id].f[5][3] = 3;
	parts[id].f[5][4] = -1;

/* make normal vectors */
	parts[id].n[0][0] =  0.0;
	parts[id].n[0][1] =  0.0;
	parts[id].n[0][2] = -1.0;

	parts[id].n[1][0] =  1.0;
	parts[id].n[1][1] =  0.0;
	parts[id].n[1][2] =  0.0;

	parts[id].n[2][0] =  0.0;
	parts[id].n[2][1] =  0.0;
	parts[id].n[2][2] =  1.0;

	parts[id].n[3][0] = -1.0;
	parts[id].n[3][1] =  0.0;
	parts[id].n[3][2] =  0.0;

	parts[id].n[4][0] =  0.0;
	parts[id].n[4][1] = -1.0;
	parts[id].n[4][2] =  0.0;

	parts[id].n[5][0] =  0.0;
	parts[id].n[5][1] =  1.0;
	parts[id].n[5][2] =  0.0;

	return(TRUE);
}

int genCylinder(float orig[3], float size[3], int ngon, int mat, int col, int id, Item *item)
{
/*
	size[0] : lower radius
	size[1] : upper radius
	size[2] : hight
*/
int	i;
double	angle;

	if ((id < 0) || (MAX_PARTS < id)) {
		fprintf(stderr, "genBox: id [%d] is out of range\n", id);
		return(FALSE);
	}
	if (ngon < 3 || MAX_FACES < ngon + 2 || MAX_FACEVERTICES < ngon + 2 || MAX_VERTICES < ngon * 2)  return(FALSE);

	parts[id].mat = mat;
	parts[id].col = col;
	parts[id].num_face = ngon + 2;
	parts[id].hilight_face = ngon + 1;
	parts[id].item = item;

/* make vertices */
	for (i = 0; i < ngon; i++) {
		angle = 2.0 * PI * (double)i / (double)ngon;
		parts[id].v[i][0] = orig[0] + size[0] * cos(angle);
		parts[id].v[i][1] = orig[1];
		parts[id].v[i][2] = orig[2] + size[0] * sin(angle);
		parts[id].v[i + ngon][0] = orig[0] + size[1] * cos(angle);
		parts[id].v[i + ngon][1] = orig[1] + size[2];
		parts[id].v[i + ngon][2] = orig[2] + size[1] * sin(angle);
	}

/* make faces */
	for (i = 0; i < ngon; i++) {
		parts[id].f[i + 1][0] = i + 1 < ngon ? i + 1 : 0;
		parts[id].f[i + 1][1] = i;
		parts[id].f[i + 1][2] = i + ngon;
		parts[id].f[i + 1][3] = i + ngon + 1 < ngon * 2 ? i + ngon + 1 : ngon;
		parts[id].f[i + 1][4] = -1;

		parts[id].f[0][i] = i;
		parts[id].f[ngon + 1][i] = ngon * 2 - i - 1;
	}
	parts[id].f[0][ngon] = -1;
	parts[id].f[ngon + 1][ngon] = -1;

/* set normal vectors */
	setPartNVec(&parts[id]);
}

int genPyramid(float orig[3], float size[3], int ngon, int mat, int col, int id, Item *item)
{
/*
	size[0] : radius
	size[1] : hight
*/
int	i;
double	angle;

	if ((id < 0) || (MAX_PARTS < id)) {
		fprintf(stderr, "genBox: id [%d] is out of range\n", id);
		return(FALSE);
	}
	if (ngon < 3 || MAX_FACES < ngon + 1 || MAX_FACEVERTICES < ngon + 2)  return(FALSE);

	parts[id].mat = mat;
	parts[id].col = col;
	parts[id].num_face = ngon + 1;
	parts[id].hilight_face = 0;
	parts[id].item = item;

/* make vertices */
	for (i = 0; i < ngon; i++) {
		angle = 2.0 * PI * (double)i / (double)ngon;
		parts[id].v[i][0] = orig[0] + size[0] * cos(angle);
		parts[id].v[i][1] = orig[1];
		parts[id].v[i][2] = orig[2] + size[0] * sin(angle);
	}
	parts[id].v[ngon][0] = orig[0];
	parts[id].v[ngon][1] = orig[1] + size[1];
	parts[id].v[ngon][2] = orig[2];

/* make faces */
	for (i = 0; i < ngon; i++) {
		parts[id].f[i + 1][0] = i + 1 < ngon ? i + 1 : 0;
		parts[id].f[i + 1][1] = i;
		parts[id].f[i + 1][2] = ngon;
		parts[id].f[i + 1][3] = -1;

		parts[id].f[0][i] = ngon - i - 1;
	}
	parts[id].f[0][ngon] = -1;

/* set normal vectors */
	setPartNVec(&parts[id]);
}

void setPartNVec(Part *p)
{
int	i;

	for (i = 0; i < p->num_face; i++) {
		NormalVec(p->v[p->f[i][0]], p->v[p->f[i][1]], p->v[p->f[i][2]], p->n[i]);
	}
}


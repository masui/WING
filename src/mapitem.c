/*
	WING for Win95/NT

	970907 mina
*/

/*
	$Date: 1995/01/31 16:25:05 $
	$Revision: 1.4 $
*/
static char rcsid[] = "$Header: /usr/people/masui/WING/wing/RCS/mapitem.c,v 1.4 1995/01/31 16:25:05 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	<math.h>
#include	"mapitem.h"
#include	"geom.h"
#include	"main.h"
#include	"mapData.h"
#include	"move.h"
#include	"draw.h"
#include	"calc.h"
#include	"showGuide.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif


/* global variables */
MoveList	*moveList = NULL;
MoveList	*curtList = NULL;

void dispItem(Item *ip)
{
float	pos[3];
float	size[3] = {0.02, 0.02, 0.02};
unsigned long col;
unsigned char r, g, b;

	if(ip->locx == 0.0 && ip->locy == 0.0) return;

	normalizeCoord(ip->locy, ip->locx, &(pos[0]), &(pos[2]));
	calcAltitude(pos);
/* Feb.20, 1995 mina added */
	/* d‚Ý‚É‰ž‚¶‚Ä—§•û‘Ì‚Ì‘å‚«‚³‚ð•Ï‚¦‚é
	size[0] = size[1] = size[2] = ip->weight * 0.05;
	*/

	r = ip->r < 255 ? ip->r : 255;
	g = ip->g < 255 ? ip->g : 255;
	b = ip->b < 255 ? ip->b : 255;
	col = 0xff << 24 | b << 16 | g << 8 | r;

	genBox(pos, size, 0, col, num_parts, ip);
	num_parts++;
}

void clearAllItems()
{
	num_parts = 0;
}

void clearAllPath()
{
MoveList	*nextList;
	curtList = moveList;
	while (curtList) {
		nextList = curtList->nextitem;
		free(curtList);
		curtList = nextList;
	}
	curtList = NULL;
	moveList = NULL;
}

void movetoItem(Item *ip)
{
float	moveto[2];

	normalizeCoord(ip->locy, ip->locx, &(moveto[0]), &(moveto[1]));
	moveTo(moveto);
	addMoveList(ip);
}

void addMoveList(Item *ip)
{
MoveList	*newList;

	if (!moveList) {
		moveList = (MoveList *)malloc(sizeof(MoveList));
		moveList->item = ip;
		moveList->previtem = NULL;
		moveList->nextitem = NULL;
		curtList = moveList;
		return;
	} else {
		if (!curtList) {
			fprintf(stderr, "the current position has not been set\n");
			return;
		}
		newList = (MoveList *)malloc(sizeof(MoveList));
		newList->item = ip;
		newList->previtem = curtList;
		newList->nextitem = curtList->nextitem;
		if (curtList->nextitem)  curtList->nextitem->previtem = newList;
		curtList->nextitem = newList;
		curtList = newList;
		return;
	}
}

void moveNextItem()
{
float	moveto[2];

	if (!curtList || !curtList->nextitem)  return;

	curtList = curtList->nextitem;
	normalizeCoord(curtList->item->locx, curtList->item->locy, &(moveto[0]), &(moveto[1]));
	moveTo(moveto);
}

void movePrevItem()
{
float	moveto[2];

float	toPhy;
float	diffPhy;

	if (!curtList || !curtList->previtem)  return;

	curtList = curtList->previtem;

	normalizeCoord(curtList->item->locx, curtList->item->locy, &(moveto[0]), &(moveto[1]));
	moveTo(moveto);
}

void delCurtItem()
{
MoveList	*newCurtList = NULL;

	if (!curtList)  return;

	if (curtList->nextitem) {
		curtList->nextitem->previtem = curtList->previtem;
		newCurtList = curtList->nextitem;
	}
	if (curtList->previtem) {
		curtList->previtem->nextitem = curtList->nextitem;
		newCurtList = curtList->previtem;
	} else {
		moveList = curtList->nextitem;
	}

	if (!(curtList->previtem && curtList->nextitem))  moveList = NULL;

	free(curtList);
	curtList = newCurtList;
}

void moveCurtItem()
{
float	moveto[2];

	if (!curtList)  return;

	normalizeCoord(curtList->item->locx, curtList->item->locy, &(moveto[0]), &(moveto[1]));
	moveTo(moveto);
}


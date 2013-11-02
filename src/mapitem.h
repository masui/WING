/*
	WING for Win95/NT

	970904 mina
*/

#ifndef MAPITEM_H
#define	MAPITEM_H

#include	"dataentry.h"

/* these are defined in dataentry.h
struct _ItemList;

typedef struct _Item {
	float	locx, locy;
	char	*name;
	char	*info;
	char	*weight;
	struct _ItemList	*super;
	struct _ItemList	*sub;
	struct _ItemList	*relateditem;
} Item;

typedef struct _ItemList {
	struct _ItemList	*nextitem;
	Item	*item;
} ItemList;
*/

typedef struct _moveList {
	Item	*item;
	struct _moveList	*previtem;
	struct _moveList	*nextitem;
} MoveList;


/* global variables */
extern MoveList	*moveList;
extern MoveList	*curtList;

/* prototypes */
extern void	dispItem(Item *);
/*
extern void	dispItem(int);
*/
extern void	clearAllItems();
extern void	clearAllPath();

extern void	movetoItem(Item *);
extern void	addMoveList(Item *);
/*
extern void	movetoItem(int);
extern void	addMoveList(int);
*/
extern void	moveNextItem();
extern void	movePrevItem();
extern void	delCurtItem();
extern void	moveCurtItem();

#endif

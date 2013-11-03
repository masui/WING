/*
	WING for Win95/NT

	97/09/04 mina
*/

/*
	$Date: 1995/02/02 07:33:45 $
	$Revision: 1.6 $
*/
#ifndef	_INDEX_H_
#define	_INDEX_H_

#include "dataentry.h"

#ifndef PINDEX
#define PINDEX "data/pindex"
#endif

extern int	indexwin;

/* prototypes */
extern void	initIndex();
extern void	resetIndex();
extern Item*	indexSelect(int, int);
extern void	indexDisplay(void);

/* 970904 */
extern void	indexKeyFunc(unsigned char, int, int);
extern void	indexMouseFunc(int, int, int, int);
extern void	indexMotionFunc(int, int);

#endif


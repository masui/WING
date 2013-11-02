/*
	WING for Win95/NT

	970904 mina
*/

/*
	$Date: 1995/01/31 16:25:54 $
	$Revision: 1.3 $
*/
#ifndef MOVE_H
#define	MOVE_H

#include	"mapData.h"

/* grobal variables */
extern float	lspeed;
extern float	aspeed;
extern int	moving;

/* prototypes */
extern void	moveTo(float [2]);
extern void	zoomTo(float);
extern void	moveAlongPath(Path *, Mark *);	/* old */
extern void	moveToMark(Mark *, Mark *);	/* old */
extern void	rotateTo(float [2]);		/* old */
extern void	translateTo(float [2]);		/* old */

#endif	/* !MOVE_H */

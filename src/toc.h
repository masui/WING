/*
	$Date: 1995/01/31 10:06:09 $
	$Revision: 1.1 $
*/
#ifndef	_RECTS_H_
#define	_RECTS_H_

extern int	tocwin;

/* prototypes */
extern void	initTOC();
extern void	resetTOC();
extern void	TOCDisplay(void);
extern int	inTOCWindow(int, int);
extern void	handleTOC(int, int);

extern void	tocKeyFunc(unsigned char, int, int);
extern void	tocMouseFunc(int, int, int, int);
extern void	tocMotionFunc(int, int);

#endif

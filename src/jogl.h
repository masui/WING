/*
	jogl.h: Japanese font library for OpenGL

	similar to jgl.h
*/

#ifndef _JOGL_H_
#define	_JOGL_H_

#define	STROKE	0
#define	MINCHO	1
#define	GOTHIC	2	/* not GOSIKU */
#define	MARUGO	3

#define	LINE	0
#define	POLYGON	1

extern int jcreate(unsigned char *, int, int, int);
extern void jdelete(int);
extern void jdraw(int, GLdouble [3], GLdouble[3], GLdouble, GLdouble, GLdouble);

#endif


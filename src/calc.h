/*
	WING for Win32/NT

	97/09/02, mina
*/

#ifndef CALC_H
#define	CALC_H

/*
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#ifndef PI
#define	PI	3.1415926535897932
#endif
#ifndef EPSI
#define	EPSI	1.0e-8
#endif


/* prototypes */
extern int	NormalVec(float [] , float [], float [], float []);
extern void	InitMatrix(GLfloat [4][4]);
extern void	m4444(GLfloat [4][4], GLfloat [4][4]);
extern void	m3144(float [3], GLfloat [4][4]);
extern void	m3333t(float[3][3], float[3][3]);
extern void	PrintMatrix(GLfloat [4][4]);
extern float	f_distance(float [3], float [3]);

#endif	/* !CALC_H */


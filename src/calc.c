/*
	WING for Win95/NT

	97/09/02 mina
*/

/********************************************************************

calc.c

This file contains the functions for calcuration some values

written by Mina

********************************************************************/

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>
#include	<stdio.h>
#include	<math.h>
#include	"calc.h"

#ifndef TRUE
#define TRUE	(0 == 0)
#endif
#ifndef FALSE
#define FALSE	(0 != 0)
#endif



/********************************************************************
[module]	: NormalVec
[function]	: calcurate normal vector of given plane

[return value]	: TRUE : normally end
		  FALSE: invalid data

[argument] :	TYPE		NAME	I/O	COMMENT
		float		v1[3]	I	vertex 1
		float		v2[3]	I	vertex 2
		float		v3[3]	I	vertex 3
		float		nv[3]	O	normal vector

[comment]	: v1, v2 and v3 are in the left turn
		  the normal vector is normalized (the length equals 1).

[date]		: 
[creator]	: Mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
int NormalVec(float v1[3], float v2[3], float v3[3], float nv[3])
{
	float	a[3];	/* vector v2->v3 */
	float	b[3];	/* vector v2->v1 */
	float	len;

	a[0] = v3[0] - v2[0];
	a[1] = v3[1] - v2[1];
	a[2] = v3[2] - v2[2];
	b[0] = v1[0] - v2[0];
	b[1] = v1[1] - v2[1];
	b[2] = v1[2] - v2[2];

	nv[0] = a[1] * b[2] - a[2] * b[1];
	nv[1] = a[2] * b[0] - a[0] * b[2];
	nv[2] = a[0] * b[1] - a[1] * b[0];

	len = (float)sqrt((double)(nv[0] * nv[0] + nv[1] * nv[1] + nv[2] * nv[2]));
	if (len < EPSI) {
		fprintf(stderr, "NormalVec: the given points are on a point or a line?\n");
		return(FALSE);
	}

/* normalize the normal vector */
	nv[0] /= len;
	nv[1] /= len;
	nv[2] /= len;

	return(TRUE);
}


/********************************************************************
[module]	: InitMatrix
[function]	: initialize the given matrix with a unit matrix

[return value]	: void

[argument] :	TYPE		NAME	I/O	COMMENT
		GLfloat [4][4]		mat	I/O

[comment]	:

[date]		: 
[creator]	: Mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
void InitMatrix(GLfloat mat[4][4])
{
	mat[0][0] = 1.;
	mat[0][1] = 0.;
	mat[0][2] = 0.;
	mat[0][3] = 0.;
	mat[1][0] = 0.;
	mat[1][1] = 1.;
	mat[1][2] = 0.;
	mat[1][3] = 0.;
	mat[2][0] = 0.;
	mat[2][1] = 0.;
	mat[2][2] = 1.;
	mat[2][3] = 0.;
	mat[3][0] = 0.;
	mat[3][1] = 0.;
	mat[3][2] = 0.;
	mat[3][3] = 1.;
}


/********************************************************************
[module]	: m4444
[function]	: multiply a 4x4 matrix A by B

[return value]	: void

[argument] :	TYPE		NAME	I/O	COMMENT
		GLfloat [4][4]	A	I/O
		GLfloat [4][4]	B	I

[comment]	: the multiplication order is A x B

[date]		: 
[creator]	: Mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
void m4444(GLfloat A[4][4], GLfloat B[4][4])
{
        float   a[4][4];
        int     i, j;

	/* backup matrix A */
        for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
                a[i][j] = A[i][j];
        }
        }

	/* multiply */
        A[0][0] = a[0][0] * B[0][0] + a[0][1] * B[1][0] + a[0][2] * B[2][0] + a[0][3] * B[3][0];
        A[0][1] = a[0][0] * B[0][1] + a[0][1] * B[1][1] + a[0][2] * B[2][1] + a[0][3] * B[3][1];
        A[0][2] = a[0][0] * B[0][2] + a[0][1] * B[1][2] + a[0][2] * B[2][2] + a[0][3] * B[3][2];
        A[0][3] = a[0][0] * B[0][3] + a[0][1] * B[1][3] + a[0][2] * B[2][3] + a[0][3] * B[3][3];
        A[1][0] = a[1][0] * B[0][0] + a[1][1] * B[1][0] + a[1][2] * B[2][0] + a[1][3] * B[3][0];
        A[1][1] = a[1][0] * B[0][1] + a[1][1] * B[1][1] + a[1][2] * B[2][1] + a[1][3] * B[3][1];
        A[1][2] = a[1][0] * B[0][2] + a[1][1] * B[1][2] + a[1][2] * B[2][2] + a[1][3] * B[3][2];
        A[1][3] = a[1][0] * B[0][3] + a[1][1] * B[1][3] + a[1][2] * B[2][3] + a[1][3] * B[3][3];
        A[2][0] = a[2][0] * B[0][0] + a[2][1] * B[1][0] + a[2][2] * B[2][0] + a[2][3] * B[3][0];
        A[2][1] = a[2][0] * B[0][1] + a[2][1] * B[1][1] + a[2][2] * B[2][1] + a[2][3] * B[3][1];
        A[2][2] = a[2][0] * B[0][2] + a[2][1] * B[1][2] + a[2][2] * B[2][2] + a[2][3] * B[3][2];
        A[2][3] = a[2][0] * B[0][3] + a[2][1] * B[1][3] + a[2][2] * B[2][3] + a[2][3] * B[3][3];
        A[3][0] = a[3][0] * B[0][0] + a[3][1] * B[1][0] + a[3][2] * B[2][0] + a[3][3] * B[3][0];
        A[3][1] = a[3][0] * B[0][1] + a[3][1] * B[1][1] + a[3][2] * B[2][1] + a[3][3] * B[3][1];
        A[3][2] = a[3][0] * B[0][2] + a[3][1] * B[1][2] + a[3][2] * B[2][2] + a[3][3] * B[3][2];
        A[3][3] = a[3][0] * B[0][3] + a[3][1] * B[1][3] + a[3][2] * B[2][3] + a[3][3] * B[3][3];
}



/********************************************************************
[module]	: PrintMatrix
[function]	: print a 4x4 matrix

[return value]	: void

[argument] :	TYPE		NAME	I/O	COMMENT
		GLfloat[4][4]	mat	I

[comment]	: this function only print out the value of elements

[date]		: 
[creator]	: Mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
void PrintMatrix(GLfloat mat[4][4])
{
        printf("matrix = {%f, %f, %f, %f}\n", mat[0][0], mat[0][1], mat[0][2], mat[0][3]);
        printf("         {%f, %f, %f, %f}\n", mat[1][0], mat[1][1], mat[1][2], mat[1][3]);
        printf("         {%f, %f, %f, %f}\n", mat[2][0], mat[2][1], mat[2][2], mat[2][3]);
        printf("         {%f, %f, %f, %f}\n", mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
	printf("\n");
}



/********************************************************************
[module]	: m3144
[function]	: multiply a 3x1 matrix by a 4x4 matrix

[return value]	: void

[argument] :	TYPE		NAME	I/O	COMMENT
		float		pnt[3]	I/O	coodinate
		GLfloat [4][4]	m	I	transformation matrix

[comment]	: assume the 3x1 matrix as a 4x1 homogenous coordinate matrix

[date]		: 
[creator]	: Mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
void m3144(float pnt[3], GLfloat m[4][4])
{
        float   x, y, z;

        x = pnt[0] * m[0][0] + pnt[1] * m[1][0] + pnt[2] * m[2][0] + m[3][0];
        y = pnt[0] * m[0][1] + pnt[1] * m[1][1] + pnt[2] * m[2][1] + m[3][1];
        z = pnt[0] * m[0][2] + pnt[1] * m[1][2] + pnt[2] * m[2][2] + m[3][2];

        pnt[0] = x;
        pnt[1] = y;
        pnt[2] = z;
}



/********************************************************************
[module]	: m3333t
[function]	: multiply a 3x3 matrix by a 3x3 transposed matrix

[return value]	: void

[argument] :	TYPE		NAME		I/O	COMMENT
		float		m1[3][3]	I/O	
		float		m2[3][3]	I	to be transposed

[comment]	: only rotation, the transposed matrix correspond with
		the inverse matrix.

[date]		: May 25, 1993
[creator]	: mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
void m3333t(float m1[3][3], float m2[3][3])
{
	int	i, j;
	float	M1[3][3];

	for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
		M1[i][j] = m1[i][j];
	}
	}

	for (i = 0; i < 3; i++) {
	for (j = 0; j < 3; j++) {
		m1[i][j] = M1[i][0] * m2[j][0] +
			   M1[i][1] * m2[j][1] +
			   M1[i][2] * m2[j][2];
	}
	}
}


/********************************************************************
[module]	: f_distance
[function]	: calcurate the distance between two points

[return value]	: the distance

[argument] :	TYPE		NAME		I/O	COMMENT
		float		p1[3]		I	
		float		p2[3]		I

[comment]	: float version

[date]		: Jan 10, 1995
[creator]	: mina

<< MODIFICATION >>
[date]		[modifier]	[comment]
********************************************************************/
float f_distance(float p1[3], float p2[3])
{
	return(sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) +
		     (p1[1] - p2[1]) * (p1[1] - p2[1]) +
		     (p1[2] - p2[2]) * (p1[2] - p2[2])));
}


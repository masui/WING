/*
** RGB Image Structure
*/

#ifndef _RGB_H
#define _RGB_H

/*
#include <GL/gl.h>
*/
#include	<GLUT/glut.h>

typedef struct _RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char *data;
} RGBImageRec;

extern RGBImageRec *rgbImageLoad(char *);

#endif

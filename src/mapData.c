/*
	WING for Win95/NT

	970904 mina
*/

/*
	$Date: 1995/02/03 14:59:05 $
	$Revision: 1.3 $
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/mapData.c,v 1.3 1995/02/03 14:59:05 masui Exp masui $";

/*
#include	<GL/glut.h>
#include	<GL/gl.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"mapData.h"
#include	"digitalMap.h"
#include	"main.h"
#include	"draw.h"
#include	"dataentry.h"
#include	"showGuide.h"

#ifndef TRUE
#define TRUE	(0 == 0)
#endif
#ifndef FALSE
#define FALSE	(0 != 0)
#endif

/*** global variables ***/
int	lower_left[2];
int	upper_right[2];
float	mapcenter[2];
float	mapsize[2];

Line	lines[MAXLINES];
Mark	marks[MAXMARKS];
Path	paths[MAXPATHS];
int	num_lines;
int	num_marks;
int	num_paths;

int	colors[MAXCOLORS] = {
	0x000000, 0x0000ff, 0x00ff00, 0x00ffff,
	0xff0000, 0xff00ff, 0xffff00, 0xffffff,
	};

int writeMapData(char *filename)
{
/* not used */
FILE	*fp;

	if (!(fp = fopen(filename, "w"))) {
		fprintf(stderr, "cannot open %s\n", filename);
		return(FALSE);
	}

	fwrite(lower_left, sizeof(int) * 2, 2, fp);
	fwrite(upper_right, sizeof(int) * 2, 2, fp);
	fwrite(&num_lines, sizeof(int), 1, fp);
	fwrite(&num_marks, sizeof(int), 1, fp);
	fwrite(&num_paths, sizeof(int), 1, fp);
	fwrite(lines, sizeof(Line), num_lines, fp);
	fwrite(marks, sizeof(Mark), num_marks, fp);
	fwrite(paths, sizeof(Path), num_paths, fp);

	fclose(fp);
}

int readMapData(char *filename)
{
/* not used */
FILE	*fp;

	if (!(fp = fopen(filename, "r"))) {
		fprintf(stderr, "cannot open %s\n", filename);
		return(FALSE);
	}

	fread(lower_left, sizeof(int) * 2, 2, fp);
	fread(upper_right, sizeof(int) * 2, 2, fp);
	fread(&num_lines, sizeof(int), 1, fp);
	fread(&num_marks, sizeof(int), 1, fp);
	fread(&num_paths, sizeof(int), 1, fp);
	fread(lines, sizeof(Line), num_lines, fp);
	fread(marks, sizeof(Mark), num_marks, fp);
	fread(paths, sizeof(Path), num_paths, fp);

	fclose(fp);

/*
lower_left[0] = 0;
lower_left[1] = 0;
upper_right[0] = 4000;
upper_right[1] = 4000;
*/

	mapsize[0] = fabs((float)lower_left[0] - (float)upper_right[0]);
	mapsize[1] = fabs((float)lower_left[1] - (float)upper_right[1]);
	/*
	mapcenter[0] = (float)(lower_left[0] + upper_right[0]) / 2.0;
	mapcenter[1] = (float)(lower_left[1] + upper_right[1]) / 2.0;
	*/
	mapcenter[0] = mapsize[0] / 2.0;
	mapcenter[1] = mapsize[1] / 2.0;
}

Path *searchSubPath(Mark *m1, Mark *m2)
{
int	i;
Mark	*e[2];
	for (i = 0; i < num_paths; i++) {
		e[0] = (paths[i].points)[0];
		e[1] = (paths[i].points)[paths[i].numpoints - 1];
		if ((e[0] == m1 && e[1] == m2) ||
		    (e[0] == m2 && e[1] == m1))
			return(&(paths[i]));
	}

	return(NULL);
}

void normalizeCoord(int ix, int iy, float *nx, float *ny)
{
	*nx = ((float)ix - mapcenter[0]) * 2.0 / mapsize[0];
	*ny = ((float)iy - mapcenter[1]) * 2.0 / mapsize[1];
}

void digitalMapCoord(float ix, float iy, float *mx, float *my)
{
	*mx = mapcenter[0] + ix * mapsize[0] / 2.0;
	*my = mapcenter[1] + iy * mapsize[1] / 2.0;
}


void calcAllAltitude()
{
int	i, j;

	for (i = 0; i < num_lines; i++) {
		for (j = 0; j < lines[i].numpoints; j++) {
			calcAltitude(lines[i].f_points[j]);
		}
	}

	for (i = 0; i < num_marks; i++)  calcAltitude(marks[i].f_location);
}

void calcAltitude(float pnt[3])
{
/* for nara only */
float	altCoord[2];
int	i, j;
float	fi, fj;

	altCoord[0] = - pnt[0] * 50.0 + 149.0;
	altCoord[1] = pnt[2] * 50.0 + 149.0;

	if (altCoord[0] < 50)  altCoord[0] = 50.0;
	if (altCoord[1] < 50)  altCoord[1] = 50.0;
	if (altCoord[0] > 198.0)  altCoord[0] = 198.0;
	if (altCoord[1] > 198.0)  altCoord[1] = 198.0;

	i = (int)(altCoord[0]);
	j = (int)(altCoord[1]);
	fi = altCoord[0] - i;
	fj = altCoord[1] - j;

	/* 
	pnt[1] = altitude[i][j] * (1.0 - fi) * (1.0 - fj) +
		 altitude[i][j + 1] * fi * (1.0 - fj) +
		 altitude[i + 1][j] * (1.0 - fi) * fj +
		 altitude[i + 1][j + 1] * fi * fj;
	*/

	i = (int)(altCoord[0]);
	j = (int)(altCoord[1]);
	fi = altCoord[0] - i;
	fj = altCoord[1] - j;
	if (fi >= 0.5)  i++;
	if (fj >= 0.5)  j++;
	if (i > 199) i = 199;
	if (j > 199) j = 199;
	pnt[1] = altitude[i][j];
/*
	pnt[1] /= 10000.0;
*/
	pnt[1] /= 12000.0; /* 山の高さの調整 */
}


void calcLookAtPoint(float pnt[3], char redrawnow)
	/* 970912 mina  ガイド画面更新のタイミングのフラグを追加 */
{
/* for Nara only */
/* 0.1 stands for the avarage altitude in Nara */
	pnt[0] = eye_x + (to_x - eye_x) * (eye_y - 0.1) / (eye_y - to_y); 
	pnt[1] = 0.1;
	pnt[2] = eye_z + (to_z - eye_z) * (eye_y - 0.1) / (eye_y - to_y); 

	viewrange = 0.01 + sqrt((eye_x - pnt[0]) * (eye_x - pnt[0]) +
			(eye_y - pnt[1]) * (eye_y - pnt[1]) +
			(eye_z - pnt[2]) * (eye_z - pnt[2])) / 5.0;

/* call other modules */
	{
	float	map_x, map_y;
	int	irange;
	float	totalWeight;

	digitalMapCoord(pnt[0], pnt[2], &map_x, &map_y);
	/* 970911 mina
	irange = viewrange * mapsize[0] / 2.0;
	setGuidePos((int)map_y, (int)map_x);
	*/
	setGuidePos((int)map_y, (int)map_x, viewrange, redrawnow);
		/* x and y are changed places each other */
	/* 970911 mina
	setGuideArea(viewrange);
	*/
	}
}

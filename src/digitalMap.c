/*
	WING for Win95/NT

	97/09/03 mina
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	"digitalMap.h"
#include	"mapData.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif


/* global variables */
float	altitude[200][200];


int readDigitalMap(char *filename)
{
FILE	*fp;
char	buf[86];
char	str[86];
int	i, j;

	if (!(fp = fopen(filename, "r"))) {
		fprintf(stderr, "cannot open %s\n", filename);
		return(FALSE);
	}

/* 1st record */
	if (!fgets(buf, 86, fp)) {
		fprintf(stderr, "data end in the 1st record?\n");
		return(FALSE);
	}
	/* check the data format */
	if (atoi(cutString(buf, str, 31, 5)) != 10000) {
		fprintf(stderr, "Isn't this map a digital map?\n");
		return(FALSE);
	}

/* 2nd record */
	if (!fgets(buf, 86, fp)) {
		fprintf(stderr, "the file end in the 2st record?\n");
		return(FALSE);
	}
	/* get map size */
	lower_left[0] = atoi(cutString(buf, str, 1, 7));
	lower_left[1] = atoi(cutString(buf, str, 8, 14));
	upper_right[0] = atoi(cutString(buf, str, 15, 21));
	upper_right[1] = atoi(cutString(buf, str, 22, 28));

	mapsize[0] = fabs((float)lower_left[0] - (float)upper_right[0]);
	mapsize[1] = fabs((float)lower_left[1] - (float)upper_right[1]);
	mapcenter[0] = mapsize[0] / 2.0;
	mapcenter[1] = mapsize[1] / 2.0;

/* unify the mapsize */
	mapsize[0] = mapsize[1] = mapsize[0] > mapsize[1] ? mapsize[1] : mapsize[0];

/* 3rd record */
	if (!fgets(buf, 86, fp)) {
		fprintf(stderr, "the file end in the 2st record?\n");
		return(FALSE);
	}

/* layors, elements, coordinates, and notes */
	while (fgets(buf, 86, fp)) {
		if (buf[0] == 'H') {
		/* layer headder */
		/* now no data stored */
		}
		if (buf[0] == 'E') {
		/* entity */
		int	category = atoi(cutString(buf, str, 3, 2));
		int	type = atoi(cutString(buf, str, 5, 2));
			if (buf[1] == '2') {
			/* line */
			int	numpoints, numrecords, p;
			ICoord	*points;	
			FCoord	*f_points;	
			numpoints = atoi(cutString(buf, str, 28, 4));
			points = (ICoord *)malloc(sizeof(ICoord) * numpoints);
			f_points = (FCoord *)malloc(sizeof(FCoord) * numpoints);
			numrecords = numpoints / 6;
			p = 0;
			/* read coordinates records */
				for (i = 0; i < numrecords; i++) {
					fgets(buf, 86, fp);
					for (j = 0; j < 6; j++) {
						/*
						points[p][0] = atoi(cutString(buf, str, 14 * j + 1, 7));
						points[p][1] = atoi(cutString(buf, str, 14 * j + 8, 7));
						normalizeCoord(points[p][0], points[p][1], &(f_points[p][0]), &(f_points[p][1]));
						*/
						points[p][0] = atoi(cutString(buf, str, 14 * j + 1, 7));
						points[p][2] = atoi(cutString(buf, str, 14 * j + 8, 7));
						normalizeCoord(points[p][0], points[p][2], &(f_points[p][0]), &(f_points[p][2]));
						p++;
					}
				}
				if (numpoints % 6) {
					fgets(buf, 86, fp);
					for (j = 0; j < numpoints % 6; j++) {
						/*
						points[p][0] = atoi(cutString(buf, str, 14 * j + 1, 7));
						points[p][1] = atoi(cutString(buf, str, 14 * j + 8, 7));
						normalizeCoord(points[p][0], points[p][1], &(f_points[p][0]), &(f_points[p][1]));
						*/
						points[p][0] = atoi(cutString(buf, str, 14 * j + 1, 7));
						points[p][2] = atoi(cutString(buf, str, 14 * j + 8, 7));
						normalizeCoord(points[p][0], points[p][2], &(f_points[p][0]), &(f_points[p][2]));
						p++;
					}
				}
			/* store data */
				if (num_lines < MAXLINES) {
					lines[num_lines].category = category;
					lines[num_lines].type = type;
					lines[num_lines].numpoints = numpoints;
					lines[num_lines].points = points;
					lines[num_lines].f_points = f_points;
					lines[num_lines].color = typeToColor(category, type);
					num_lines++;
				} else {
					fprintf(stderr, "too many lines\n");
				}
			} else if (buf[1] == '5') {
			/* marks */
				if (num_marks < MAXMARKS) {
					marks[num_marks].category = category;
					marks[num_marks].type = type;
					/*
					marks[num_marks].location[0] = atoi(cutString(buf, str, 36, 7));
					marks[num_marks].location[1] = atoi(cutString(buf, str, 43, 7));
					marks[num_marks].location[2] = atoi(cutString(buf, str, 50, 7));
					normalizeCoord(marks[num_marks].location[0], marks[num_marks].location[1], &(marks[num_marks].f_location[0]), &(marks[num_marks].f_location[1]));
					*/
					marks[num_marks].location[0] = atoi(cutString(buf, str, 36, 7));
					marks[num_marks].location[2] = atoi(cutString(buf, str, 43, 7));
					marks[num_marks].location[1] = atoi(cutString(buf, str, 50, 7));
					normalizeCoord(marks[num_marks].location[0], marks[num_marks].location[2], &(marks[num_marks].f_location[0]), &(marks[num_marks].f_location[2]));
					num_marks++;
				} else {
					fprintf(stderr, "too many marks\n");
				}
			} else if (buf[1] == '7') {
			/* note */
				/* not yet supported */
			} else {
				fprintf(stderr, "unsupported entity type\n");
			}
		}
	}

	fclose(fp);

	return(TRUE);
}

int readMeshData(char *filename)
{
FILE	*fp;
char	buf[1024];
char	str[1024];
int	i, j, k;

	if (!(fp = fopen(filename, "r"))) {
		fprintf(stderr, "cannot open %s\n", filename);
		return(FALSE);
	}

/* 1st record */
	if (!fgets(buf, 1024, fp)) {
		fprintf(stderr, "data end in the 1st record?\n");
		return(FALSE);
	}
	/* ignore the file header */

/* mesh data */
	for (i = 0; i < 200; i++) {
		fgets(buf, 1024, fp);
		k = 10;
		for (j = 0; j < 200; j++) {
			altitude[i][j] = atof(cutString(buf, str, k, 5));
			k += 5;
		}
	}

	fclose(fp);
	return(TRUE);
}


char *cutString(char *orig, char *dest, int head, int num)
{
register int	i;
	for (i = 0; i < num; i++)  dest[i] = orig[i + head - 1];
	dest[num] = '\0';

	return(dest);
}

int typeToColor(int cate, int type)
{
	switch(cate) {
	case 11: /* s­ŠE */
		switch(type) {
		case 1: /* “s“¹•{Œ§ŠE */
			return(1);
		case 3: /* “sŽsŠEE“Á•Ê‹æ‚ÌŠE */
			return(1);
		case 4: /* Žw’è“sŽs‚Ì‹æŠEE’¬‘ºŠE */
			return(1);
		case 6:	/* ’¬(’š)E‘åŽšŠE */
			return(1);
		case 7: /* ’š–ÚEŽšŠE */
			return(1);
		default:
			return(1);
		}
		break;
	case 21: /* “¹˜H */
		switch(type) {
		case 1: /* ˆê”Ê“¹˜H(’nã) */
			return(2);
		case 7: /* ˆê”Ê“¹˜H(’n‰º) */
			return(2);
		case 11: /* —L—¿“¹˜H(’nã) */
			return(6);
		case 17: /* —L—¿“¹˜H(’n‰º) */
			return(6);
		default:
			return(2);
		}
		break;
	case 23: /* “S“¹ */
		switch(type) {
		case 1: /* –{ü(’nã) */
			return(3);
		case 7: /* –{ü(’n‰º) */
			return(3);
		case 11: /* ‰w(’nã) */
			return(3);
		case 17: /* ‰w(’n‰º) */
			return(3);
		default:
			return(3);
		}
		break;
	case 35: /* Œš•¨‹L† */
		/* not yet implemented */
		return(0);
	case 51: /* …•” */
		switch(type) {
		case 0: /* …ŠUü */
			return(4);
		default:
			return(4);
		}
		break;
	case 73: /* Šî€“_ */
		/* not yet implemented */
		return(0);
	default:
		return(0);
	}
}

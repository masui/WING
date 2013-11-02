/*
	WING for Win95/NT

	97/09/03 mina
*/

#ifndef DIGITALMAP_H
#define DIGITALMAP_H

/* variables */
extern float	altitude[200][200];

/* prototypes */
extern int	readDigitalMap(char *);
extern int	readMeshData(char *);
extern char	*cutString(char *, char *, int, int);
extern int	typeToColor(int, int);

#endif	/* !DIGITALMAP_H */

/*
	WING for Win95/NT

	970904 mina
*/

/*
	$Date: 1995/01/30 17:16:02 $
	$Revision: 1.2 $
*/
#ifndef MAPDATA_H
#define	MAPDATA_H

#ifndef MAPDIR
#define MAPDIR "./data/map/"
#endif

#define	MAXLINES	10000
#define	MAXMARKS	10000
#define	MAXPATHS	3000
#define	MAXCOLORS	8

typedef int	ICoord[3];
typedef	float	FCoord[3];

typedef struct _line {
	int	category;
	int	type;
	int	subtype;	/* for extension */
	int	numpoints;
	ICoord	*points;	/* pointer to the points list */
	FCoord	*f_points;	/* normalized coordinates */
	int	color;
} Line;

typedef struct _mark {
	int	category;
	int	type;
	int	subtype;	/* for extension */
	char	*label;		/* for extension */
	int	location[3];
	float	f_location[3];
	int	color;
} Mark;

typedef struct _path {
	int		type;		/* the way to move */
	Mark		**points;	/* route */
	int		numpoints;
} Path;

extern int	lower_left[2];
extern int	upper_right[2];
extern float	mapcenter[2];
extern float	mapsize[2];

extern Line	lines[MAXLINES];
extern Mark	marks[MAXMARKS];
extern Path	paths[MAXPATHS];
extern int	num_lines;
extern int	num_marks;
extern int	num_paths;

extern int	colors[MAXCOLORS];


/* prototypes */
extern int	writeMapData(char *);		/* not used */
extern int	readMapData(char *);		/* not used */
extern Path *	searchSubPath(Mark *, Mark *);
extern void	normalizeCoord(int, int, float *, float *);
extern void	digitalMapCoord(float, float, float *, float *);
extern void	calcAllAltitude();
extern void	calcAltitude(float [3]);
extern void	calcLookAtPoint(float [3], char);

#endif	/* !MAPDATA_H */

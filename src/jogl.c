/*
	jogl: Japanese font libraly for OpenGL

		written by mina

	notes:
	1) Characters should be EUC code.
	2) This will convert 1-byte printable characters
	   into 2-byte "zenkaku" chars automatically.
   	3) Each Character has 1.0 height and width.
   	4) Polygons have not been supported yet. (->980928)
	5) Curves have not been supported yet.

	980928: Polygons are supported.
*/


/*
#include	<GL/gl.h>
#include	<GL/glu.h>
*/
#include	<GLUT/glut.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<math.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"jogl.h"


#define	MAXVERTICES	100

typedef struct {
	short code;
	short dum;
	int offset;
} INDEX;

static INDEX *index = NULL;
static int IL;

typedef struct {
	char attr;
	unsigned char x;
	unsigned char y;
} DATA;

static DATA *data = NULL;

static char *fontname[7][2] = {
	"./fonts/strvec.ixl", "./fonts/strvec.otl",	/* stroke style		*/
	"./fonts/mincho.ixl", "./fonts/mincho.otl",	/* mincho style outline */
	"./fonts/gosiku.ixl", "./fonts/gosiku.otl",	/* gosiku style outline */
	"./fonts/marugo.ixl", "./fonts/marugo.otl",	/* marugo style outline */
	"./fonts/mincho.ixp", "./fonts/mincho.plg",	/* mincho style polygon */
	"./fonts/gosiku.ixp", "./fonts/gosiku.plg",	/* gosiku style polygon */
	"./fonts/marugo.ixp", "./fonts/marugo.plg",	/* marugo style polygon */
};

static unsigned char zenkakustr[] = 
"¡¡¡ª¡É¡ô¡ð¡ó¡õ¡Ç¡Ê¡Ë¡ö¡Ü¡¤¡Ý¡¥¡¿£°£±£²£³£´£µ£¶£·£¸£¹¡§¡¨¡ã¡á¡ä¡©\
¡÷£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú¡Î¡À¡Ï¡°¡²\
¡Æ£á£â£ã£ä£å£æ£ç£è£é£ê£?ì£???ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú¡Ð¡Ã¡Ñ¡Á";

static GLUtesselator	*tobj = NULL;


void	drawOneChar(unsigned short, int);
void	errorFunc(GLenum);
#ifdef TEST
void	main(int, char *[]);
void	drawscene(void);
void	mouseFunc(int, int, int, int);
void	motionFunc(int, int);
int	button = -1;
int	ox, oy;
#endif



int jcreate(unsigned char *str, int font, int mode, int interp)
{
static int	type, lasttype = -1;
int		fd;
struct stat	st;
GLuint		listID;
int		i;
unsigned short	buf;


/* check the parameters */
	if (font < 0 || 3 < font)  return(0);
	if (mode < 0 || 1 < mode)  return(0);
	if (interp < 0 || 5 < interp)  return(0);
/* interp have not been supported yet. */

/* set font type */
	type = font;
	if (mode == POLYGON) {
		if (font != STROKE)  type += 3;
	}

/* load font data */
	if (lasttype != type) {
	/* If the font is changed, reload the font data. */
		if (index != NULL) {
			free(index);
			index = NULL;
		}
		if (data != NULL) {
			free(data);
			data = NULL;
		}

	/* read the index file */
		stat(fontname[type][0], &st);
		if ((fd = open(fontname[type][0], O_RDONLY)) == -1) {
			printf("open(%s) error.\n", fontname[type][0]);
			lasttype = -1;
			return(0);
		}
		index = (INDEX *)malloc(sizeof(INDEX) * (IL = st.st_size));
                {
                int count = 0;
                for (count = 0; count * sizeof(INDEX) < st.st_size; count++) {
                    read(fd, &(index[count]), sizeof(INDEX));
                }
                }
                /*
		read(fd, (void *)index, st.st_size);
                */
		close(fd);

	/* read the data file */
		stat(fontname[type][1], &st);
		if ((fd = open(fontname[type][1], O_RDONLY)) == -1) {
			printf("open(%s) error.\n", fontname[type][1]);
			free(index);
			index = NULL;
			lasttype = -1;
			return(0);
		}
		data = (DATA *)malloc(sizeof(DATA) * st.st_size);
                /*
		read(fd, data, st.st_size);
                */
                {
                int count = 0;
                for (count = 0; count * sizeof(DATA) < st.st_size; count++) {
                    read(fd, &(data[count]), sizeof(DATA));
                }
                }
		close(fd);

		lasttype = type;
	}

/* create a display list */
	listID = glGenLists(1);
	if (listID == 0) {
		printf("cannot generate a new list.\n");
		return(0);
	}

/* prepare the tesseration */
	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_BEGIN, glBegin);
	gluTessCallback(tobj, GLU_TESS_VERTEX, glVertex3dv);
	gluTessCallback(tobj, GLU_TESS_END, glEnd);
	gluTessCallback(tobj, GLU_TESS_ERROR, errorFunc);

/* str should be EUC code */
	glNewList(listID, GL_COMPILE);
		glPushMatrix();
		for (i = 0; str[i] != NULL; ) {
		/* convert kanji string (JIS, zenkaku only) */
			if (str[i] > 0x80) {
			/* zenkaku char */
				buf = str[i] & 0x7F;
				buf = buf * 256 + (str[i + 1] & 0x7F);
				i += 2;
			} else {
			/* hankaku char */
				if (str[i] < 0x20) {
				/* unprintable code */
					i++;
					continue;
				}
				buf = zenkakustr[(str[i] - 0x20) * 2] & 0x7F;
				buf = buf * 256 + (zenkakustr[(str[i] - 0x20) * 2 + 1] & 0x7F);
				i++;
			}
		/* draw one character */
			drawOneChar(buf, mode);
		/* for the next character */
			glTranslated(1.0, 0.0, 0.0);
		}
		glPopMatrix();
	glEndList();

	gluDeleteTess(tobj);
	tobj = NULL;

	return(listID);
}


void drawOneChar(unsigned short k, int mode)
{
int	st;	/* drawing status */
int	i;
int	iposi;
double	v[MAXVERTICES][3];
int	curtv;

	if (k == 0x2121)  return;	/* zenkaku-space */
	for (i = 0; i < IL; i++) {
		if (index[i].code == k) {
			iposi = index[i].offset / 3;
			break;
		}
		iposi = 0xffffffff;
	}
	if (iposi == 0xffffffff) {
		printf("no data for this code [%x]\n", k);
		return;
	}

	curtv = 0;
	st = 1;
	while (1) {
		v[curtv][0] = (double)(data[iposi].x) / 255.0;
		v[curtv][1] = (double)(data[iposi].y) / -255.0 + 1.0;
		v[curtv][2] = 0.0;
		switch (data[iposi].attr) {
		case 1:	/* the end of data */
			return;

		case 2:	/* the end of region */
			if (mode == POLYGON) {
				gluTessVertex(tobj, v[curtv], v[curtv]);
				gluTessEndContour(tobj);
				gluTessEndPolygon(tobj);
			} else {
				glVertex3dv(v[curtv]);
				glEnd();
			}
			curtv = 0;
			st = 1;
			break;

		default:
			if (st) {
				if (mode == POLYGON) {
					gluTessBeginPolygon(tobj, NULL);
					gluTessBeginContour(tobj);
					gluTessVertex(tobj, v[curtv], v[curtv]);
				} else {
					glBegin(GL_LINE_STRIP); 
					glVertex3dv(v[curtv]);
				}
				st = 0;
			}
			else {
				if (mode == POLYGON) {
					gluTessVertex(tobj, v[curtv], v[curtv]);
				} else {
					glVertex3dv(v[curtv]);
				}
			}
			curtv++;
			if (curtv > MAXVERTICES) {
				fprintf(stderr, "number of vertices exceeds the limit.\n");
				return;
			}
		}
		iposi++;
	}
}


void jdelete(int listID)
{
	glDeleteLists(listID, 1);
}

void jdraw(int listID, GLdouble pos[3], GLdouble rot[3], GLdouble xs, GLdouble ys, GLdouble thick)
{
	glPushMatrix();
		glTranslated(pos[0], pos[1], pos[2]);
		glRotated(rot[2], 0.0, 0.0, 1.0);
		glRotated(rot[1], 0.0, 1.0, 0.0);
		glRotated(rot[0], 1.0, 0.0, 0.0);
		glScaled(xs, ys, 1.0);
		glCallLists(1, GL_INT, &listID);
	glPopMatrix();
}

void errorFunc(GLenum errno)
{
const GLubyte *estr;
	estr = gluErrorString(errno);
	fprintf(stderr, "Tesselation error: %s\n", estr);
}



#ifdef TEST
#include	<GL/gl.h>
#include	<GL/glut.h>

double	loc[3];
double	rot[3];
double	xs, ys, thick;
int	s;
int	f = 0;
int	p = LINE;

void main(int argc, char *argv[])
{
	if (argc >= 2) {
		f = atoi(argv[1]);
	}
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("jogl test");
	glutDisplayFunc(drawscene);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 50, 0.0, 50, -50.0, 50.0);

	if (f > 3) {
		f -= 3;
		p = POLYGON;
	}
	s = jcreate("¤¤¤?Ï¥Ë¥Û¥Ø´Á»úabc@#!", f, p, 0);
	if (s == 0) {
		printf("failed to create the string\n");
		exit(0);
	}
	loc[0] = 0.0;
	loc[1] = 0.0;
	loc[2] = 0.0;
	rot[0] = 0.0;
	rot[1] = 0.0;
	rot[2] = 0.0;
	xs = 1.0;
	ys = 1.0;
	thick = 0.0;

	glutMainLoop();
}

void drawscene(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	jdraw(s, loc, rot, xs, ys, thick);
glCallList(10000);
	glutSwapBuffers();
}

void mouseFunc(int b, int s, int x, int y)
{
	if (s == GLUT_DOWN) {
		button = b;
		ox = x;
		oy = y;
	} else {
		button = -1;
	}
}

void motionFunc(int x, int y)
{
	switch(button) {
	case GLUT_LEFT_BUTTON:
		xs = pow(2.0, (double)(x - ox) / 100.0);
		ys = pow(2.0, (double)(x - ox) / 100.0);
		break;
	case GLUT_MIDDLE_BUTTON:
		loc[0] += (double)(x - ox) * 50.0 / 500.0;
		ox = x;
		loc[1] -= (double)(y - oy) * 50.0 / 500.0;
		oy = y;
		break;
	case GLUT_RIGHT_BUTTON:
		rot[1] += (double)(x - ox) * 180.0 / 500.0;
		ox = x;
		rot[0] += (double)(y - oy) * 180.0 / 500.0;
		oy = y;
		break;
	}
	glutPostRedisplay();
}


#endif	/* TEST */


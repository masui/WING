/*
	WING for Win95/NT

	97/09/04 mina
*/

/*
	$Date: 1995/02/03 15:31:03 $
	$Revision: 1.21 $

	目次ウィンドウ操作
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/index.c,v 1.21 1995/02/03 15:31:03 masui Exp masui $";

/*
#include <GL/glut.h>
#include <GL/gl.h>
*/
#include <GLUT/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "index.h"
#include "jstring.h"
#include "jogl.h"
#include "dataentry.h"
#include "sub.h"
#include "winpos.h"
#include "main.h"
#include "move.h"
#include "calcWeight.h"
#include "showGuide.h"

#ifndef TRUE
#define TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif

#define CENTERCOLUMN 40
#define MAXSCALE 14.0
#define POW 3

static int wordindex = 0;
static double centerpoint = 0.5;
static double oldcenterpoint;
static double disppoint = 0.5;
static double selectpoint;
static double scalefactor = MAXSCALE;
static int wordgap;
static double dispgap;
static int xpos = 0;
static int charheight = 16;

static char	mouseLpressed = FALSE;
static char	mouseRpressed = FALSE;

#define MAXINDEXENTRY 10000
static char *indices[MAXINDEXENTRY];
static int maxindex = 0;

#define BUFLEN 1000
static char buf[BUFLEN+1];

static char *pindexfile = PINDEX;	/* 置換インデクスファイル */

int	indexwin;

static int xorig;
static float offset;
static char	rainbow = TRUE;	/* 970916 mina  虹色背景モード */




static void readpindex()
{
	int line = 0;
	FILE *f;
	if (!(f = fopen(pindexfile,"r"))) {
		fprintf(stderr, "Can't open %s\n", pindexfile);
		exit(0);
	}
	while(fgets(buf,BUFLEN,f) && line < MAXINDEXENTRY){
		buf[strlen(buf)-1] = '\0';
		indices[line++] = salloc(buf);
	}
	if(line >= MAXINDEXENTRY){
		fprintf(stderr, "Too many index entries (max = %d)\n",
			MAXINDEXENTRY);
	}
	maxindex = line-1;
}

static pow2(int i)
{
	int pow = 1;
	for(;i;i--) pow *= 2;
	return pow;
}

void initIndex()
{
	glutInitWindowPosition(INDEXPOSX, INDEXPOSY);
	glutInitWindowSize(INDEXWIDTH, INDEXHEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	indexwin = glutCreateWindow("Index");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(indexDisplay);
	glutKeyboardFunc(indexKeyFunc);	/* 選択はスペースにする */
	glutMouseFunc(indexMouseFunc);	/* どのボタンが押されたかを見るのに必要 */
	glutMotionFunc(indexMotionFunc);	/* マウス操作処理 */

	readpindex();
	glutPostRedisplay();
}

void resetIndex()
{
	wordindex = 0;
	centerpoint = 0.5;
	disppoint = 0.5;
	scalefactor = MAXSCALE;
	xpos = 0;
	glutSetWindow(indexwin);
	glutPostRedisplay();
}

void indexKeyFunc(unsigned char key, int x, int y)
{
	if (key == ' ') {
	/* スペースキーで選択 */
		Item *ip;
		ip = indexSelect(x, y);
                if (ip) {
			if(ip->locx != 0 || ip->locy != 0){
				/* 970904 これは必要ない？
				glutSetWindow(tocwin);
				glutPostRedisplay();
				*/
				movetoItem(ip);
				zoomTo(0.27);
				zoomTo(0.3);
				zoomTo(0.27);
				zoomTo(0.3);
			} else {
			/* 地図上に無い項目（お祭りなど） */
				weights[0].item = ip;
				weights[0].weight = 1.0;
				weights[1].item = NULL;
				weights[1].weight = 0.0;
				glutSetWindow(gWid);
				glutPostRedisplay();
			}
		}
	} else if (key == 'r' || key == 'R') {
		rainbow = ~rainbow;
		glutSetWindow(indexwin);
		glutPostRedisplay();
	}
}

void indexMouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		mouseLpressed = (state == GLUT_DOWN);
	} else if (button == GLUT_RIGHT_BUTTON) {
		mouseRpressed = (state == GLUT_DOWN);
	}
	/* the middle button is not supported. */

	if (state == GLUT_DOWN) {
	/* prepare for dragging */
	int	i;
		oldcenterpoint = centerpoint;
		/* 970910 mina
		selectpoint = 1.0 - (double)(y) / (double)INDEXHEIGHT;
		*/
		selectpoint = (double)(y) / (double)INDEXHEIGHT;
		i = (int)floor((oldcenterpoint - selectpoint)/dispgap);
		offset = (oldcenterpoint - selectpoint) - i * dispgap;
		centerpoint = selectpoint + offset;
		wordindex -= wordgap * i;
		if(wordindex < 0) wordindex = 0;
		if(wordindex > maxindex) wordindex = maxindex;
		xorig = x;
	}
}

void indexMotionFunc(int x, int y)
{
int delta;
int i;

/* zoom */
	if (mouseRpressed) {
		oldcenterpoint = centerpoint;
		/* 970910 mina
		selectpoint = 1.0 - (double)(y) / (double)INDEXHEIGHT;
		*/
		selectpoint = (double)(y) / (double)INDEXHEIGHT;
		centerpoint = selectpoint + offset;
		delta = x - xorig;
		xpos += delta;
		xorig = x;
		scalefactor -= delta/20.0;
		if(scalefactor < 0.0) scalefactor = 0.0;
		if(scalefactor > MAXSCALE) scalefactor = MAXSCALE;
	}

/* translate */
	if (mouseLpressed) {
		oldcenterpoint = centerpoint;
		/* 970910 mina
		selectpoint = 1.0 - (double)(y) / (double)INDEXHEIGHT;
		*/
		selectpoint = (double)(y) / (double)INDEXHEIGHT;
		centerpoint = selectpoint + offset;
		xpos += (x - xorig);
		xorig = x;
	}

	glutSetWindow(indexwin);
	glutPostRedisplay();
}

Item *indexSelect(int x, int y)
{
/* select index item */
long	w, h;
int i;
unsigned char *word;
Item *ip = NULL;

	h = glutGet(GLUT_WINDOW_HEIGHT);
	w = glutGet(GLUT_WINDOW_WIDTH);

	/* 970910 mina
	selectpoint = 1.0 - (double)y / (double)h;
	*/
	selectpoint = (double)y / (double)h;
	i = (int)floor((disppoint-selectpoint)/dispgap);
	offset = disppoint - i * dispgap;
	centerpoint = selectpoint + offset;
	i = wordindex - wordgap * i;
	while(i < 0){ i += wordgap; }
	while(i >= maxindex){ i -= wordgap; }
	word = indices[i];
	while(*word == ' ') word++; /* 先頭の空白をスキップ */
	/*
		左マウスで選択された項目へのポインタを求める
	*/
	for(i=0; i<nitem; i++){ /* 読みの一致を調べる */
		ip = &items[i];
		if(ip->yomi && strcmp(ip->yomi,word) == 0){
			break;
		}
	}
	if(i >= nitem){ /* 今度は漢字の一致を調べる */
		for(i=0; i<nitem; i++){
			ip = &items[i];
			if(ip->name && strcmp(ip->name,word) == 0){
				break;
			}
		}
	}
	if(i >= nitem){
		fprintf(stderr,"Can't find item <%s> in items[]\n",word);
		return NULL;
	}
	else {
/*		printf("Item found: name = %s, filename = %s\n", ip->name, ip->itemname); */
		return ip;
	}
}

static float colorweight(float angle)
{
	while(angle > 360.0){ angle -= 360.0; }
	while(angle < 0.0){ angle += 360.0; }
	if(angle > 180.0) angle = 360.0 - angle;
	return angle < 60.0 ? 1.0 : angle > 120.0 ? 0.0 :
		-angle/60.0 + 2.0;
}
#define colorG(angle) (colorweight(angle) * 128.0)
#define colorB(angle) (colorweight(angle + 120.0) * 128.0)
#define colorR(angle) (colorweight(angle + 240.0) * 128.0)

void indexDisplay(void)
/*
	1画面表示する。
*/
{
double p;
int i;
int intscale;
unsigned char buf[200];
int ypos;
int v1[2],v2[2];
float angle;
int	width, height;

	glutSetWindow(indexwin);
	glClear(GL_COLOR_BUFFER_BIT);

	/* mina 20141016: use stroke font
	jsetfont(JIS16, LINE, 2);
	*/
	jsetfont(STROKE, LINE, 2);
	jfontsize((float)charheight);

	/* 970909 mina */
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	intscale = (int)scalefactor;
	wordgap = pow2(intscale);
	dispgap = pow(2.0, -POW - scalefactor + (float)intscale);

	/*
		虹色の背景を書く
	*/
/* Windows では遅いかも */
	if (rainbow) {
	ypos = (int)((float)(INDEXHEIGHT) * (1.0-centerpoint));
	for(i=0; ypos <= INDEXHEIGHT; ypos++,i--){
		angle = 360.0 * (
			(float)wordindex/(float)maxindex +
			pow(2.0, POW+scalefactor) * (float)i /
				(float)maxindex / (float)INDEXHEIGHT
			);
		if(angle < 0.0 || angle > 360.0) break;
		glColor3ub(colorR(angle), colorG(angle), colorB(angle));
		v1[0] = 0; v1[1] = ypos;
		v2[0] = INDEXWIDTH; v2[1] = ypos;
		glBegin(GL_LINES);
			glVertex2iv(v1);
			glVertex2iv(v2);
		glEnd();
	}
	glColor3ub(colorR(0.0), colorG(0.0), colorB(0.0));
	for(; ypos <= INDEXHEIGHT; ypos++){
		v1[0] = 0; v1[1] = ypos;
		v2[0] = INDEXWIDTH; v2[1] = ypos;
		glBegin(GL_LINES);
			glVertex2iv(v1);
			glVertex2iv(v2);
		glEnd();
	}

	ypos = (int)((float)(INDEXHEIGHT) * (1.0-centerpoint));
	for(i=0; ypos >= 0; ypos--,i++){
		angle = 360.0 * (
			(float)wordindex/(float)maxindex +
			pow(2.0, POW+scalefactor) * (float)i /
				(float)maxindex / (float)INDEXHEIGHT
			);
		if(angle < 0.0 || angle > 360.0) break;
		glColor3ub(colorR(angle), colorG(angle), colorB(angle));
		v1[0] = 0; v1[1] = ypos;
		v2[0] = INDEXWIDTH; v2[1] = ypos;
		glBegin(GL_LINES);
			glVertex2iv(v1);
			glVertex2iv(v2);
		glEnd();
	}
	glColor3ub(colorR(0.0), colorG(0.0), colorB(0.0));
	for(; ypos >= 0; ypos--){
		v1[0] = 0; v1[1] = ypos;
		v2[0] = INDEXWIDTH; v2[1] = ypos;
		glBegin(GL_LINES);
			glVertex2iv(v1);
			glVertex2iv(v2);
		glEnd();
	}
	} /* end of rainbow */

	/*
		表示可能なインデクスを表示する
	*/
	disppoint = p = centerpoint;
	i = wordindex;

	while(i >= 0 && p >= 0.0){
		int j;
		glColor3ub(255,  255,  0);
		strncpy(buf,indices[i],CENTERCOLUMN);
		buf[CENTERCOLUMN] = '\0';
		for(j=0;j<CENTERCOLUMN;j++){
			if(buf[j] != ' ') break;
		}
		jsdraw(buf+j,xpos+j*charheight/2 - 200,
			(float)((float)INDEXHEIGHT) * (1.0-p));
		glColor3ub(255,  255,  255);
		jsdraw(indices[i]+CENTERCOLUMN,xpos+CENTERCOLUMN*charheight/2 - 200,
			(float)((float)INDEXHEIGHT) * (1.0-p));
		i -= wordgap;
		p -= dispgap;
	}

	disppoint = p = centerpoint;
	i = wordindex;
	while(i <= maxindex && p <= 1.0){
		int j;
		glColor3ub(255,  255,  0);
		strncpy(buf,indices[i],CENTERCOLUMN);
		buf[CENTERCOLUMN] = '\0';
		for(j=0;j<CENTERCOLUMN;j++){
			if(buf[j] != ' ') break;
		}
		jsdraw(buf+j,xpos+j*charheight/2 - 200,
			(float)((float)INDEXHEIGHT) * (1.0-p));
		glColor3ub(255,  255,  255);
		jsdraw(indices[i]+CENTERCOLUMN,
			xpos+CENTERCOLUMN*charheight/2 - 200,
			(float)((float)INDEXHEIGHT) * (1.0-p));
		i += wordgap;
		p += dispgap;
	}

	glutSwapBuffers();
}


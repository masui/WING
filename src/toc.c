/*
	WING for Win95/NT

	97/09/05 mina
*/

/*
	$Date: 1995/02/02 15:30:36 $
	$Revision: 1.26 $

	Revised by GRB 01/27/95
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/toc.c,v 1.26 1995/02/02 15:30:36 masui Exp masui $";

/*
#include <GL/glut.h>
#include <GL/gl.h>
*/
#include <GLUT/glut.h>

#include <stdio.h>
#include <math.h>

#include "toc.h"
#include "sub.h"
#include "jstring.h"
#include "dataentry.h"
#include "mapitem.h" /* for movetoItem() */
#include "winpos.h"
#include "main.h"
#include "draw.h"
#include "move.h"
#include "calcWeight.h"
#include "showGuide.h"
#include "jogl.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif


static float scalefactor = 1.0;

/* ortho2()で指定する表示枠 */
static float dispx = 0.0;
static float dispy = 0.0;
static float dispw = (float)TOCWIDTH;
static float disph = (float)TOCHEIGHT;

int	tocwin;

/* 970905 mina, glut でのイベント処理用 */
static int	mouseLpressed = FALSE;
static int	mouseRpressed = FALSE;
static int	oldx, oldy;
static float	transX, transY;


/* prototypes */
int		visible(TOC *);
void	selecttree(Item *);



/*
	選択されたことが目に見える目次エントリが有る場合は
	関連する項目のみ検索する。

*/
void initTOC()
{
	glutInitWindowPosition(TOCPOSX, TOCPOSY);
	glutInitWindowSize(TOCWIDTH, TOCHEIGHT);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	tocwin = glutCreateWindow("categories");

	glClearColor(0.0, 40.0 / 256.0, 0.0, 0.0);

	glutDisplayFunc(TOCDisplay);
	glutKeyboardFunc(tocKeyFunc);
	glutMouseFunc(tocMouseFunc);
	glutMotionFunc(tocMotionFunc);
}

void resetTOC()
{
	scalefactor = 1.0;
	dispx = 0.0;
	dispy = 0.0;
	dispw = (float)TOCWIDTH;
	disph = (float)TOCHEIGHT;
	glutSetWindow(tocwin);
	glutPostRedisplay();
}

void tocKeyFunc(unsigned char key, int x, int y) {
int	i;

	if (key == ' ') {
	/* 項目選択 */
	float selx, sely;
		selx = dispx + dispw*(float)x/(float)TOCWIDTH;
		/* 970910 mina
		sely = dispy + disph*(float)y/(float)TOCHEIGHT;
		*/
		sely = dispy + disph* (1.0 - (float)y/(float)TOCHEIGHT);
		for(i=0; i<ntoc; i++){
			TOC *p;
			p = &tocs[i];
			if(selx > p->x && sely > p->y &&
			   selx < p->x + p->width &&
			   sely < p->y + p->height &&
			   visible(p)) {
				p->selected = ! p->selected;
				TOCDisplay();
				if(p->selected){
					if(p->item && p->item->sub == NULL){
						if(p->item->locx != 0 || p->item->locy != 0){
							glutSetWindow(tocwin);
							glutPostRedisplay();
							movetoItem(p->item);
							zoomTo(0.27);
							zoomTo(0.3);
							zoomTo(0.27);
							zoomTo(0.3);
						}
						else {
							calcWeight(0.0,0.0,0.0);
							glutSetWindow(gWid);
							glutPostRedisplay();
							break;
						}
					}
				}
				break;
			}
		}
		calcLookAtPoint(lookAt, FALSE);
		glutSetWindow(mapwindow);
		glutPostRedisplay();
        }
}

void tocMouseFunc(int button, int stat, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		mouseLpressed = (stat == GLUT_DOWN);
	} else if (button == GLUT_RIGHT_BUTTON) {
		mouseRpressed = (stat == GLUT_DOWN);
	}

	oldx = x;
	oldy = y;
}

void tocMotionFunc(int x, int y)
{
int i;

	/* ドラッグ */
	if (mouseLpressed) {
		transX = (x - oldx)/scalefactor;
		transY = (oldy - y)/scalefactor;
		dispx -= transX;
		dispy -= transY;
		oldx = x;
		oldy = y;
	}

	/* ズーム */
	if (mouseRpressed) {
	float zoomfactor;
		zoomfactor = (float)(x - oldx) / 300.0;
		if (scalefactor * (1.0 + zoomfactor) > 0.1) {
			scalefactor *= (1.0 + zoomfactor);
			dispw = (float)TOCWIDTH / scalefactor;
			disph = (float)TOCHEIGHT / scalefactor;
			dispx += dispw * 0.5 * zoomfactor;
			dispy += disph * 0.5 * zoomfactor;
		}
		oldx = x;
	}

	glutSetWindow(tocwin);
	glutPostRedisplay();
}

int visible(TOC *p)
{
	return p->height * scalefactor > 20.0 &&
		p->x + p->width >= dispx &&
		p->x <= dispx + dispw &&
		p->y + p->height >= dispy &&
		p->y <= dispy + disph ;
}

void TOCDisplay(void)
{
	int i, j, grey; /* GRB 01/27/95 */
	TOC *p;
	float size,dispsize,level;
	int tocselected = 0;
	float v[2];  /* GRB 01/27/95 */
	float boxInX, boxInY, boxDec, fcnt;  /* GRB 01/27/95 */

	glutSetWindow(tocwin);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(dispx,  dispx + dispw,  dispy,  dispy + disph);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glTranslatef(transX, transY, 0.0);

	jsetfont(STROKE, LINE, 2);

	/*
		選択されている項目を(毎回)チェックする
	*/
	for(i=0; i<nitem; i++){
		items[i].selected = 0;
	}

	for(i=ntoc-1; i>=0; i--){
	        p = &tocs[i];
		if(visible(p)){
			boxInX = p->width/10;
			boxInY = p->height/10;

			boxDec = boxInY/5;
			j=1;
			for(fcnt=boxInY;fcnt>=boxDec;fcnt-=boxDec) {
			  grey=100-j*20;
			  if(grey<0)
			    grey=0;
			  glColor3ub(grey,grey,grey);
			  glRectf(p->x+(2.5*boxDec)*j,  p->y+boxDec*j,  p->x + p->width-(2.5*boxDec)*j,  p->y + p->height - boxDec*j);
			  j++;
			}

			glColor3ub(0,0,0);
			glRectf(p->x+(2.5*boxDec)*j,  p->y+boxDec*j,  p->x + p->width-(2.5*boxDec)*j,  p->y + p->height - boxDec*j);
			
			if(p->selected){
				glColor3ub(0,0,255); /* 青 */
				glRectf(p->x+boxInX,  p->y+boxInY,  p->x + p->width-boxInX,  p->y + p->height- boxInY);
				tocselected = 1;
				selecttree(p->item);
			}

			if(p->x > dispx && p->x < dispx + dispw &&
			   p->y > dispy && p->y < dispy + disph){
				glRasterPos2f(p->x, p->y);
				size = tocs[i].height/2.0;
				dispsize = size * scalefactor;
				if(dispsize > 8.0 && dispsize < 70.0){
					level = (62.0-(dispsize-8.0))/62.0*256.0;
					glColor3ub(level, level, 0.0);
					jfontsize(size * 0.8);
					if(p->item){
						unsigned char buf[100];
						strcpy(buf,p->item->name);
						if(strlen(buf) <= 12){
							jsdraw(buf, p->x+boxInX, p->y+boxInY); /* GRB 01/27/95 */
						}
						else {
							int i=0;
							unsigned char *s = buf;
							for(s=buf; *s && i < 6; s++){
								if(*s < 0x80) continue;
								s++;
								i++;
							}
							jsdraw(s, p->x+boxInX, p->y+boxInY); /* GRB 01/27/95 */
							*s = '\0';
							jsdraw(buf, p->x+boxInX, p->y + (size*0.8)+boxInY); /* GRB 01/27/95 */
						}
					}
				}
			}
		}
	}
	if(! tocselected){ /* 何も選択されていない場合は全部選択されているのと同じにする */
		for(i=0; i<nitem; i++){
			items[i].selected = 1;
		}
	}

	glutSwapBuffers();
}	

void selecttree(Item *ip) /* ipおよびその子供項目を全部選択状態にする */
{
	ItemList *ilp;
	if(ip == NULL) return;
	for(ilp = ip->sub; ilp; ilp = ilp->nextitem){
		selecttree(ilp->item);
	}
	ip->selected = 1;
}

/*
	WING for Win95/NT

	97/09/05 mina
*/

/*=======================================================
//
// Project Name         :       WING
// File Name	 	:	ShowGuide.c
// Author(s)		:	George Borden
// Description		:
//========================================================
// Special Note(s) :
//========================================================
//
// Modification History:
//	DATE		 INITIALS		REASON
//	01/13/95	 GRB			created.	
//      01/25/95         GRB                    
//                          テキストを使えるようにしました。
//      01/30/95         GRB
//                          処理高速化・見かけ変更
//      02/03/95         GRB
//                          コメント書きやプログラムの整理
//
//	$Date: 1995/02/03 15:34:42 $
//	$Revision: 1.19 $
//========================================================
// 			   Created by Sharp Corporation
//========================================================*/

static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/showGuide.c,v 1.19 1995/02/03 15:34:42 masui Exp masui $";

#define kashi

	/* ----------- Include Files ------------------ */
/*
#include <GL/glut.h>
#include <GL/gl.h>
*/
#include <GLUT/glut.h>

#include <stdio.h>
#include "rgb.h"

#include	"mapitem.h"

#include <math.h>	/* hypot()を呼ぶのに何故か絶対必要 */

#include "jogl.h"  // mina 2014 10 16
#include "jstring.h"     /* GRB 01/25/95 */
#include "showGuide.h"
#include "sub.h"
#include "winpos.h"
#include "calcWeight.h"
#include "dataentry.h"
#include "main.h"
#include "move.h"

#ifndef TRUE
#define	TRUE	(0 == 0)
#endif
#ifndef FALSE
#define	FALSE	(0 != 0)
#endif

	/* ----------- Globals ------------------------ */

long gGuideW, gGuideH;  /* ウインドウの寸法 */

Weight weights[MAXITEM];

	/* ----------- Defines ------------------------ */
#define TITLEH 30
#define TEXTH 20
#define FNTSZ16 16
#define FNTSZ24 24
#define PICTMGN 10
#define PICTH 256+10
#define MAXTITLES  HEIGHT/30;		/* 最大表示項目数 */
#define MAXTEXT  HEIGHT/(30+144)	/* 最大解説表示数 */
#define MAXALL HEIGHT/(30+144+256)	/* 最大写真表示数 */

	/* ----------- ROM Globals -------------------- */
	/* ----------- Macro Definitions -------------- */
	/* ----------- Type Definitions --------------- */
	/* ----------- Structs, Type Declarations ----- */
	/* ----------- Variables ---------------------- */
int gWid;
static float gArea;	/* 目と地図の距離 */
static float gVpx, gVpy;	/* 見ている場所 */
static long gX1[MAXITEM], gY1[MAXITEM], gX2[MAXITEM], gY2[MAXITEM];

	/* ----------- Prototypes ----------- */
	/* ----------- Functions ----------- */

/*******
* initGuide
*     Description: ガイドの初期化処理
*     Pass:
*     Returns:
*******/
void initGuide()
{
  /*Read necessary graphics data
    Set up necessary globals*/
  initWeight();
  glutInitWindowPosition(GUIDEPOSX, GUIDEPOSY);
  glutInitWindowSize(GUIDEWIDTH, GUIDEHEIGHT);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  gWid = glutCreateWindow("Guide Book");
  glClearColor(170.0 / 256.0, 170.0 / 256.0, 170.0 / 256.0, 0.0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA);

  gGuideW = GUIDEWIDTH;
  gGuideH = GUIDEHEIGHT;

  /* 970904 mina  コールバック関数の設定 */
  glutDisplayFunc(showGuide);
  glutKeyboardFunc(guideKeyFunc);
}

/*******
* showGuide
*     Description: ガイド項目表示位置計算してガイドを表示する処理
*     Pass:
*     Returns:
*******/
void showGuide(void)
{
  calcPos(gX1, gY1, gX2, gY2);
  refreshGuide();
}

/*******
* refreshGuide
*     Description: 計算せずにガイドを表示し直す処理
*     Pass:
*     Returns:
*******/
void refreshGuide()
{
  Item *ip;
  int i=0;
  long subbyt;
  long yPos=0, j, k, len;
  char tmpBuf[255];
  long v[2];
  int	width, height;

  glutSetWindow(gWid);
  width = glutGet(GLUT_WINDOW_WIDTH);
  height = glutGet(GLUT_WINDOW_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glClear(GL_COLOR_BUFFER_BIT);

	clearAllItems();
  while(weights[i].item)	{
    /*draw it or do something else*/
    ip = weights[i].item;
    /* \/ \/ \/ GRB 01/25/95 */
    yPos=gY1[i];

    yPos=yPos-TITLEH;
/* 970916 mina  useless?
    glColor3ub(170, 170, 170);
    glRectf(0, yPos, gGuideW, yPos+TITLEH);
*/
	glColor3ub(ip->r,  ip->g,  ip->b);
    glRectf(10, yPos+TITLEH-2, 10+TITLEH,  yPos+2);

    /* 131016 mina: use stroke font
    jsetfont(JIS24, 0, 0);
    jfontsize((float)FNTSZ24);
    glColor3ub(0, 0, 0);
    jsdraw(ip->name, 20+TITLEH, yPos+4);
    */
    jsetfont(STROKE, LINE, 2);
    jfontsize((float)FNTSZ24);
    glColor3ub(0, 0, 0);
    jsdraw(ip->name, 20+TITLEH, yPos-12);

    /* 131016 mina: use stroke font
    jsetfont(JIS16, 0, 0);
    jfontsize((float)FNTSZ16);
    */
    jsetfont(STROKE, LINE, 2);
    jfontsize((float)FNTSZ16);

    k=0;
    while(k<ip->infolines && yPos-TEXTH*k>gY2[i])
      k++;
    if(k) {
/* 970916 mina  useless?
      glColor3ub(170,  170,  170);
      glRectf(0, yPos-TEXTH*k, gGuideW, yPos);
*/
	  glColor3ub(0, 0, 0);
      for(j=1;j<=k;j++) {
	yPos=yPos-TEXTH;
	getLine(ip->info, tmpBuf, &len, j);
        /* 131016 mina: use stroke font
	jsdraw(tmpBuf,20+TITLEH,yPos+4);
	*/
	jsdraw(tmpBuf,20+TITLEH,yPos-8);
      }
    }
    if(yPos>gY2[i] && ip->image) {
      yPos=yPos-(ip->image->height+PICTMGN);
/* 970916 mina  useless?
      glColor3ub(170, 170, 170);
      glRectf(0, yPos,  gGuideW,  yPos+ip->image->height+PICTMGN);
*/
	  glRasterPos2i(20+TITLEH, yPos+6);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glDrawPixels(ip->image->width, ip->image->height, GL_RGB, GL_UNSIGNED_BYTE, ip->image->data);
    }
    /* /\ /\ /\ GRB 01/25/95 */

	dispItem(weights[i].item);

      glColor3ub(20, 20, 20);
      glBegin(GL_LINES);
          glVertex2i(10, yPos + 2);
          glVertex2i(gGuideW - 10, yPos + 2);
      glEnd();
      glColor3ub(255, 255, 255);
      glBegin(GL_LINES);
          glVertex2i(10, yPos + 1);
          glVertex2i(gGuideW - 10, yPos + 1);
      glEnd();
    i++;
  }

    /* finish drawing it*/
    glutSwapBuffers();
}

/*******
* calcPos
*     Description: 項目表示位置を計算する処理
*     Pass:    *xPos1,*yPos1,*xPos2,*yPos2 = アイテムの数の
*              ポインターリスト
*     Returns: 項目の表示座標「weightアイテム順」
*******/
void calcPos(long *xPos1, long *yPos1, long *xPos2, long *yPos2)
{
  Item *ip;
  int i=0, j=0;
  long yTot = 0, yOff, yInc, width, height;
  char imagename[200];
  float totalWeight=1.0;

  glutSetWindow(gWid);
  yOff = gGuideH;

  while(weights[i].item) {
    ip = weights[i].item;
    yInc = gGuideH*(weights[i].weight/totalWeight);
    /* \/ \/ \/ GRB 01/25/95 */
    j=0;
    while(j*TEXTH+TITLEH<yInc && j<ip->infolines)
      j++;
  if(yInc>TITLEH+TEXTH*ip->infolines) {
      Image *imagep;
      RGBImageRec *rgbImage;
      if(ip->image && ! ip->image->data) {
	imagep = ip->image;
	sprintf(imagename,IMAGEDIR "%s",imagep->name);
	rgbImage = rgbImageLoad(imagename);
	imagep->data = rgbImage->data;
	imagep->width = rgbImage->sizeX;
	imagep->height = rgbImage->sizeY;
      }
      if(ip->image && ip->image->data)
	yInc=TITLEH+TEXTH*ip->infolines+ip->image->height+PICTMGN;
      else
	yInc= TITLEH+j*TEXTH;
    }
    else
      yInc = TITLEH+j*TEXTH;
    /* /\ /\ /\ GRB 01/25/95 */
    xPos1[i] = 0;
    yPos1[i] = yOff - yTot;
    xPos2[i] = gGuideW;
    yPos2[i] = yOff - yTot - yInc;
    yTot = yTot + yInc;
    if(yPos2[i]<0)
      yPos2[i]=0;
    if(yPos1[i]<0){
      weights[i+1].item = NULL;
      weights[i].item = NULL;
    }
    i++;
  }
}
/*******
* getLine
*     Description: 改行の何番目かまでのテキストを取る処理
*     Pass:    *src            元のテキストバッファ
*              *dest           処理結果のテキストバッファ
*              lineNum         取り込む行番
*     Returns: *dest           lineNum番目の改行までのテキスト
*              *length         *destの桁数「0=テキスト無し」
*******/
void getLine(char *src, char *dest, long *length, long lineNum)
{
  char *ptr1=src, *ptr2=dest;
  long cnt=1, len=0;
  int done=FALSE;

  while(!done) {
    if(cnt==lineNum) {
      *ptr2=*ptr1;
      ptr2++;
      len++;
    }
    if(*ptr1=='\n') {
      cnt++;
      if(cnt>lineNum)
	done=TRUE;
    }
    else if(*ptr1==0)
      done=TRUE;
    ptr1++;
  }
  *ptr2=0;
  *length = len-1;
}

/*******
* guideClick
*     Description: ガイドの中でクリックされた項目を取り込む処理
*     Pass:    xPos, yPos      クリックされた座標「グローバル座標」
*     Returns: ITEM*           クリックされた項目
*******/
Item *guideClick(long xPos, long yPos)
{
  char done=0;
  int i=0;
  long lcX, lcY, yOff, yInc, orgX, orgY;

  glutSetWindow(gWid);
  gGuideW = glutGet(GLUT_WINDOW_WIDTH);
  gGuideH = glutGet(GLUT_WINDOW_HEIGHT);
  yOff=gGuideH;
  /*
  getorigin(&orgX, &orgY);
  */
  orgX = orgY = 0;

  lcX=xPos-orgX;
  lcY = yOff - yPos;	/* 970912 mina */

  while(!done) {
    if((lcX>=gX1[i] && lcX<=gX2[i] && lcY>=gY2[i] && lcY<=gY1[i]) || !weights[i].item)
      done=1;
    i++;
  }
  if(!weights[i-1].item)
    return (NULL);
  else
    return (weights[i-1].item);
}

/* 970905 mina, 元は handleGuide */
void guideKeyFunc(unsigned char key, int x, int y)
{
Item	*item;

	glutSetWindow(gWid);

	if (key == ' ') {
	/* スペースで項目選択、地図の視点が移動する */
                item = guideClick(x, y);
                if (item){
			if(item->locx != 0 || item->locy != 0){
				movetoItem(item);
				zoomTo(0.27);
				zoomTo(0.3);
				zoomTo(0.27);
				zoomTo(0.3);
			}
			else { /* 選択された項目のみ表示するようにする */
				weights[0].item = item;
				weights[0].weight = 1.0;
				weights[1].item = NULL;
				weights[1].weight = 0.0;
				showGuide();
			}
		}
        }
}

void setGuideArea(float area)
{
  float total;

  gArea = area;
  calcWeight((float)gVpx, (float)gVpy, gArea);
  showGuide();
}

/*******
* setGuidePos
* Description: 現在見えている図上座標値を設定する処理
*              ここでは項目の重さ表示位置や実際の表示が行います。
*     Pass:    xPos, yPos      現在位置の地図上座標
*     Returns:
*
* 970911 mina  setGuideArea と機能を統合
* 970912 mina  画面更新のタイミングのフラグを追加
*******/
void setGuidePos(long xPos, long yPos, float area, char redrawnow)
{
  float total;

  gVpx = xPos;
  gVpy = yPos;
  gArea = area;
  calcWeight((float)gVpx, (float)gVpy, gArea);

  if (redrawnow) {
	  showGuide();
  } else {
	  glutSetWindow(gWid);
	  glutPostRedisplay();
  }
}


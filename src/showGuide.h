/*
	WING for Win95/NT

	97/09/05 mina
*/

/*
	$Date: 1995/02/03 14:07:35 $
	$Revision: 1.6 $
*/
#ifndef _SHOWGUIDE_H_
#define _SHOWGUIDE_H_

#include "dataentry.h"

extern int gWid;

void initGuide();			/* Guideの初期化処理 */
void showGuide(void);			/* Guideの中を表示する処理 */
void refreshGuide();
void setGuideArea(float);		/* Guideの範囲設定処理 */
void setGuidePos(long, long, float, char);		/* Guideの位置の設定処理 */
Item *guideClick(long xPos, long yPos);	/* Guideのクリック処理 */

void calcPos(long *, long *, long *, long *);
void distribute(float, float);
int inGuideWindow(long, long);
void getLine(char *, char *, long *, long);     /* GRB 01/25/95 */

void handleGuide(int, int);		/* マウスとペンの扱い */

void guideKeyFunc(unsigned char, int, int);

#endif


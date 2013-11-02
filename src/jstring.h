/*
	$Date: 1995/01/30 17:02:32 $
	$Revision: 1.5 $
	$Source: /usr/people/masui/WING/wing/RCS/jstring.h,v $
*/
#ifndef _JSTRING_H_
#define _JSTRING_H_

#ifndef JISFONTDIR
#define JISFONTDIR "./fonts/"
#endif
#define JIS16FONT JISFONTDIR "jis16"
#define JIS24FONT JISFONTDIR "jis24"

/*
	jgl.hでは以下のような定義になっている

	#define STROKE 0
	#define MINCHO 1
	#define GOSIKU 2
	#define MARUGO 3

	ドットフォント名を追加
*/
#define JIS16 4
#define JIS24 5
#define JIS32 6

void jsetfont(int name, int type, int smooth);
/*
	name:	MINCHO/GOSIKU
	type:	POLYGON/LINE
	smooth:	1: no spline
		2: spline (coarse)
		...
		5: spline (fine)
*/
void jfontsize(float);
void jsdraw(unsigned char*, float x, float y);

#endif

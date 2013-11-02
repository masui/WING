/*
	970909 mina  OpenGL ÂÐ±þ¤Ë½¤Àµ
*/

/*
	$Date: 1995/02/03 12:06:12 $
	$Revision: 1.16 $
	$Source: /user/masui/WING/wing/RCS/jstring.c,v $
	$Author: masui $

	JGL¤Î¥é¥¤¥Ö¥é¥ê¤ò»È¤Ã¤Æ2¼¡¸µ¤Î´Á»úÊ¸»úÎó¤ò³Ú¤Ë½ñ¤¯¡£
	JGL¥Õ¥©¥ó¥È¤ÈJIS16/24¥Õ¥©¥ó¥È¤ò»È¤¦¤³¤È¤¬¤Ç¤­¤?£

	»È¤¤¤«¤¿
		jsetfont(MINCHO, POLYGON, 1);
		jfontsize(100.0);
		...
		jsdraw("Áý°?, locx, locy);

*/
/*
#include <GL/glut.h>
#include <GL/gl.h>
*/
#include <GLUT/glut.h>

#include "jogl.h"
#include <stdio.h>
#include "jstring.h"

static int fontname = GOTHIC;
static int fonttype = LINE;
static int fontsmooth = 2;	/* ÁÆ¤¤¥¹¥×¥é¥¤¥?*/
static float fontsize = 10.0;
#define MAXJSTRLEN 1000

#define ROW (0x74-0x21)
#define COLUMN (0x7f-0x21)

/*
	JGL¤ËÉ¬Í×¤ÊÊÑ¿ô¤Ê¤É
*/
static double pos[3] = {0.0, 0.0, 0.0};
static double norm[3] = {0.0, 0.0, 1.0};

static int jid[ROW][COLUMN];
static int ids[MAXJSTRLEN+1];
static int jglopened = 0;

/*
	¥É¥Ã¥È¥Õ¥©¥ó¥È»ÈÍÑ¤ËÉ¬Í×¤ÊÊÑ¿ô¤Ê¤É
*/
static unsigned char *dot16 = NULL;
static unsigned char *dot24 = NULL;

/* Caution! This module use EUC code. Don't change the character code. */ 
static unsigned char zenkakustr[] = 
"¡¡¡ª¡É¡ô¡ð¡ó¡õ¡Ç¡Ê¡Ë¡ö¡Ü¡¤¡Ý¡¥¡¿£°£±£²£³£´£µ£¶£·£¸£¹¡§¡¨¡ã¡á¡ä¡©\
¡÷£Á£Â£Ã£Ä£Å£Æ£Ç£È£É£Ê£Ë£Ì£Í£Î£Ï£Ð£Ñ£Ò£Ó£Ô£Õ£Ö£×£Ø£Ù£Ú¡Î¡À¡Ï¡°¡²\
¡Æ£á£â£ã£ä£å£æ£ç£è£é£ê£?ì£???ð£ñ£ò£ó£ô£õ£ö£÷£ø£ù£ú¡Ð¡Ã¡Ñ¡Á";

void jsetfont(int name, int type, int smooth)
/*
	name:	STROKE/MINCHO/GOSIKU/JIS16/JIS24
	type:	POLYGON/LINE
	smooth:	1: no spline
		2: spline (coarse)
		...
		5: spline (fine)
*/
{
	fontname = name;
	fonttype = type;
	fontsmooth = smooth;
}

void jfontsize(float size)
{
	fontsize = size;
}

/*
	JGL¤ò»ÈÍÑ¤·¤¿´Á»úÉ½¼¨¥?¼¥Á¥?*/
static int jscreat(unsigned char *str)
{
#ifdef IRIS_GL
	int len;
	int i,c;
	int ub,lb;
	int ch,cl;
	unsigned char buf[3];

	if(! jglopened){
		jglopened = 1;
		jopen(2000);
	}
	/* str¤ÏEUC¤ò²¾Ä?*/
	len = strlen(str) / 2;
	for(i=c=0; str[i]; ){
		if(str[i] >= 0x80){ /* Á´³Ñ */
			buf[0] = str[i];
			buf[1] = str[i+1];
			ch = (buf[0] & 0x7f) - 0x21;
			cl = (buf[1] & 0x7f) - 0x21;
			i += 2;
		}
		else { /* È¾³Ñ */
			if(str[i] < 0x20){
				i++;
				continue;
			}
			strncpy(buf,zenkakustr+2*(str[i]-0x20),2);
			ch = (buf[0] & 0x7f) - 0x21;
			cl = (buf[1] & 0x7f) - 0x21;
			i++;
		}
		if(! jid[ch][cl]){
			/* 970909 mina
			jid[ch][cl] = jcreat(fontname, buf, fonttype, fontsmooth);
			*/
			jid[ch][cl] = jcreate(fontname, buf, fonttype, fontsmooth);
			if(jid[ch][cl] == 0){
				fprintf(stderr,"Can't create Japanese character font!\n");
			}
		}
		ids[c] = jid[ch][cl];
		c++;
		if(c >= MAXJSTRLEN) break;
	}
	ids[c] = 0; /* end of id */
#endif
	return -1;
}

#define RGBA(r,g,b,a) \
	((((r) & 0xff) << 24) + (((g) & 0xff) << 16) + \
	(((b) & 0xff) << 8) + ((a) & 0xff))
/* 970917 mina for WINDOWS
#define RGBA(r,g,b,a) \
	((((a) & 0xff) << 24) + (((b) & 0xff) << 16) + \
	(((g) & 0xff) << 8) + ((r) & 0xff))
*/
static int masks[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

static int jdotdraw(unsigned char *str, float x, float y)
{
	FILE *f;
	register int i,j,k;
	register unsigned long *lp;
	register unsigned char mask;
	int size;
	int dotfontsize = 16;
	int column;
	unsigned char *p;
	short r,g,b;
	float col[4];
	int ub, lb;
	unsigned long dot[24*24];
	int ix,iy;
	unsigned long fgcolor, bgcolor;

	dotfontsize = (fontname == JIS16 ? 16 : 24);
	/* fontsize = (fontname == JIS16 ? 16.0 : 24.0); */
	if(fontname == JIS16 && dot16 == NULL){
		size = ROW*COLUMN*dotfontsize*dotfontsize/8;
		f = fopen(JIS16FONT,"r");
		if(f != NULL){
			dot16 = (unsigned char*)malloc(size);
			fread(dot16,1,size,f);
			fclose(f);
		}
		else{
			fprintf(stderr, "Can't open font file %s\n", JIS16FONT);
			return 0;
		}
	}
	else if(fontname == JIS24 && dot24 == NULL){
		size = ROW*COLUMN*dotfontsize*dotfontsize/8;
		f = fopen(JIS24FONT,"r");
		if(f != NULL){
			dot24 = (unsigned char*)malloc(size);
			fread(dot24,1,size,f);
			fclose(f);
		}
		else{
			fprintf(stderr, "Can't open font file %s\n", JIS24FONT);
			return 0;
		}
	}
	/* 970909 mina
	gRGBcolor(&r, &g, &b);
	*/
	glGetFloatv(GL_CURRENT_COLOR, col);
	ub = *str & 0x7f;
	lb = *(str+1) & 0x7f;
	column = (dotfontsize+7)/8;
	if(fontname == JIS16){
		p = &dot16[(COLUMN*(ub-0x21)+(lb-0x21))*dotfontsize*column];
	}
	else if(fontname == JIS24){
		p = &dot24[(COLUMN*(ub-0x21)+(lb-0x21))*dotfontsize*column];
	}
	r = col[0] * 255;	/* 970909 mina */
	g = col[1] * 255;
	b = col[2] * 255;
	fgcolor = RGBA(r,g,b, 0xff);
	bgcolor = RGBA(0,0,0,0);
#ifdef THIS_IS_SLOW
	for(i=0; i<dotfontsize; i++){
		for(j=0; j<dotfontsize; j++){
			dot[(dotfontsize-i-1)*dotfontsize+j] =
				(*(p + j/8)) & (0x80 >> j%8) ?
					fgcolor : bgcolor;
		}
		p += column;
	}
#endif
#ifdef THIS_IS_FASTER
	lp = dot + dotfontsize * (dotfontsize-1);
	for(i=0; i<dotfontsize; i++){
		mask = 0x80;
		for(j=0; j<dotfontsize; j++, lp++){
			*lp = (*p & mask) ? fgcolor : bgcolor;
			if(! (mask >>= 1)){ mask = 0x80; p++; }
		}
		if(mask != 0x80) p++;
		lp -= (2 * dotfontsize);
	}
#endif
	lp = dot + dotfontsize * (dotfontsize-1);
	for(i=0; i<dotfontsize; i++){
		for(j=k=0; j<dotfontsize; j++, lp++){
			*lp = (*p & masks[k]) ? fgcolor : bgcolor;
			if(++k == 8){
				k = 0;
				p++;
			}
		}
		if(k) p++;
		lp -= (2 * dotfontsize);
	}

	ix = (int)x; iy = (int)y;
	/* 970909 mina
	lrectwrite(ix,iy,ix+dotfontsize-1,iy+dotfontsize-1,dot);
	*/
	glRasterPos2i(ix, iy);
	glDrawPixels(dotfontsize, dotfontsize, GL_RGBA, GL_UNSIGNED_BYTE, dot);
}

void jsdraw(unsigned char *str, float x, float y)
{
	int *idp = ids;
	int ret;
	unsigned char buf[3];

	int listID;	/* 970909 mina */

	switch(fontname){
	case STROKE:
	case MINCHO:
	/*
	case GOSIKU:
	*/
	case GOTHIC:	/* 970909 mina */
	case MARUGO:
		/* 970909 mina
		jscreat(str);
		pos[0] = x + fontsize/2.0;
		pos[1] = y + fontsize/2.0;
		for(;*idp;idp++){
			ret = jdraw(*idp, pos, norm, fontsize, fontsize, 0.0);
			pos[0] += fontsize;
		}
		*/
		listID = jcreate(str, fontname, fonttype, fontsmooth);
		pos[0] = x + fontsize/2.0;
		pos[1] = y + fontsize/2.0;
		jdraw(listID, pos, norm, fontsize, fontsize, 0.0);
		break;
	case JIS16:
	case JIS24:
		while(*str){
			if(*str < 0x20){
				str++;
				continue;
			}
			else if(*str < 0x80){
				strncpy(buf,zenkakustr+2*(*str-0x20),2);
				jdotdraw(buf,x,y);
				x += fontsize;
				str++;
				continue;
			}
			jdotdraw(str,x,y);
			x += fontsize;
			str += 2;
		}
		break;
	}
}

#ifdef TEST
#include <gl/gl.h>
main()
{
	int win;

	prefposition(20,500,20,500);
	win = winopen("test");
	RGBmode();
	gconfig();
	winset(win);

	RGBcolor(255,255,0);
	clear();

	blendfunction(BF_SA,BF_MSA); /* ¦ÁÃÍ¤òÆ¯¤«¤»¤?*/

	/* jsetfont(MINCHO, POLYGON, 1); */

	jsetfont(JIS24, NULL, NULL);
	jfontsize(24.0);

	RGBcolor(255,0,0);
	jsdraw("Áý°æ¤Ç¤¹hankaku½Ð¤Þ¤¹¤«ËÜÅö¤ËÃÙ¤¤¤Ç¤¹¤«", 10, 50);

	RGBcolor(0,0,255);
	jfontsize(40.0);
	jsdraw("Áý°æ¤Ç¤¹ËÜÅö¤ËÃÙ¤¤¤Ç¤¹¤«", 10, 200);

	jsetfont(GOSIKU, POLYGON, 1);
	jfontsize(50.0);
	jsdraw("Áý°æ¤Ç¤¹hankakuËÜÅö¤ËÃÙ¤¤¤Ç¤¹¤«", 10, 300); /* È¾³Ñ¤Ï½Ð¤Ê¤¤ */

	jfontsize(30.0);
	jsdraw("Áý°æ¤Ç¤¹ËÜÅö¤ËÃÙ¤¤¤Ç¤¹¤«", 10, 370);

	jsetfont(JIS16, NULL, NULL);
	jfontsize(50.0);
	RGBcolor(255,0,0);
	jsdraw("Áý°æ¤Ç¤¹", 10, 420);

	sleep(10);
}
#endif

#ifdef BENCH
#include <gl/gl.h>
main(int argc, char **argv)
{
	int win;
	int i,j,k;
	int font;
	unsigned char buf[3];
	float fontsize = 24.0;

	foreground();
	prefposition(20,500,20,500);
	win = winopen("test");
	RGBmode();
	gconfig();
	winset(win);
	blendfunction(BF_SA,BF_MSA); /* ¦ÁÃÍ¤òÆ¯¤«¤»¤?*/

	if(argc > 1){
		font = atoi(argv[1]);
	}
	switch(font){
	case STROKE:
		jsetfont(font, LINE, 0);
		break;
	case MINCHO:
	case GOSIKU:
	case MARUGO:
		jsetfont(font, POLYGON, 2);
		break;
	case JIS16:
	case JIS24:
		jsetfont(font, NULL, NULL);
	}
	jfontsize(fontsize);

	for(k=0; k<20; k++){
		RGBcolor(255,255,0);
		clear();
		RGBcolor(0,0,255);
		for(i=0; i<20; i++){
			for(j=0; j<20; j++){
				buf[0] = i + 0xb1;
				buf[1] = j + 0xb1;
				buf[2] = '\0';
				jsdraw(buf, fontsize * j, fontsize * i);
			}
		}
	}
}
#endif

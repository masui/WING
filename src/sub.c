/*
	WING for Win95/NT

	97/09/05 mina
*/

/* -*-fundamental-*-

	$Date: 1995/01/26 14:40:11 $
	$Revision: 1.3 $
	$Source: /usr/people/masui/WING/wing/RCS/sub.c,v $
*/
static char rcsid[] = "$Header: /usr/people/masui/WING/wing/RCS/sub.c,v 1.3 1995/01/26 14:40:11 masui Exp masui $";

#include <GLUT/glut.h>

#include <stdio.h>
#include <string.h>
#include "sub.h"

char *salloc(char *str)
{
	char *s;
	s = (char*)malloc(strlen(str)+1);
	if(s){
		strcpy(s,str);
	}
	else {
		fprintf(stderr,"Warning: salloc() failed!\n");
	}
	return s;
}

int inwindow(int win, int px, int py)
{
/* 970905 mina, ‚±‚ÌŠÖ”‚Í‚à‚¤Žg‚Á‚Ä‚È‚¢‚Í‚¸
	long	x, y, w, h;

	winset(win);
	getorigin(&x, &y);
	getsize(&w, &h);
	px -= x;
	py -= y;

	return px >= 0 && px <= w && py >= 0 && py <= h ;
*/
	return 0;
}


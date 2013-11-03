/*
	WING for Win95/NT

	97/09/03 mina
*/

/*=======================================================*
// 
// Project Name:     WING
// File Name:        CalcWeight.h
// Author(s):        Kashiwagi Kouichi
//========================================================*
// Created by Sharp Corporation
//========================================================*/

#ifndef _CALCWEIGHT_H_
#define _CALCWEIGHT_H_

#include "dataentry.h"

typedef struct _Weight{
	Item *item;
	float weight;
} Weight;

extern Weight weights[];

/* functions */
void initWeight();
void calcWeight(float x, float y, float d);
void calcHierarhy(ItemList *list, float base);
void calcCoefficient(float eyeheight);
void backProp(ItemList *list, float propweight);
void crossProp(ItemList *list, float propweight);
void changeOrder(ItemList *list, int *weightindex, float *total);

void calcWeightNoMove(Item*);

#endif


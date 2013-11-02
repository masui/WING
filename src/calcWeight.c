/*
	WING for Win95/NT
	
	97/09/03 mina
*/

/*
	$Date: 1995/02/01 12:57:01 $
	$Revision: 1.13 $
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/calcWeight.c,v 1.13 1995/02/01 12:57:01 masui Exp masui $";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataentry.h"
#include "calcWeight.h"

#define MAX_AREA 2000
#define MAX_DISPLAY_ITEM 30  /* <= nitem  */

static float magnitude,decweight;

#define max(x,y) ((x) > (y) ? (x) : (y))

/*
Function of calcWeight
	W(i) = f(d) * g(d,distance(i))
		i-th item
		d:eye height
	
	f(d) = A * exp(-a*d) : magnitude
		A:max_magnitude
		a:decrease coefficient of MaxMag
	g() = exp(-B*exp(-2*b*d)*distance(i)*distance(i))
		B*exp(-2*b*d):decrease coefficient of weight(decweight)
*/

void initWeight()
{
	int i;

	for (i=0;i<nitem;i++) {
		items[i].weight=0;
		weights[i].item = &items[i];
		weights[i].weight = 0.0;
	}
}

static int
compareWeight(const void *p1, const void *p2)
{
	float w1, w2;
	if(p1 == NULL || p2 == NULL) return 0;
	w1 = items[*((int*)p1)].weight;
	w2 = items[*((int*)p2)].weight;
	return w1 > w2 ? -1 : w1 == w2 ? 0 : 1 ;
}

static 
float weight(Item *ip, float d, float r)
{
	float v = 0.0;
	if(ip->weighttype == STANDARD){
		r /= 2000.0;
		if(d != 0.0) 
			v = (d - 1.0) / d * r + (1.0 - d);
	}
	else if(ip->weighttype == GENERAL){
		float v1, v2;
		v1 = 0.0;
		r /= 2000.0;
		if(d != 0.0) 
			v1 = (d - 1.0) / d * r + (1.0 - d);
		v2 = d - 0.3;
		v = (v1 > v2 ? v1 : v2);
	}
 	return v > 0.0 ? v : 0.0;
}

void propagateWeight(ItemList *list, float value)
{
	while(list && list->item){
		list->item->weight = max(list->item->weight, value * list->weight);
		list = list->nextitem;
	}
}

void calcWeight_masui(float x, float y, float d)
{
	int i;
	float r;
	Item *ip;
	float total;
	int ndisp;
	int sortOrder[MAXITEM];

	for(i=0; i<nitem; i++){
		ip = &items[i];
		r = hypot(x - ip->locx, y - ip->locy);
		if(items[i].selected && items[i].locx && items[i].locy)
			items[i].weight = weight(ip, d, r);
		else
			items[i].weight = 0.0;
	}
	/*
		関連項目に重みを伝播
	*/
	for (i=0;i<nitem;i++) 
		if(items[i].weight && items[i].relateditem)
			propagateWeight(items[i].relateditem, items[i].weight);
	/*
		重みでソート
	*/
	for(i=0; i<nitem; i++)
		sortOrder[i] = i;
	qsort(sortOrder, nitem, sizeof(int), compareWeight);

	total = 0.0;
	for(i=0; i<MAX_DISPLAY_ITEM; i++){
		int j;
		j = sortOrder[i];
/* printf("weight[%d] = %f\n",i,items[j].weight); */
		if (items[j].weight <= 0.0) break;
		weights[i].item = &items[j];
		weights[i].weight = items[j].weight;
		total += items[j].weight;
	}
	ndisp = i;
	if(total > 0.0){
		for(i=0; i<ndisp; i++){
			weights[i].weight /= total;
		}
	}
	weights[i].item = NULL;
	weights[i].weight = 0;
}

void calcWeight(float pointx, float pointy, float eyeheight)
{
	int i,count;
	float  total,distance;
	int sortOrder[MAXITEM];
	int ndisp;
	int haveloc = 0;


	/* calculate weights */
	/*
		選択されているものに全然座標が設定されていなければ
		距離による計算は行なわない
	*/
	for (i=0;i<nitem;i++) {
		if(! items[i].selected) continue;
		if(items[i].locx || items[i].locy){
			haveloc = 1;
			break;
		}
	}
	if(haveloc){
		for (i=0;i<nitem;i++) {
			distance = hypot(pointx-items[i].locx, pointy-items[i].locy);
			if(items[i].selected && items[i].locx && items[i].locy)
				items[i].weight =1-1/(eyeheight+0.01)*distance/MAX_AREA;
			else
				items[i].weight = 0.0;
			if (items[i].weight < 0) items[i].weight = 0;
		}
	}
	else {
		for (i=0;i<nitem;i++) {
			items[i].weight = items[i].selected ? 1.0 : 0.0;
		}
	}

	/* renew weights */
/*
	for (i=0;i<nitem;i++) {
		if (items[i].super == NULL) {
			renewWeight(items[i].sub,0,eyeheight);
			break;
		}
	}
*/
/*
	for (i=0;i<nitem;i++) {
		if (items[i].weight > 0)  
			backProp(items[i].super,items[i].weight);
	}
*/
	for (i=0;i<nitem;i++) { 
		if (items[i].weight > 0) 
			crossProp(items[i].relateditem,items[i].weight);
	}

	for(i=0; i<nitem; i++)
		sortOrder[i] = i;
	qsort(sortOrder, nitem, sizeof(int), compareWeight);
	
	total = 0.0;
	for(i=0; i<MAX_DISPLAY_ITEM; i++){
		int j;
		j = sortOrder[i];
/* printf("weight[%d] = %f\n",i,items[j].weight); */
		if (items[j].weight <= 0.0) break;
		weights[i].item = &items[j];
		weights[i].weight = items[j].weight;
		total += items[j].weight;
	}
	count = i;
/*
	for (i=0; i<nitem; i++) {
		if (items[i].super == NULL){
			if (items[i].weight > 0) {
				weights[count].item = &items[i];
				weights[count].weight = items[i].weight;
				total += weights[count].weight;
				count++;
			}
			changeOrder(items[i].sub,&count,&total);
		}
	}
*/
	weights[count].item = NULL;
	weights[count].weight = 0.0;
	if (total > 0.0) {	/* 970911 mina */
		for (i=0;i<count;i++) {
			weights[i].weight /= total;
		}
	}
}



void renewWeight(ItemList *list, int depth, float eyeheight)
{
	if (! list) return;
/* 1 */	(*list->item).weight = (*list->item).weight
			- eyeheight*depth/2;

	if ((*list->item).weight < 0) (*list->item).weight = 0;
	renewWeight(list->item->sub, depth+1,eyeheight);
	renewWeight(list->nextitem, depth,eyeheight);
}

void backProp(ItemList *list, float propweight)
{
	if(! list) return;
	if ((*list->item).weight < propweight/2) 
		(*list->item).weight = propweight/2;
	backProp(list->item->super,(*list->item).weight);
/*	backProp(list->nextitem,(*list->item).weight); */
}

void crossProp(ItemList *list, float propweight)
{
	if (! list) return;
	if ((*list->item).weight < propweight/2) 
		(*list->item).weight = propweight/2;
/*	backProp(list->item->super,propweight); */
	crossProp(list->nextitem,propweight);
}

void changeOrder(ItemList *list, int *weightindex, float *total)
{
	if (! list) return;
	if ((*list->item).weight > 0) {
		weights[*weightindex].item = list->item;
		weights[*weightindex].weight = (*list->item).weight;
		(*total) += weights[*weightindex].weight;
		(*weightindex)++;
	}
	changeOrder(list->item->sub,weightindex,total);
	changeOrder(list->nextitem,weightindex,total);
}

void calcWeightNoMove(Item *ip)
{
	int i, count;
	int sortOrder[MAXITEM];
	float total;

	for (i=0;i<nitem;i++) {
		items[i].weight = 0.0;
	}
	ip->weight = 1.0;

	crossProp(ip->relateditem,ip->weight);

	for(i=0; i<nitem; i++)
		sortOrder[i] = i;
	qsort(sortOrder, nitem, sizeof(int), compareWeight);

	total = 0.0;
	for(i=0; i<MAX_DISPLAY_ITEM; i++){
		int j;
		j = sortOrder[i];
		if (items[j].weight <= 0.0) break;
		weights[i].item = &items[j];
		weights[i].weight = items[j].weight;
		total += items[j].weight;
	}
	count = i;
	weights[count].item = NULL;
	weights[count].weight = 0.0;
	if (total > 0.0) {	/* 970911 mina */
		for (i=0;i<count;i++) {
			weights[i].weight /= total;
		}
	}
}

/*
	WING for Win95/NT

	970905 mina
*/

/* -*-fundamental-*-

	$Date: 1995/02/03 10:23:36 $
	$Revision: 1.28 $
	$Source: /user/masui/WING/wing/RCS/readdata.c,v $
*/
static char rcsid[] = "$Header: /user/masui/WING/wing/RCS/readdata.c,v 1.28 1995/02/03 10:23:36 masui Exp masui $";

#include	<GLUT/glut.h>

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include "dataentry.h"
#include "rgb.h"
#include "sub.h"
#include "winpos.h"

#define MAXBUF 1000
static unsigned char buf[MAXBUF];
static unsigned char s1[MAXBUF], s2[MAXBUF];

int nitem;				/* 項目総数 */
Item items[MAXITEM];			/* 項目データ */
int ntoc = 0;				/* 目次総数 */
TOC tocs[MAXTOC];

#define MAXFILENAME 500

static void getitemnames()
/*
	項目名リストの入ったファイルを読み込みつつ
	データ初期化
*/
{
	DIR *dir;
	struct dirent *dirent;
	char *itemname;
	Item *ip;

	dir = opendir(TEXTDIR);
	if (dir == NULL) {
		fprintf(stderr, "getitemnames(): Can't open directory %s\n",TEXTDIR);
		exit(0);
	}

	nitem = 0;
	while(dirent = readdir(dir)) {
		itemname = dirent->d_name;
		if(itemname[0] == '.') continue;
		if(itemname[0] == '#') continue;
		if(itemname[strlen(itemname)-1] == '~') continue;
		/*
			項目データ構造初期設定
		*/
		ip = &items[nitem];
		ip->itemname = salloc(itemname);
		ip->locx = ip->locy = 0.0;
		ip->name = ip->info = "";
		ip->weighttype = NOWEIGHT;
		ip->infolines = 0;
		ip->super = ip->sub = ip->relateditem = (ItemList*)NULL;
		ip->image = (Image*)NULL;

		if(nitem++ >= MAXITEM) break;
	}
	closedir(dir);
}

static
getitemindex(char *name) /* 項目名から項目インデクスを取得 */
{
	int i;
	for(i=0; i<nitem; i++){
		if(strcmp(name,items[i].itemname) == 0) return i;
	}
	return -1;
}

static Item*
getitemp(char *name) /* 項目名から項目ポインタを取得 */
{
	Item *ip;
	int index;
	index = getitemindex(name);
	ip = (index >= 0 ? &items[index] : (Item*)NULL);
	return ip;
}

static void
additemlist(ItemList **ilpp, Item *ip, float weight)
/*
	項目リストに項目を追加する
*/
{
	ItemList *ilp;
	ilp = (ItemList*)malloc(sizeof(ItemList));
	ilp->item = ip;
	ilp->weight = weight;
	ilp->nextitem = *ilpp;
	*ilpp = ilp;
}

void printitemnames() /* 項目名リスト印刷 */
{
	int i;
	for(i=0;i<nitem;i++)
		printf("%s\n",items[i].itemname);
}

void printitem(Item *ip) /* 項目ダンプ */
{
	ItemList *p;
	printf("item %s\n",ip->name);
	printf("location: %f %f\n",ip->locx,ip->locy);
	printf("weighttype: %s\n",ip->weighttype == STANDARD ? "Standard" :
		ip->weighttype == GENERAL ? "General" : "None");
	printf("info: %s\n",ip->info);
	printf("super: ");
	for(p=ip->super; p; p=p->nextitem)
		printf("%s ",p->item->name);
	printf("\n");
	printf("sub: ");
	for(p=ip->sub; p; p=p->nextitem)
		printf("%s ",p->item->name);
	printf("\n");
}

static int havesuper = 0;
static readitem(char*, Item*);

static void readitems() /* 項目内容を読み出しつつリンクを設定 */
{
	DIR	*dir;
	struct dirent	*dirent;
	char *itemname;
	int i;

	dir = opendir(TEXTDIR);
	if (dir == NULL) {
		fprintf(stderr, "readitems(): Can't open directory %s\n",TEXTDIR);
		exit(0);
	}

	i = 0;
	while(dirent = readdir(dir)) {
		itemname = dirent->d_name;
		if(itemname[0] == '.') continue;
		if(itemname[0] == '#') continue;
		if(itemname[strlen(itemname)-1] == '~') continue;
		readitem(itemname,&items[i++]);
	}
	closedir(dir);
}

static void processkey(Item*,char*,char*);

#define MAXKEYLEN 100
#define MAXVALLEN 10000

static readitem(char *entryname, Item *ip)
{
	FILE *f;
	unsigned char key[MAXKEYLEN];
	unsigned char value[MAXVALLEN];
	char filename[MAXFILENAME];

	sprintf(filename,TEXTDIR "%s",entryname);
	f = fopen(filename,"r");
	if(f == NULL) return 0;
	key[0] = '\0';
	havesuper = 0;
	while(fgets(buf, MAXBUF, f)){
		if(buf[0] != '\t'){
			if(key[0]){
				processkey(ip,key,value);
				key[0] = '\0';
			}
			sscanf(buf,"%s %[^\n]",s1,s2);
			strcpy(key,s1);
			strcpy(value,s2);
		}
		else {
			sscanf(buf," %[^\n]",s1);
			/* 最初の空白文字だけ読み飛ばす */
			if(strlen(value) + strlen(s1) + 1 >= MAXVALLEN){
				fprintf(stderr,"Too long data entry: <%s>\n",value);
				fprintf(stderr,"modify MAXVALLEN and recompile the system!\n");
				exit(0);
			}
			strcat(value,"\n");
			strcat(value,s1);
		}
	}
	if(key[0]){
		processkey(ip,key,value);
	}
	if(! havesuper && strcmp(entryname,"All")){
		/* Superが定義されていない項目は'All'をSuperに */
		processkey(ip,"Super:","All");
	}
	fclose(f);
}


/*
	データ項目例

Super:		Temple
Name:		白毫寺
Location:	3809 112
Info:		椿と萩の古寺
		志貴皇子(天智天皇の第7皇子)の山荘
		一切教寺
		阿弥陀如来座像(本尊、国宝)
		五色椿(別名七福神、4月)と萩(9月)
		9時-17時/300円
Weight:		Standard
*/

static void processkey(Item *ip, char *key, char *value)
{
/* printf("key=<%s>, value=<%s>\n",key,value);*/
	if(strcmp(key,"Name:") == 0){
		ip->name = salloc(value);
	}
	else if(strcmp(key,"Info:") == 0){
		char *s;
		ip->info = salloc(value);
		ip->infolines = 1;
		for(s = ip->info; *s; s++){
			if(*s == '\n') ip->infolines++;
		}
	}
	else if(strcmp(key,"Yomi:") == 0){
		ip->yomi = salloc(value);
	}
	else if(strcmp(key,"Location:") == 0){
		float x,y;
		if(sscanf(value,"%f %f",&x,&y) == 2){
			ip->locx = x; ip->locy = y;
		}
	}
	else if(strcmp(key,"Weight:") == 0){
		ip->weighttype = (strcmp(value, "Standard") == 0 ? STANDARD :
				  strcmp(value, "General") == 0 ? GENERAL :
				  STANDARD);
	}
	else if(strcmp(key,"Super:") == 0){
		Item *sp;
		char *s, t[100];
		s = value;
		while(sscanf(s,"%s",t)){
			sp = getitemp(t);
			if(sp){
				additemlist(&(ip->super),sp,0.0);
				additemlist(&(sp->sub),ip,0.0);
				havesuper = 1;
			}
			while(*s && *s != '\n') s++; /* EOSか改行まで読み飛ばす */
			if(! *s) break;
			s++;
		}
	}
	else if(strcmp(key,"Relation:") == 0){
		Item *rep;
		float weight;
		char *s;
		s = value;
		while(1){
			if(2 == sscanf(s,"%s %f",s1,&weight)){
				rep = getitemp(s1);
				if(rep){
					additemlist(&(ip->relateditem),rep,weight);
				}
				else {
					fprintf(stderr,"Item %s - related item <%s> not found\n",
						ip->name, s1);
				}
			}
			else {
				fprintf(stderr,"Item %s - strange Relation: format\n",
					ip->name);
			}
			while(*s && *s != '\n') s++;
			if(! *s) break;
			s++;
		}
	}
	else if(strcmp(key,"Image:") == 0){
		char *s, imagename[100], filename[MAXFILENAME];
		Image **imagepp, *imagep;
		s = value;
		imagepp = &(ip->image);
		while(sscanf(s,"%s",imagename)){ /* Imageのリンクを作成する */
			*imagepp = imagep = (Image*)malloc(sizeof(Image));
			if(imagep == NULL){
				fprintf(stderr,"processkey(): Can't alloc an Image structure\n");
				break;
			}
			imagep->name = salloc(imagename);
#ifdef PRELOADIMAGE
			sprintf(filename,IMAGEDIR "%s",imagename);
			imagep->data = loadRGB(filename,&(imagep->width),&(imagep->height));
#else
			imagep->data = (unsigned char*)NULL;
#endif
			imagep->nextimage = (Image*)NULL;
			imagepp = &(imagep->nextimage);

			while(*s && *s != '\n') s++; /* EOSか改行まで読み飛ばす */
			if(! *s) break;
			s++;
		}
	}
}

static nchildren(Item *ip)
{
	ItemList *ilp;
	int i;
	ilp = ip->sub;
	for(i=0; ilp; i++) ilp = ilp->nextitem;
	return i;
}

static void maketoc1(Item *ip, float x, float y, float width, float height);

static void maketoc() /* tocs[]の初期化 */
{
	Item *ip;
	ip = getitemp("All");
	ntoc = 0;
	maketoc1(ip, 0.0, 0.0, (float)TOCWIDTH, (float)TOCHEIGHT);
}

static void maketoc1(Item *ip, float x, float y, float width, float height)
{
	TOC *tocp;
	ItemList *ilp;
	int nc;
	int div;
	int i;
	float slit, cx, cy, cheight, cwidth;

	if(ntoc >= MAXTOC) return;
	if(nc = nchildren(ip)){
		for(div=2; nc > div * div; div++);
		slit = height / div / 10;
		cheight = (height-height/5-slit*(div+1)) / div;
		cwidth = (width-width/5-slit*(div+1)) / div;
		ilp = ip->sub;
		for(i=0; i<nc; i++){
 			cx = x + slit +(width/10.0)+(i % div) * (cwidth+slit);
			cy = y + slit + (height/10.0)+(div - (i / div) - 1) * (cheight+slit);
			maketoc1(ilp->item, cx, cy, cwidth, cheight);
			ilp = ilp->nextitem;
		}
	}
/* printf("tocs[%d] = %f, %f, %f, %f, %s\n", ntoc,x,y,width,height,ip->itemname); */

	tocp = &tocs[ntoc];
	tocp->x = x;
	tocp->y = y;
	tocp->width = width;
	tocp->height = height;
	tocp->name = ip->itemname;
	tocp->item = ip;
	tocp->selected = 0;
	ntoc++;
}

void printtocs() /* 目次データ印刷 */
{
	int i;
	for(i=0; i<ntoc; i++){
		printf("TOC[%d]: %s %s\n",i,tocs[i].name,
			tocs[i].item ? (tocs[i].item)->name : "");
	}
}

static void inherit1(Item *ip)
{
	ItemList *cp;
	Item *cip;
	for(cp = ip->sub; cp; cp = cp->nextitem){
		cip = cp->item;
		if(cip->locx == 0.0 && cip->locy == 0.0){
			cip->locx = ip->locx;
			cip->locy = ip->locy;
		}
		if(!cip->name || strcmp(cip->name,"") == 0)
			cip->name = ip->name;
		if(!cip->info || strcmp(cip->info,"") == 0)
			cip->info = ip->info;
		if(cip->weighttype == NOWEIGHT)
			cip->weighttype = ip->weighttype;
		inherit1(cip);
	}
}

static void
inherit()
{
	Item *ip;
	ip = &items[getitemindex("All")];
	inherit1(ip);
}

static float colorweight(float val)
/*
	以下のような関数で色の重みを決める

	1+-----
	 |      \
	 |       \
	 +--------\
	 0    60  120
*/
{
	float w;
	while(val > 360.0){ val -= 360.0; }
	while(val < 0.0){ val += 360.0; }
	if(val > 180.0) val = 360.0 - val;
	w = (val < 60.0 ? 1.0 : val > 120.0 ? 0.0 : -val/60.0 + 2.0);
	return w * 255.0;
}

static void
setcolor()
{
	int i;
	float bgangle = 60.0;
	float gap = 30.0; 
	float angle;
	Item *ip;
	for(i=0; i<nitem; i++){
		ip = &items[i];
		angle = bgangle + gap + (360.0 - gap * 2) * i / (nitem-1);
		ip->r = (short)colorweight(angle);
		ip->g = (short)colorweight(angle + 120.0);
		ip->b = (short)colorweight(angle + 240.0);
	}
}

void readdata()
{
	getitemnames();		/* 項目名だけ得てデータ初期化 */
	readitems();		/* 項目データの中身を読み出しリンク初期化 */
	inherit();		/* All から順にSuperのデータを子に伝える */
	maketoc();		/* TOC[]から項目へのリンクを張る */
	setcolor();		/* タイトル色を配分する */
}

#ifdef TEST
main(int argc, char **argv)
{
	FILE *f;
/*	if(argc < 2) exit(0); */
	readdata();

	{
		int i;
		for(i=0; i<ntoc; i++){
			if(tocs[i].item){
				printf("TOC item %s\n",(tocs[i].item)->name);
				printitem(tocs[i].item);
			}
		}
	}
	printtocs();
}
#endif

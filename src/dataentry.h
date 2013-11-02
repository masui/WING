/*
	WING for Win95/NT

	97/09/03 mina
*/

/* -*-fundamental-*-

	$Date: 1995/02/03 11:32:58 $
	$Revision: 1.19 $
	$Source: /user/masui/WING/wing/RCS/dataentry.h,v $
*/
#ifndef _DATAENTRY_H_
#define _DATAENTRY_H_

#ifndef TEXTDIR
#define TEXTDIR "./data/text/"
#endif
#ifndef IMAGEDIR
#define IMAGEDIR "./data/rgb/"
#endif

typedef struct _ItemList {
	struct _Item *item;
	struct _ItemList *nextitem;
	float weight;
} ItemList;

typedef enum {
	NOWEIGHT,
	STANDARD,
	GENERAL
} WeightType;

typedef struct _Image {
	char *name;			/* 画像ファイル名 (e.g. img0003)*/
	unsigned char *data;		/* lrectwrite()で使うビットマップデータ */
	long width, height;		/* 画像サイズ */
	struct _Image *nextimage;	/* 次の画像へのポインタ */
} Image;

typedef struct _Item {
	float locx,locy;	/* 地図上の座標 */
	char *itemname;		/* 名前(ASCII) ファイル名と同じ */
	char *name;		/* 名前(漢字) */
	char *yomi;		/* 名前の読み */
	char *info;		/* 解説文字列 */
	int infolines;		/* 解説文字列行数 */
	ItemList *super;	/* 親インスタンス */
	ItemList *sub;		/* 子インスタンス */
	ItemList *relateditem;	/* 関連インスタス */
	Image *image;		/* 画像データのリストへのポインタ */
	short r,g,b;		/* タイトル表示色 */
	int selected;		/* 目次で選択されたかどうか */
	WeightType weighttype;	/* 重みタイプ */
	float weight;		/* 重み */
} Item;

#define MAXITEM 1000
extern Item items[];		/* データ項目 */
extern int nitem;		/* データ項目総数 */

typedef struct _TOC {		/* 目次の表 */
	float x,y;		/* 目次ボタン座標 */
	float width,height;	/* 目次ボタンサイズ */
	char *name;		/* 項目名 */
	Item *item;		/* 対応する項目 */
	int selected;		/* ボタン選択/非選択 */
} TOC;

#define MAXTOC (MAXITEM * 2)
extern TOC tocs[];		/* 目次項目 */
extern int ntoc;		/* 目次項目総数 */

void readdata();			/* 項目データを読み込む */
     
#endif

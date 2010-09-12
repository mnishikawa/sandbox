/*
  pacman-sim.h
  pacman simulator definitions.
 */

/* デバッグ出力 */
#define DEBUGOUT


/* ステージデータ */
#define MAP_MAX_WIDTH 60
#define MAP_MAX_HEIGHT 22

typedef struct _stagedata{
  char data[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
  int w;
  int h;
}stagedata_s;


/* フィールドマップ定義（壁、通路情報） */
#define FIELD_NULL    0  /* 未定義（初期化用） */
#define FIELD_WALL    '#'  /* 壁 */
#define FIELD_IMPASSE '*'  /* 行き止まり */
#define FIELD_PASSAGE '-'  /* 通路 */
#define FIELD_CROSS   '+'  /* 交差点 */


/* ドット情報 */
#define DOT_ON        1  /* ドットあり */
#define DOT_OFF       0  /* ドットなし */


/* 自機情報 */
#define MAX_ENEMY_NUM 100

typedef struct _player{
  int x;               /* 現在のx座標*/
  int y;               /* 現在のy座標*/
  int x_prev;          /* 前のx座標*/
  int y_prev;          /* 前のy座標*/
}player_t;


/* 敵情報 */
typedef struct _enemy{
  int  type;            /* 敵タイプ(V,H,L,R,J) */
  int  subtype;         /* 敵タイプ(Jのときの次の行動タイプ状態保存) */
  int  x;               /* 現在のx座標 */
  int  y;               /* 現在のy座標 */
  int  x_prev;          /* 前のx座標 */
  int  y_prev;          /* 前のy座標 */
}enemy_t;

#define MAX_ENEMY_NUM  100  /* 敵の最大数 */

#define ENEMY_TYPE_0  '0'   /* 敵タイプ0 : t=0のときの動き */
#define ENEMY_TYPE_IMPASSE 'I' /* 敵タイプI ： 行き止まりの時は唯一進めるところに進む */
#define ENEMY_TYPE_PASSAGE 'P' /* 敵タイプP ： 通路の時は、前の位置では無い方に進む */

#define ENEMY_TYPE_V  'V'   /* 敵タイプV */
#define ENEMY_TYPE_H  'H'   /* 敵タイプH */
#define ENEMY_TYPE_L  'L'   /* 敵タイプL */
#define ENEMY_TYPE_R  'R'   /* 敵タイプR */
#define ENEMY_TYPE_J  'J'   /* 敵タイプJ */

/* 方向定義 */
#define DIR_R     1    /* 右 */
#define DIR_L     2    /* 左 */
#define DIR_U     3    /* 上 */
#define DIR_D     4    /* 下 */
#define DIR_I     0    /* 不正な値 */


/*
  pacman-sim.c
    pacman simulator functions.
    for GDD 2010

    Makoto Nishikawa
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "pacman-sim.h"


/* ステージデータ */
stagedata_s stage;

/* フィールドマップ（壁、通路情報） */
char pacman_field_map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];

/* ドット情報 */
char pacman_dot_map[MAP_MAX_HEIGHT][MAP_MAX_WIDTH];
int dot_counter;

/* 自機情報 */
player_t pacman;

/* 敵情報 */
enemy_t enemy[MAX_ENEMY_NUM];
int nr_enemy;

/* 画面情報の更新を有効にするフラグ : デフォルトはenable(1)*/
int update_display_data_flag = 1;



/* マクロ */

#define CHECKWALL(x,y)  (pacman_field_map[(y)][(x)] != FIELD_WALL)  /* 壁が無ければ真 */
#define CHECKSAMEPOS(px,py,x,y)  ( ((px)==(x)) && ((py)==(y)) )   /* 比較する座標が一緒なら真 */



/* 
   update_display_data()
     表示情報を更新するフラグ制御

     input:
       1 : 有効にする
       0 : 無効にする

     return:
       セットした値を返す。
 */
int update_display_data(int enable) {
  if(enable == 1)
    update_display_data_flag = 1;
  return update_display_data_flag;
}


/*
  pacman_init_map()

   初期化時のフィールドマップ解析・初期化関数。
   シミュレータ実行中の条件分岐を減らすために、
   壁、通路情報を判定してマッピングデータを作っておく。
   また、dot_counterを計算しておき、クリア条件判定に用いる。

*/
int pacman_init_map(int width, int height, char **stage_data)
{
  int x,y;

  /* 初期化 */
  for(y=0;y<height;y++){
    for(x=0;x<width;x++) { 
      stage.data[y][x]=stage_data[y][x];
    }
  }
  stage.w=width;
  stage.h=height;
  dot_counter = 0;
  nr_enemy = 0;


  for(y=0;y<stage.h;y++){
    for(x=0;x<stage.w;x++) { 
      /* ドット情報、通路情報の初期化 */
      pacman_dot_map[y][x]=DOT_OFF;
      pacman_field_map[y][x]=FIELD_NULL;

      /* 壁マッピング、ドットカウント、自機、敵設定 */
      switch (stage.data[y][x]) {
      case '#':
      case ' ':
	pacman_field_map[y][x]=FIELD_WALL;
	break;
      case '.':
	pacman_dot_map[y][x]=DOT_ON;
	dot_counter++;
	break;
      case '@':
	pacman.x = x; pacman.y = y;
	pacman.x_prev = x; pacman.y_prev = y;
	break;
      case 'V':
	enemy[nr_enemy].type = ENEMY_TYPE_V;
	enemy[nr_enemy].subtype = ENEMY_TYPE_V;
             	enemy[nr_enemy].x = x; enemy[nr_enemy].y = y; nr_enemy++;
             	enemy[nr_enemy].x_prev = x; enemy[nr_enemy].y_prev = y;
	break;
      case 'H':
	enemy[nr_enemy].type = ENEMY_TYPE_H;
	enemy[nr_enemy].subtype = ENEMY_TYPE_H;
             	enemy[nr_enemy].x = x; enemy[nr_enemy].y = y; nr_enemy++;
             	enemy[nr_enemy].x_prev = x; enemy[nr_enemy].y_prev = y;
	break;
      case 'L':
	enemy[nr_enemy].type = ENEMY_TYPE_L;
	enemy[nr_enemy].subtype = ENEMY_TYPE_L;
             	enemy[nr_enemy].x = x; enemy[nr_enemy].y = y; nr_enemy++;
             	enemy[nr_enemy].x_prev = x; enemy[nr_enemy].y_prev = y;
	break;
      case 'R':
	enemy[nr_enemy].type = ENEMY_TYPE_R;
	enemy[nr_enemy].subtype = ENEMY_TYPE_R;
             	enemy[nr_enemy].x = x; enemy[nr_enemy].y = y; nr_enemy++;
             	enemy[nr_enemy].x_prev = x; enemy[nr_enemy].y_prev = y;
	break;
      case 'J':
	enemy[nr_enemy].type = ENEMY_TYPE_J;
	enemy[nr_enemy].subtype = ENEMY_TYPE_L;
             	enemy[nr_enemy].x = x; enemy[nr_enemy].y = y; nr_enemy++;
             	enemy[nr_enemy].x_prev = x; enemy[nr_enemy].y_prev = y;
	break;
      default:
	fprintf(stderr, "Error: Unknown map data : %c\n",stage.data[y][x]);
	return -EINVAL;
	break;
      }	  
    }
  }

  /* 通路情報解析 */
  for(y=1;y<(stage.h-1);y++){
    for(x=1;x<(stage.w-1);x++) {
      int nr_dir;
      nr_dir=0;
      if(pacman_field_map[y][x] == FIELD_NULL){
	(CHECKWALL(x-1,y))&&nr_dir++;
	(CHECKWALL(x,y-1))&&nr_dir++;
	(CHECKWALL(x+1,y))&&nr_dir++;
	(CHECKWALL(x,y+1))&&nr_dir++;
         if(nr_dir == 1)
           pacman_field_map[y][x] = FIELD_IMPASSE; /* 行き止まり */
         if(nr_dir == 2)
           pacman_field_map[y][x] = FIELD_PASSAGE; /* 通路 */
         if(nr_dir >= 3)
           pacman_field_map[y][x] = FIELD_CROSS; /* 交差点 */
      }
    }
  }

  return 0;
}


/*
  pacman_disp_screen()
  画面表示
*/
int pacman_disp_screen()
{
  int x,y;
  int id;

  printf("---stage string---\n");
  for(y=0;y<stage.h;y++){
    for(x=0;x<stage.w;x++){
      /* update dot data */
      if(CHECKWALL(x,y)) {
	if(pacman_dot_map[y][x] == DOT_ON)
	  stage.data[y][x] = '.';
	else
	  stage.data[y][x] = ' ';
      }

      /* update enemy data */
      for(id=0;id<nr_enemy;id++){
	if((enemy[id].x == x) &&(enemy[id].y ==y))
	  stage.data[y][x] = enemy[id].type;
      } 

      /* update player data */
      if((pacman.x == x) &&(pacman.y ==y))
	stage.data[y][x] = '@';
      
      /* display data */
      printf("%c",stage.data[y][x]);
    }

    printf("\n");
  }

  return 0;
}

/* 
 pacman_disp_stat()
  状態表示
*/
void pacman_disp_stat()
{
  int i;
  int x,y;

  printf("---field map---\n");
  for(y=0;y<stage.h;y++){
    for(x=0;x<stage.w;x++)
      printf("%c",pacman_field_map[y][x]);
    printf("x=%d,y=%d\n",x,y);
  }

  printf("---initial dot map---\n");
  for(y=0;y<stage.h;y++){
    for(x=0;x<stage.w;x++)
      printf("%01d",pacman_dot_map[y][x]);
    printf("x=%d,y=%d\n",x,y);
  }

  printf("---dot count---\n");
  printf("dot_counter=%d\n",dot_counter);

  printf("---enemy count---\n");
  printf("nr_enemy=%d\n",nr_enemy);

  for(i=0;i<nr_enemy;i++){
    printf("Enemy(%d): %c / %d,%d\n",i,enemy[i].type,enemy[i].x,enemy[i].y);
  }

}


/*
  pacman_player_move()

  自機の移動
   input:
     char p_dir
         'h'   左方向へ移動
         'j' 	下方向へ移動
         'k' 	上方向へ移動
         'l' 	右方向へ移動
         '.' 	その場に留まる     

   return:
         0         移動する
         -EINVAL    移動不可(座標を変更しない)

 */
int pacman_player_move(char pdir)
{
  int x_next,y_next;
 
  x_next = pacman.x;
  y_next = pacman.y;

  switch(pdir){
  case 'h': /* 左移動 */
    x_next = pacman.x - 1;
    break;
  case 'l': /* 右移動 */
    x_next = pacman.x + 1;
    break;
  case 'j':  /* 下移動 */
    y_next = pacman.y + 1;
    break;
  case 'k':  /* 上移動 */
    y_next = pacman.y - 1;
    break;
  case '.':  /* その場に留まる */
    break;
  default:   /* 不正な入力 */
    fprintf(stderr, "pacman_player_move():Error: Invalid input data : %c\n",pdir);
    return -EINVAL;
  }

  if(!CHECKWALL(x_next,y_next)){
    /* ここには進めない */
    return -EINVAL;
  }

  /* 自機の位置情報を更新 */
  pacman.x_prev = pacman.x;
  pacman.y_prev = pacman.y;
  pacman.x = x_next;
  pacman.y = y_next;

  /* ドットカウンター判定 */
  if(pacman_dot_map[pacman.y][pacman.x] == DOT_ON){
    pacman_dot_map[pacman.y][pacman.x] = DOT_OFF;
    dot_counter--;
  }

  return 0;
}

/*
  pacman_check_dir()
    前の座標からの相対移動値で方向を判定する。

  input:
      int dx   移動前の相対座標。x_cur - x_prevで求める。
      int dy   

  return:
      DIR_R       右 
      DIR_L       左
      DIR_U       上 
      DIR_D       下 
      DUR_I       不正な値 
 */
int pacman_check_dir(int dx,int dy)
{
  if( (dx==0) && (dy==1) )
    return DIR_D;
  if( (dx==1) && (dy==0) )
    return DIR_R;
  if( (dx==0) && (dy==-1) )
    return DIR_U;
  if( (dx==-1) && (dy==0) )
    return DIR_L;

  return DIR_I;
}


/*
  pacman_enemy_move()

  敵の移動（フィールド上の全敵を動かす）
  input:
        int t

  return:
        0         移動OK
        -EINVAL    エラー
 */
int pacman_enemy_move(int t)
{
  int enemy_id;
  int enemy_type;
  int x_cur,y_cur;
  int x_next,y_next;
  int x_prev,y_prev;
  int current_field;
  int dx,dy;

  for (enemy_id=0;enemy_id<nr_enemy;enemy_id++) {

    /* 過去、現在、次の座標初期値 */
    x_next = x_cur = enemy[enemy_id].x;
    y_next = y_cur = enemy[enemy_id].y;
    x_prev = enemy[enemy_id].x_prev;
    y_prev = enemy[enemy_id].y_prev;
    
    /* 敵のタイプ判定 */
    enemy_type = enemy[enemy_id].type; /* 初期設定タイプを設定 */

    /* 現在地のフィールド（行き止まり、通路、交差点） */
    current_field = pacman_field_map[y_cur][x_cur];

    /* 交差点以外の動作 */
    if( t == 0 ){    /* t=0のときは共通の動き */
      enemy_type = ENEMY_TYPE_0;
    } 
    else if(current_field == FIELD_IMPASSE) {  /* 行き止まりにいる */
      enemy_type = ENEMY_TYPE_IMPASSE;
    }
    else if(current_field == FIELD_PASSAGE) {  /* 通路にいる */
      enemy_type = ENEMY_TYPE_PASSAGE;
    }


    /* 交差点の動作 */

    if(enemy_type == ENEMY_TYPE_J) { /* 行動変化タイプの敵の処理 */
      /* --- 敵J ---
         交差点マスに入るたびに、最初は敵Lの行動、次回は敵Rの行動、さらに次回はまた敵Lの行動、と繰り返します。 */
      if(enemy[enemy_id].subtype == ENEMY_TYPE_L) {
        enemy_type = ENEMY_TYPE_L;  /* 今回は敵Lの行動 */
        enemy[enemy_id].subtype = ENEMY_TYPE_R;  /* 次回は敵Rの行動 */
      } else if(enemy[enemy_id].subtype == ENEMY_TYPE_R) {
        enemy_type = ENEMY_TYPE_R;  /* 今回は敵Rの行動 */
        enemy[enemy_id].subtype = ENEMY_TYPE_L;  /* 次回は敵Lの行動 */
      }
    }

    switch(enemy_type){
    case ENEMY_TYPE_0:
      /* 全敵共通
         t=0のときは初期位置の 下、左、上、右 の順で最初に進入可能なマスの方向に移動します。*/
    case ENEMY_TYPE_IMPASSE:
      /* 全敵共通
         行き止まりマスの場合、唯一進入可能な隣接マスに移動します。(初期動作と同じ判定でOK) */

      if(CHECKWALL(x_cur,y_cur+1)) y_next++;      /* 下に行けたら下に */
      else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左に行けたら左に */
      else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上に行けたら上に */
      else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右に行けたら右に */
      break;


    case ENEMY_TYPE_PASSAGE:
      /* 全敵共通
       通路マスの場合、時刻 t-1 に居たマス以外の進入可能な隣接するマスに移動します。 */
 
      if( !CHECKSAMEPOS(x_prev,y_prev,x_cur,y_cur+1) && CHECKWALL(x_cur,y_cur+1)) { /* 下に行けたら下に */
           y_next++;
      }else if( !CHECKSAMEPOS(x_prev,y_prev,x_cur-1,y_cur) && CHECKWALL(x_cur-1,y_cur)){ /* 左に行けたら左に */
           x_next--;
      }else if( !CHECKSAMEPOS(x_prev,y_prev,x_cur,y_cur-1) && CHECKWALL(x_cur,y_cur-1)){ /* 上に行けたら上に */
           y_next--;
      }else if( !CHECKSAMEPOS(x_prev,y_prev,x_cur+1,y_cur) && CHECKWALL(x_cur+1,y_cur)){ /* 右に行けたら右に */
           x_next++;
      }
      break;


    case ENEMY_TYPE_V:
      /* --- 敵V ---
         敵から見た自機の相対位置を (dx, dy) と表すものとします。
         次のルールを上から順に適用し、最初に選ばれた方向に移動します。
           1. dy ≠ 0 でかつ dy の符号方向にあるマスが進入可能であれば、その方向に移動します。
           2. dx ≠ 0 でかつ dx の符号方向にあるマスが進入可能であれば、その方向に移動します。
           3. 現在位置の 下、左、上、右 の順で最初に進入可能なマスの方向に移動する。 */
       dx = x_cur - pacman.x_prev;
       dy = y_cur - pacman.y_prev;

       if(dy != 0){
	 if ( (dy<0) && CHECKWALL(x_cur,y_cur+1)) { /* PAC-MANは下にいるので下に向かってみる。 */
	   y_next++;
	   break;
	 } else if( (dy>0) && CHECKWALL(x_cur,y_cur-1) ){ /* PAC-MANは上にいるので上に向かってみる。 */
	   y_next--;
	   break;
	 }
       }
       if( (dx != 0) && (y_cur == y_next)) {
	 if ( (dx<0) && CHECKWALL(x_cur+1,y_cur)) { /* PAC-MANは右にいるので右に向かってみる。 */
	   x_next++;
	   break;
	 } else if( (dx>0) && CHECKWALL(x_cur-1,y_cur) ){ /* PAC-MANは左にいるので左に向かってみる。 */
	   x_next--;
	   break;
	 }
       }
       if( (x_cur==x_next)&&(y_cur==y_next) ) {
	 if(CHECKWALL(x_cur,y_cur+1)) y_next++;      /* 下 */
	 else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
	 else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
	 else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
       }

       break;

    case ENEMY_TYPE_H:
      /* --- 敵H ---
         敵 V とほぼ同じです。唯一異なるのは 、進行方向を決めるルールのうち、
         最初の二つのルールの適用順序が入れ替わるところです。
         すなわち、先に dx ≠ 0 のチェックを行ない、次に dy ≠ 0 のチェックを行います。 */
       dx = x_cur - pacman.x_prev;
       dy = y_cur - pacman.y_prev;

       if(dx != 0) {
	 if ( (dx<0) && CHECKWALL(x_cur+1,y_cur)) { /* PAC-MANは右にいるので右に向かってみる。 */
	   x_next++;
	   break;
	 } else if( (dx>0) && CHECKWALL(x_cur-1,y_cur) ){ /* PAC-MANは左にいるので左に向かってみる。 */
	   x_next--;
	   break;
	 }
       }
       if( (dy != 0)  && (y_cur == y_next) ){
	 if ( (dy<0) && CHECKWALL(x_cur,y_cur+1)) { /* PAC-MANは下にいるので下に向かってみる。 */
	   y_next++;
	   break;
	 } else if( (dy>0) && CHECKWALL(x_cur,y_cur-1) ){ /* PAC-MANは上にいるので上に向かってみる。 */
	   y_next--;
	   break;
	 }
       }
       if( (x_cur==x_next)&&(y_cur==y_next) ) {
	 if(CHECKWALL(x_cur,y_cur+1)) y_next++;      /* 下 */
	 else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
	 else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
	 else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
       }
       break;

    case ENEMY_TYPE_L:
      /* --- 敵L ---
         現在位置への進入方向から見て相対的に 左、前、右 の順で最初に進入可能なマスの方向に移動します。 */
      switch(pacman_check_dir(x_cur - x_prev,y_cur - y_prev)){
      case DIR_U:  /* 上向き */
         if(CHECKWALL(x_cur-1,y_cur)) x_next--;      /* 左 */
         else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
         else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
         break;
      case DIR_R:  /* 右向き */
         if(CHECKWALL(x_cur,y_cur-1)) y_next--;      /* 上 */
         else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
         else if(CHECKWALL(x_cur,y_cur+1)) y_next++; /* 下 */
         break;
      case DIR_D: /* 下向き */
         if(CHECKWALL(x_cur+1,y_cur)) x_next++;      /* 右 */
         else if(CHECKWALL(x_cur,y_cur+1)) y_next++; /* 下 */
         else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
         break;
      case DIR_L:  /* 左向き */
         if(CHECKWALL(x_cur,y_cur+1)) y_next++;      /* 下 */
         else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
         else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
         break;
       default:
         fprintf(stderr,"pacman_enemy_move(): Error :invalid direction\n");
         break;
       }

      break;

    case ENEMY_TYPE_R:
      /* --- 敵R ---
         現在位置への進入方向から見て相対的に 右、前、左 の順で最初に進入可能なマスの方向に移動します。 */
      switch(pacman_check_dir(x_cur - x_prev,y_cur - y_prev)){
      case DIR_U:  /* 上向き */
         if(CHECKWALL(x_cur+1,y_cur)) x_next++;      /* 右 */
         else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
         else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
         break;
      case DIR_R:  /* 右向き */
         if(CHECKWALL(x_cur,y_cur+1)) y_next++;      /* 下 */
         else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
         else if(CHECKWALL(x_cur,y_cur-1)) y_next--; /* 上 */
         break;
      case DIR_D: /* 下向き */
         if(CHECKWALL(x_cur-1,y_cur)) x_next--;      /* 左 */
         else if(CHECKWALL(x_cur,y_cur+1)) y_next++; /* 下 */
         else if(CHECKWALL(x_cur+1,y_cur)) x_next++; /* 右 */
         break;
      case DIR_L:  /* 左向き */
         if(CHECKWALL(x_cur,y_cur-1)) y_next--;      /* 上 */
         else if(CHECKWALL(x_cur-1,y_cur)) x_next--; /* 左 */
         else if(CHECKWALL(x_cur,y_cur+1)) y_next++; /* 下 */
         break;
       default:
         fprintf(stderr,"pacman_enemy_move(): Error :invalid direction\n");
         break;
      } //switch(pacman_check_dir(x_cur - x_prev,y_cur - y_prev))

      break;

    default:
      fprintf(stderr,"Unknown enemy type %c\n",enemy[enemy_id].type);
      return -EINVAL;
    }
  /* 敵の位置情報を更新 */
  enemy[enemy_id].x_prev = x_cur;
  enemy[enemy_id].y_prev = y_cur;
  enemy[enemy_id].x = x_next;
  enemy[enemy_id].y = y_next;


  } //for (enemy_id=0;enemy_id<nr_enemy;enemy_id++) 

  return 0;
}


/*
  pacman_isalive()

  敵との当たり判定
  
  input:
    void

  return:
    0             生存（敵と接触しなかった）
    0以外の値      敵と接触      

 */
int pacman_isalive()
{
  int dead = 0;
  int id;

  for (id=0;id<nr_enemy;id++) {
    /* 重なった */
    if( (enemy[id].x == pacman.x) && (enemy[id].y == pacman.y))
      dead++;

    /* 敵と位置が入れ替わった */
    if( ((enemy[id].x_prev == pacman.x)&&(enemy[id].y_prev == pacman.y))
	&& ((enemy[id].x == pacman.x_prev)&&(enemy[id].y == pacman.y_prev)) )
      dead++;
  }
  
  return dead;
}

/* 
   pacman_stage_cleared()
   ステージクリア判定

   input:
     void

   return:
     0             ステージクリア！
     0以外の値      残りのドットの数

 */
int pacman_stage_cleared()
{
  return dot_counter;
}


/*
  pacman_close()
  終了処理
 */
pacman_close()
{
  return 0;
}

/*
  pacman.c
  GDD 2010 devquiz
  パックマンシミュレータテスト用モジュール

  Nishikawa Makoto

 */

#include <stdio.h>
#include <string.h>
#include "pacman-gdd2010-devquiz-map.h"


int main(int argc,char *argv[])
{
  int ret;
  int stageno;

  if(argc == 3){
  /* play_testモード
     $ ./pacman p <stage(1~3)> 
   */
    if(*argv[1]=='p'){
      stageno = atoi(argv[2]);
      printf("playtest:%d\n",stageno);
      ret = play_test(stageno);
    }
  }else if(argc == 4) {
  /* string_testモード
     $ ./pacman s <stage(1~3)> <string> 
   */
    if(*argv[1]=='s'){
      printf("stringtest:%s\n",argv[3]);
      stageno = atoi(argv[2]);
      printf("stage:%d\n",stageno);
      ret = string_test(stageno,argv[3]);
    }
  }

  return ret;
}



/* 
   keyinput()
   キー入力判定（テスト用）
 */
char keyinput(){
  int ch;
  while ( (ch = getchar()) < 0 )
    ;
  return (char)ch;
}


/* 
   play_test()
     プレイモード用関数

   input:
     play_stage :  埋め込み問題。1 - 3を選択
   return:
     0 : クリア
     1 : ゲームオーバー
     2 : タイムアウト
     -1 : 不正なステージ指定
 */
int play_test(int play_stage)
{
  int dots;
  int ret=0;
  int t=0;
  char ch;
  char key_str[1000];
  int timeout;

  printf("**** pacman play mode ****\n");

  switch(play_stage){
  case 1:
    pacman_init_map(pacman_stage1_W,pacman_stage1_H,pacman_stage1_data);
    timeout = pacman_stage1_T;
    break;
  case 2:
    pacman_init_map(pacman_stage2_W,pacman_stage2_H,pacman_stage2_data);
    timeout = pacman_stage2_T;
    break;
  case 3:
    pacman_init_map(pacman_stage3_W,pacman_stage3_H,pacman_stage3_data);
    timeout = pacman_stage3_T;
    break;
  default:
    return -1;
    break;
  }

  //  デバッグ情報表示用
    pacman_disp_stat();

  /* キーログ初期化 */
  key_str[0] = '\0';

  /* 画面表示 */
  pacman_disp_screen();
  printf("dots = %d\n",dots);

  while((dots = pacman_stage_cleared()) 
	&& (t < timeout) 
	&& (pacman_isalive()==0) ){

    /* キー入力 */
    printf("pacman(h=left,j=down,k=up,l=right)->");
    ch=keyinput();
    printf("\n");
    if( pacman_player_move(ch) < 0 ){  /* キー入力エラー */
      continue;
    } else {
      pacman_enemy_move(t);
      key_str[t]=ch;
      key_str[t+1]='\0';
      t++;
    }
    pacman_disp_screen();
    printf("dots = %d\n",dots);
    printf("t=%d\n",t);
    printf("keylog=%s\n",key_str); 
  }

  if(t==timeout){
    printf("timeout!\n");
    ret = 2;
  } else if (pacman_isalive() != 0){
    printf("miss!\n");
    ret = 1;
  } else{
    printf("clear!\n");
    ret = 0;
  }

  pacman_close();

  return ret;
}


/* 
   string_test()
     操作文字列テスト関数

   input:
     play_stage :  埋め込み問題。1 - 3を選択
     str        :  テスト用文字列

   return:
     0 : クリア
     1 : ゲームオーバー
     2 : タイムアウト
     3 : コマンド終了
     -1 : 不正なステージ指定
 */
int string_test(int play_stage, char *str)
{
  int dots;
  int ret=0;
  int t=0;
  char ch;
  char key_str[1000];
  int timeout;
  int comlength;
  int ch_count=0;

  printf("**** pacman play mode ****\n");

  switch(play_stage){
  case 1:
    pacman_init_map(pacman_stage1_W,pacman_stage1_H,pacman_stage1_data);
    timeout = pacman_stage1_T;
    break;
  case 2:
    pacman_init_map(pacman_stage2_W,pacman_stage2_H,pacman_stage2_data);
    timeout = pacman_stage2_T;
    break;
  case 3:
    pacman_init_map(pacman_stage3_W,pacman_stage3_H,pacman_stage3_data);
    timeout = pacman_stage3_T;
    break;
  default:
    return -1;
    break;
  }

  // 文字数カウント
  comlength = strlen(str);

  //  デバッグ情報表示用
  //  pacman_disp_stat();

  /* キーログ初期化 */
  key_str[0] = '\0';

  /* 画面表示 */
  pacman_disp_screen();
  printf("dots = %d\n",dots);

  while((dots = pacman_stage_cleared()) 
	&& (t < timeout) 
	&& (pacman_isalive()==0)
	&& (ch_count < comlength) ){

    ch = *str++;
    ch_count++;

    if( pacman_player_move(ch) < 0 ){  /* キー入力エラー */
      continue;
    } else {
      pacman_enemy_move(t);
      key_str[t]=ch;
      key_str[t+1]='\0';
      t++;
    }
    pacman_disp_screen();
    printf("dots = %d\n",dots);
    printf("t=%d\n",t);
    printf("keylog=%s\n",key_str); 
  }

  if(t==timeout){
    printf("timeout!\n");
    ret = 2;
  } else if (pacman_isalive() != 0){
    printf("miss!\n");
    ret = 1;
  } else if (ch_count == comlength){
    printf("end of command\n");
    ret = 3;
  } else {
    printf("clear!\n");
    ret = 0;
  }

  pacman_close();

  return ret;
}

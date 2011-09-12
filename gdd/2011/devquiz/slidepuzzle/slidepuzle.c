/*
 * slidepuzle.c
 *  スライドパズル for GDD devquiz 2011
 *                              by Makoto Nishikawa
 *
 * how to make:
 *   $ gcc -o slidepuzle slidepuzle.c
 *
 * how to use
 *   $ ./slidepuzle <Problem text file> <Start No> <End No>
 *
 */
#include <stdio.h>

/* デバッグ定義 */
#define DEBUGOUT 0

/* プロトタイプ宣言 */
int get_line_str(char *str, int count, char separator);
int solver(int width, int height, char *stagedata);
void disp_puzzle(int width, int height, char *stagedata);
void str2puzzlearray(int width, int height, char *stagedata);
void puzzlearray2str(int width, int height, char *stagedata);
void get_solved_str(int width, int height, char *solvedstr, char *stagedata);
int is_cleared(char *solvedstr, char *stagedata);
int next_procedure(char *procedure, int step);
int get_zero_point(int width, int height, int *zx, int *zy);
void disp_procedure(char *procedure, int nr_procedure);
int execute_puzzle_procedure(char *stagedata, char *solvedstr, int width, int height, char *procedure, int step);


/* 定数定義 */
#define LINE_BUFFER 1024    /* 行読み込みバッファサイズ */
#define MAX_PROCEDURE 20   /* 各パズルごとの最大手順数。これ以上探索を行わない。 */
#define MAX_WIDTH  6        /* パズルの最大幅 */
#define MAX_HEIGHT 6        /* パズルの最大高 */
#define CLEARED    0        /* クリア判定の戻り値 */
#define NOT_CLEARED   -1        /* クリアしてないときの戻り値 */
char panelchar[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";


/* 
  空白は、上下左右のマスのパネルと入れ替えることができます。
  上のマスのパネルと入れ替えることを U とよび、
  同様に、下左右のマスのパネルと入れ替えることを
  それぞれ D, L, R とよぶものとします。
  壁を空白やパネルと入れ替えることはできません。
  LX, RX, UX, DX: L, R, U, D それぞれの総数。
  これを超えたL,R,U,Dを使うと0点。
 */
int LX,RX,UX,DX;

/* 行読み込み用バッファ */
char strdata[4][LINE_BUFFER];

/* パズル処理用バッファ */
char puzzlearray[MAX_HEIGHT][MAX_WIDTH];


int main(int argc,char *argv[])
{
  FILE *fp;

  /* 問題開始番号，終了番号 */
  int start_no;
  int end_no;

  /* 問題行数　*/
  int nr_line;

  /* 文字列バッファ */
  char str_line[LINE_BUFFER];
  int q_no = 1;

  if (argc != 4) {
    fprintf(stderr,"Command option error!");
    fprintf(stderr,"how to use:\n");
    fprintf(stderr,"$ ./slidepuzle <Problem text file> <Start No> <End No>\n");
    return -1;
  }

  /* 問題データファイルのパス取得 */
  if ( (fp=fopen(argv[1], "r")) == NULL) {
    fprintf(stderr,"file open error : %s\n",argv[1]);
    return -1;
  }
  /* 問題開始番号の指定 */
  start_no = atoi(argv[2]);
  /* 問題開始番号の指定 */
  end_no = atoi(argv[3]);

#if DEBUGOUT
  printf("Problem File = %s\n",argv[1]);
  printf("Start No     = %d\n",start_no);
  printf("End No       = %d\n",end_no);
#endif

  /* LX, RX, UX, DX: 使うことができる L, R, U, D それぞれの総数を読み込み */
    fgets((char *)str_line, LINE_BUFFER ,fp);
    get_line_str(str_line, 4, ' ');
    LX = atoi(strdata[0]); RX = atoi(strdata[1]);
    UX = atoi(strdata[2]); DX = atoi(strdata[3]);

  /* 問題行数の読み込み */
  fgets((char *)str_line, LINE_BUFFER ,fp);
  nr_line = atoi(str_line);

#if DEBUGOUT
  printf("LX = %d\n", LX);
  printf("RX = %d\n", RX);
  printf("UX = %d\n", UX);
  printf("DX = %d\n", DX);
  printf("nr_line = %d\n", nr_line);
#endif

  while( q_no <= end_no ) {
    /* 行読み込み */
    fgets((char *)str_line, LINE_BUFFER ,fp);
    if( q_no < start_no ){
      q_no++;
      continue;
    }

#if DEBUGOUT
    printf("Question: %d\n", q_no);
#endif
    get_line_str(str_line,3,',');
    if(solver(atoi(strdata[0]),atoi(strdata[1]),strdata[2]) == NOT_CLEARED){
      printf("\n"); /* 解けなかったら改行出力 */
    }    

    q_no++;
  }

  /* 終了！ */
  fclose(fp);
  return 0;
}

/* セパレータで区切られたデータの読み込み */
int get_line_str(char *str, int count, char separator)
{
  int i;
  int strp=0;
  int bufp=0;
  char num_str[LINE_BUFFER];

  for(i=0;i<count;i++){
    while( (str[strp]!=separator)
	   && (str[strp]!='\0')
	   && (str[strp]!='\n') ){
      strdata[i][bufp] = str[strp];
      strp++;
      bufp++;
    }
    strdata[i][bufp] = '\0';
    strp++;
    bufp=0;    
  }

  return 0;
}


/* パズルを解く */
int solver(int width, int height, char *stagedata)
{
  int nr_procedure = 0;
  char procedure[MAX_PROCEDURE];
  char solvedstr[width*height+1];
  char prev_procedure;
  int zx,zy;

#if DEBUGOUT
  printf("width=%d\n",width);
  printf("height=%d\n",height);
  printf("stagedata:%s\n",stagedata);
  disp_puzzle(width,height,stagedata);
#endif

  get_solved_str(width, height, solvedstr, stagedata);

#if DEBUGOUT
  printf("solved:\n");
  disp_puzzle(width,height,solvedstr);
#endif

  /* いきなりクリアしてる場合は何もしない */
  if(is_cleared(solvedstr,stagedata) == CLEARED){
    return -1;
  }

  /* 初期設定 */
  nr_procedure = next_procedure(procedure, nr_procedure);
  prev_procedure = procedure[0];

  while(nr_procedure <= MAX_PROCEDURE) {

    /* パズルの実行。 */
    if(execute_puzzle_procedure(stagedata, solvedstr, width, height, 
				procedure, nr_procedure) == CLEARED) {
      /* クリアできたら手順を表示して呼び出し元に戻る */
      disp_procedure(procedure,nr_procedure);
      return CLEARED;
    }

    /* 次の手順を取得 */
    nr_procedure = next_procedure(procedure, nr_procedure);
  }

  return NOT_CLEARED;
}


/* パズル表示 */
void disp_puzzle(int width, int height, char *stagedata)
{
  int counter=0;
  int x=0;

  printf("------\n");
  for(;height>0;height--){
    for(x=0;x<width;x++){
      printf("%c",stagedata[counter]);
      counter++;
    }
    printf("\n");
  }
  printf("------\n");
}


/* 面文字列→配列変換 */
void str2puzzlearray(int width, int height, char *stagedata)
{
  int counter=0;
  int x=0, y=0;

  for(y=0;y<height;y++){
    for(x=0;x<width;x++){
      puzzlearray[y][x] = stagedata[counter];
      counter++;
    }
  }
}


/* 配列→面文字列 */
void puzzlearray2str(int width, int height, char *stagedata)
{
  int counter=0;
  int x=0, y=0;

  for(y=0;y<height;y++){
    for(x=0;x<width;x++){
      stagedata[counter] = puzzlearray[y][x];
      counter++;
    }
  }
  stagedata[counter] = '\0';
}


/* 整列後の文字列生成 */
void get_solved_str(int width, int height, char *solvedstr, char *stagedata)
{
  int nr_panel = width * height;
  int ptr;
  int counter = 0;

  for(ptr=0;ptr<(nr_panel-1);ptr++) {
    if(stagedata[ptr] == '='){
      solvedstr[ptr] = '=';
      counter++;
    } else {
    solvedstr[ptr] = panelchar[counter];
    counter++;
    }    
  }
  solvedstr[ptr] = '0';
  ptr++;
  solvedstr[ptr] = '\0';
}


/* クリア判定 */
int is_cleared(char *solvedstr, char *stagedata)
{
  return strcmp(solvedstr,stagedata);
}


/* 次の手順を生成 */
int next_procedure(char *procedure, int step)
{
  int next_step = step;
  int counter=0;
  int i;

  if(step == 0) {
    /* 初期値の設定 */
    next_step=1;
    procedure[0] = 'L';
  } else {
    /* 桁上がりチェック */
    for(i=0;i<step;i++) {
      if(procedure[i]=='D')
	counter++;
    }
    /* 桁上がり処理 */
    if(counter==step){
      next_step++;
      for(i=0;i<next_step;i++)
	procedure[i] = 'L';
    } else {
      for(i=0;i<next_step;i++){
	switch(procedure[i]) {
	case 'L':
	  procedure[i] = 'R';
	  i=next_step;
	  break;
	case 'R':
	  procedure[i] = 'U';
	  i=next_step;
	  break;
	case 'U':
	  procedure[i] = 'D';
	  i=next_step;
	  break;
	case 'D':
	  procedure[i] = 'L';
	  break;
	default:
	  procedure[i] = 'L';
	  break;
	}
      }
    }

  }
  return next_step;

}


/* パズル配列から空（0）の場所を探して座標を返す */
int get_zero_point(int width, int height, int *zx, int *zy)
{
  int x,y;
  for(y=0;y<height;y++) {
    for(x=0;x<width;x++) {
      if(puzzlearray[y][x]=='0'){
	*zx = x;
	*zy = y;
	return 0;
      }
    }
  }
  return -1;
}


/* 手順を表示する */
void disp_procedure(char *procedure, int nr_procedure)
{
  int i;
  for(i=0;i<nr_procedure;i++){
    printf("%c",procedure[i]);
  }
#if 0
  for(i=nr_procedure-1;i>=0;i--){
    printf("%c",procedure[i]);
  }
#endif

  printf("\n");
}


/* パズルの実行 */
int execute_puzzle_procedure(char *stagedata, char *solvedstr, int width, int height, char *procedure, int step)
{
  int zx,zy;
  int new_zx, new_zy;
  int i;
  char puzzlestr[width*height+1];
  char prev_procedure=' ';

  str2puzzlearray(width, height, stagedata);
  get_zero_point(width,height,&zx,&zy);
  new_zx = zx; new_zy = zy;

  for(i=0;i<step;i++){
    /* ゼロの移動先をチェック */
    /* 前の場所に戻るような手順はスキップする。 */
    switch(procedure[i]) {
    case 'L' :
      if(prev_procedure == 'R')
	return NOT_CLEARED;
      new_zx--;
      break;
    case 'R' :
      if(prev_procedure == 'L')
	return NOT_CLEARED;
      new_zx++;
      break;
    case 'U' :
      if(prev_procedure == 'D')
	return NOT_CLEARED;
      new_zy--;
      break;
    case 'D' :
      if(prev_procedure == 'U')
	return NOT_CLEARED;
      new_zy++;
      break;
    default :
      break;
    }
    prev_procedure = procedure[i];

    if( (new_zx<0) || (new_zx>=width) ||
	(new_zy<0) || (new_zy>=height) ||
	(puzzlearray[new_zy][new_zx] == '=') ) {
      /* ゼロの移動先がパズルの範囲外、壁の場合は実行中断してリターン */
      return NOT_CLEARED;
    }
    
    /* ゼロの移動 */
    puzzlearray[zy][zx] = puzzlearray[new_zy][new_zx];
    puzzlearray[new_zy][new_zx] = '0';
    zx = new_zx; zy = new_zy;
  }

  /* クリアのチェック */
  puzzlearray2str(width, height, puzzlestr);

  if(is_cleared(solvedstr, puzzlestr) == CLEARED)
    return CLEARED;
  else
    return NOT_CLEARED;

}



/*
 * hitori.c
 *  ひとりゲーム for GDD devquiz 2011
 *                              by Makoto Nishikawa
 *
 * how to make:
 *   $ gcc -o hitori hitori.c
 *
 * how to use:
 *   $ ./hitori <test case file path>
 *
 */
#include <stdio.h>


/* 
   ・数値の最大値は1,000,000(2進数で20桁)なので、
   　ずっと2で割って最後に5の倍数とゼロを取り除く最長手でも21手である。
   ・最後に必ず5の倍数を取り除く手が必要
   ・2で割る手を"1"、5で割る手を"0"として、回答手順を2進数で表現する。
     (ex)
       110 →2で2回割ったあと、5の倍数を取り除く
       01110 →5の倍数を取り除いたあと、2で3回割り、最後に5の倍数を取り除く。
     このビット列を0をスタートとし、2ずつ足していくことでテストすれば、
     最小手が求められる。（最大21ビット）
 */


#define NR_MAX        10    /* 各行のデータ個数は最大10個 */
#define STEP_MAX      21    /* 各数値の最大値は1,000,000なので、最大21ビット */
#define LINE_BUFFER   1024  /* 行バッファのサイズ */
#define NODATA        -1    /* 数値が取り払われた状態 */

unsigned int nr_line;       /* 読み込みライン数 */
unsigned int nr_data;       /* テストケースの個数 */
int data[NR_MAX];           /* 数値データ */


int main(int argc,char *argv[])
{
  FILE *fp;
  char str_line[LINE_BUFFER];
  int i;
  char procedure[STEP_MAX];
  int nodata_count;
  int step;
  int stepcount;
  int ans;

  /* テストケースのファイルパスの取得 */
  if (argc != 2) {
    fprintf(stderr,"file open error : %s\n",argv[1]);
    return -1;
  }
  if ( (fp=fopen(argv[1], "r")) == NULL) {
    fprintf(stderr,"file open error : %s\n",argv[1]);
    return -1;
  }

  /* テストケース数を読み込む */
  fgets((char *)str_line, LINE_BUFFER ,fp);
  nr_line = atoi(str_line);

  /* 回答計算ループ */
  while(nr_line > 0){

    /* テストケースのデータ個数取得 */
    fgets((char *)str_line, LINE_BUFFER ,fp);
    nr_data = atoi(str_line);

    /* テストケースの数値取得 */
    fgets((char *)str_line, LINE_BUFFER ,fp);
    get_data(str_line, nr_data);

    /* 計算 */
    for(i=0;i<STEP_MAX;i++)
      procedure[i] = 0; /* 5の倍数を取り除く（最低限の手数）から評価開始 */
    stepcount=1; /* 手数 */

    while(stepcount<STEP_MAX){  /* 最大手数まで評価すれば良い*/
      
      get_data(str_line, nr_data); /* data[]配列に数値をセット */
      nodata_count = 0; /* 数字が消えた數のカウント。これがデータ数と一致したら終了。 */
      step = 0;

      while(step<stepcount) {
	if(procedure[step]){
	  /* 2で割る */
	  for(i=0;i<nr_data;i++){
	    if(data[i]!=NODATA)
	      data[i] = data[i]/2;
	  }
	} else {
	  /* 5の倍数または0を取り除く。 */
	  /* また、この時点で数字がすべて無くなったかどうかも評価する。 */
	  for(i=0;i<nr_data;i++){
	    if( ((data[i]%5)==0) && (data[i]!=NODATA)){
	      data[i]=NODATA;
	      nodata_count++;
	    }
	    if(nodata_count==nr_data)
	      break;
	  }
	}
	step++;
      }

      if(nodata_count==nr_data)
	break;
      stepcount = next_procedure(stepcount,procedure); /* 次の手順に進める */

    }    
    ans = stepcount;

    /* 回答表示 */
    printf("%d\n",ans);

    nr_line--;
  }

  /* 終了！ */
  fclose(fp);
  return 0;
}

/* テストケースをdata配列に読み込む */
int get_data(char *str, int count)
{
  int i;
  int strp=0;
  int bufp=0;
  char num_str[LINE_BUFFER];

  for(i=0;i<count;i++){
    while( (str[strp]!=' ')
	   && (str[strp]!='\0')
	   && (str[strp]!='\n') ){
      num_str[bufp] = str[strp];
      strp++;
      bufp++;
    }
    num_str[bufp] = '\0';
    data[i] = atoi(num_str);
    strp++;
    bufp=0;    
  }

  return 0;
}


/* 次の手順をprocedure配列に返す */
int next_procedure(int stepcount, char *procedure)
{
  int new_stepcount;
  int counter;
  int i,j;

  new_stepcount = stepcount;

  /* 桁上がり処理。すべて1だったら手順を一つ増やす */
  counter=0; 
  for(i=0;i<stepcount;i++) { /* 最後の桁は必ず0なので、カウント対象外 */
    if(procedure[i])
      counter++;
  }
  if( (counter==(stepcount-1)) || (stepcount==1) ) { /* 桁上がり */
    new_stepcount++;
    for(i=0;i<new_stepcount;i++)
      procedure[i] = 0;
  } else {
    for(i=(stepcount-2);i>=0;i--) {
      if(procedure[i] == 0){
	procedure[i] = 1;
	break;
      }
      else if(procedure[i] == 1) {
	procedure[i] = 0;
      }
    }
  }
  
  return new_stepcount;
}

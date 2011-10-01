/*
 * coffee.c
 *  最高のコーヒー for Google Code Jam 2011
 *                              by Makoto Nishikawa
 *
 * how to make:
 *   $ gcc -o coffee coffee.c
 *
 * how to use:
 *   $ ./coffee <test case file path>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* 

ヘインの一日は朝のコーヒーを飲むことから始まります。

彼の手元には N 種類のコーヒーがあります。i 番目の種類のコーヒーは ci 杯分残っていて、今日から数えて ti 日目に消費期限を迎えます。彼は i 番目（1 ≤ i ≤ N）の種類のコーヒーを一杯飲むごとに si の満足度が得られます。消費期限の切れたコーヒーを飲むことはできません（ちょうど ti 日目であればそのコーヒーは飲むことができます）。たとえば ti = 1 であれば、今日中に飲んでしまうか、そのコーヒーをあきらめるかのどちらかにしなければなりません。

彼はコーヒーを一日に一杯だけ、朝だけにしか飲まないことにしています。もし手元に飲めるコーヒーがない日は、満足度を得ることはできません。

これらのコーヒーを飲むことで、今日から始めて K 日目までに彼は合計して最大でどれだけの満足度を得られるでしょうか。
入力

入力の最初の行はテストケースの個数 T です。そのあとに T 個のテストケースが続きます。それぞれのテストケースは 1 つのスペースで区切られた 2 つの正の整数が含まれる行から始まります。最初の整数はコーヒーの種類数 N を表し、次の整数は日数 K を表します。そのあとにそれぞれの種類のコーヒーの残数、消費期限、満足度を表す以下の形式の行が N 行続きます。

ci ti si

出力

各テストケースごとに、

Case #X: Y

と一行出力してください。ここで X は 1 から始まるテストケースの番号、Y はヘインの満足度の合計の最大値です。

制約

1 ≤ T ≤ 100
1 ≤ ci ≤ K
1 ≤ ti ≤ K
1 ≤ si ≤ 1000
Small

1 ≤ N ≤ 8
1 ≤ K ≤ 8
Large

1 ≤ N ≤ 100
1 ≤ K ≤ 2 * 1012 (32bit整数型には収まらないことに注意)
サンプル

入力
  	
出力
 
3
2 3
2 2 2
3 3 1
2 3
1 3 2
1 3 1
5 5
5 5 1
4 4 2
3 3 3
2 2 4
1 1 5


Case #1: 5
Case #2: 3
Case #3: 15

 */


#define DEBUG_OUT     1
#define LINE_BUFFER   1024  /* 行バッファのサイズ */
#define NR_MAX        4
#define MAX_TYPE      100   /* コーヒーの種類最大値 */
#define MAX_DAY       2*1000000000000 /* 最大日数 */

typedef unsigned long long u64;

/* プロトタイプ宣言 */
int get_data(char *str, int count);

u64 data[NR_MAX];           /* 数値データ */
int TEST_CASE = 0;          /* テストケースの数 */

int main(int argc,char *argv[])
{
  FILE *fp;
  char str_line[LINE_BUFFER];
  u64 coffee_type = 0;
  u64 days = 0;
  u64 coffee_remain[MAX_TYPE]; /* コーヒー残量 */
  u64 coffee_limit[MAX_TYPE];  /* 消費期限 */
  u64 coffee_taste[MAX_TYPE]; /* 満足度 */
  u64 answer;
  int test_case_no = 0;
  u64 i;

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
  TEST_CASE = atoi(str_line);

  test_case_no = 1;
  while(test_case_no <= TEST_CASE) {

    /* コーヒー情報を読み込む */
    fgets((char *)str_line, LINE_BUFFER ,fp);
    get_data(str_line,2);
    coffee_type = data[0];
    days = data[1];
#ifdef DEBUG_OUT
      printf("coffee_type=%llu\n",coffee_type);
      printf("days=%llu\n",days);
#endif
    for(i=0;i<coffee_type;i++) {
      fgets((char *)str_line, LINE_BUFFER ,fp);
      get_data(str_line,3);
      coffee_remain[i] = data[0];
      coffee_limit[i] = data[1];
      coffee_taste[i] = data[2];
#ifdef DEBUG_OUT
      printf("coffee_remain=%llu\n",coffee_remain[i]);
      printf("coffee_limit=%llu\n",coffee_limit[i]);
      printf("coffee_taste=%llu\n",coffee_taste[i]);
#endif
    }

    

    /* 解答表示 */
    printf("Case #%d: %llu\n",test_case_no,answer);
    /* 次のテストケースへ */
    test_case_no++;
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
  char *endptr;

  for(i=0;i<count;i++){
    while( (str[strp]!=' ')
	   && (str[strp]!='\0')
	   && (str[strp]!='\n') ){
      num_str[bufp] = str[strp];
      strp++;
      bufp++;
    }
    num_str[bufp] = '\0';
    data[i] = (u64)strtoull(num_str,&endptr,10);
    strp++;
    bufp=0;    
  }

  return 0;
}

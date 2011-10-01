/*
 * bitnum.c
 *  ビット数 for Google Code Jam 2011
 *                              by Makoto Nishikawa
 *
 * how to make:
 *   $ gcc -o bitnum bitnum.c
 *
 * how to use:
 *   $ ./bitnum <test case file path>
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* 
f(x) を 「x を 2進数表記した時の "1" の個数を返す関数」と定義します。例えば、5 は 1012 と 2 進数表記されるので、f(5) = 2 です。

正の整数 N が与えられるので、 a + b = N を満たす 0 以上の整数 a, b の組の中で f(a) + f(b) が最大になるものを求め、その時の f(a) + f(b) の値を出力してください。
入力

最初の行はテストケースの個数 T を表す正の整数です。各テストケースは1行の文字列で表現され、それぞれの行には N を表す正の整数が1つだけ含まれています。
出力

各テストケースごとに、

Case #X: P

という内容を1行出力してください。ここで X は 1 から始まるテストケース番号、P は f(a) + f(b) の最大値です。

制約

1 ≤ T ≤ 1000
Small

N ≤ 10000
Large

N ≤ 1018 (32bit整数型には収まらないことに注意)
サンプル

入力
  	
出力
 
4
1
4
31
1125899906842624
	Case #1: 1
Case #2: 3
Case #3: 5
Case #4: 51

 */


//#define DEBUG_OUT     1
#define NR_MAX        10    /* 各行のデータ個数は最大10個 */
#define LINE_BUFFER   1024  /* 行バッファのサイズ */

typedef unsigned long long u64;

/* プロトタイプ宣言 */
int bitcount(u64 data);
u64 get_max_bits_value(u64 in_value);


int data[NR_MAX];           /* 数値データ */
int TEST_CASE = 0;          /* テストケースの数 */

int main(int argc,char *argv[])
{
  FILE *fp;
  char str_line[LINE_BUFFER];
  char *endptr;
  u64 value;
  u64 value_a;
  u64 value_b;
  int answer;
  int test_case_no = 0;


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
    fgets((char *)str_line, LINE_BUFFER ,fp);
    //    value = (u64)atoll(str_line);
    value = (u64)strtoull(str_line,&endptr,10);
#ifdef DEBUG_OUT
    printf("value_str=%s\n",str_line);
    printf("value=%llx\n",value);
    printf("bitcount=%d\n",bitcount(value));
#endif

    value_a = get_max_bits_value(value);
    value_b = value - value_a;
    answer = bitcount(value_a) + bitcount(value_b);

#ifdef DEBUG_OUT
    printf("value_a=%llx\n",value_a);
    printf("value_b=%llx\n",value_b);
    printf("answer=%d\n",answer);
#endif

    /* 解答表示 */
    printf("Case #%d: %d\n",test_case_no,answer);

    /* 次のテストケースへ */
    test_case_no++;
  }

  /* 終了！ */
  fclose(fp);
  return 0;
}

/* 2進数表記で1になる数をカウント */
int bitcount(u64 data)
{
  int nr_bit = 0;
  u64 bit = 1UL;
  int i;
  int bitsize = sizeof(u64)*8;

  for(i=0;i<bitsize;i++) {
    if(data & bit)
      nr_bit++;
    bit<<=1;
  }

  return nr_bit;
}

/* 引数に与えた数値より小さい
　　2進数表記で最も1が多い数値を返す */
u64 get_max_bits_value(u64 in_value)
{
  u64 out_value = 1UL;
  u64 bit = 1UL;
  int bitsize = sizeof(u64)*8;
  int i;
  
  for(i=0;i<bitsize;i++) {
    bit<<=1;
    bit+=1UL;
    if(bit >= in_value)
      return out_value;
    out_value = bit;
  }
  return out_value;
}

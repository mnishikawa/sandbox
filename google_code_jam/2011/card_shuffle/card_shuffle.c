/*
 * card_shuffle.c
 *  カードシャッフル for Google Code Jam 2011
 *                              by Makoto Nishikawa
 *
 * how to make:
 *   $ gcc -o card_shuffle card_shuffle.c
 *
 * how to use:
 *   $ ./card_shuffle <test case file path>
 *
 */
#include <stdio.h>


/* 
フランクはカードゲームが好きで、週末は友達の家でゲームパーティーに参加しています。彼らがゲームに使うカードは M 枚からなり、それぞれ 1 から M までの数字が重複しないように書かれています。フランクはパーティーで友人が使っている自動カードシャッフル装置と同じものを持っていて、どのように動作するか理解しています。その装置はカードの山を C 回カットすることでシャッフルを行います。i 回目のカットではカードの山の上から Ai 番目から Bi 枚、つまり Ai 番目から Ai + Bi - 1 番目のカードがそのままの順番で山の上に移動します。

ある日、いつも使っているカードが汚れたため、新しいカードを使うことになりました。新しいカードは上から順番に 1 から M まで並んだ状態でそのままシャッフル装置にかけられました。フランクはシャッフル装置の性質を利用し、シャッフル後に上から W 番目にあるカードが何かを知ろうとしています。
入力

最初の行はテストケースの個数 T を表す正の整数です。続いて、各テストケースが次のようなフォーマットで与えられます。

M C W
A1 B1
...
AC BC

1行目では、1 つのスペースで区切られた 3 つの整数 M, C, W が与えられます。ここで M はカードの枚数 、C はカットの回数、W は知りたいカードの位置です。続く C 行の各行では、1 つのスペースで区切られた 2 つの整数 Ai, Bi が与えられます。ここで Ai, Bi はカットの操作で、i 回目の操作で上から Ai 番目から Bi 枚のカードを山の上に移動させることを意味しています。

出力

各テストケースに対し、

Case #X: P

という内容を1行出力してください。ここで X は 1 から始まるテストケース番号、P はシャッフル後のカードの山の上から W 番目にあるカードを表します。

制約

1 ≤ T ≤ 200
1 ≤ C ≤ 100
1 ≤ W ≤ M
1 ≤ Ai ≤ M
1 ≤ Bi ≤ M
1 ≤ Ai + Bi - 1 ≤ M
Small

1 ≤ M ≤ 100
Large

1 ≤ M ≤ 109
サンプル

入力
  	
出力
 
3
1 1 1
1 1
2 3 1
2 1
2 1
2 1
5 3 2
4 2
5 1
4 2

Case #1: 1
Case #2: 2
Case #3: 2

 */

//#define DEBUG_OUT 1
#define BUFFER_SIZE 1024  /* バッファサイズ */
#define NR_CARD_MAX 1000000000 /* 最大カード数(10^9) */
//#define NR_CARD_MAX 100 /* 最大カード数(10^9) */


/* プロトタイプ宣言 */
int get_data(char *str, int count);
void disp_cards(int card_array_no);

/* グローバル変数定義 */
int TEST_CASE = 0;   /* テストケース数 */
int NR_CARDS = 0;  /* カード数 */
int NR_SHUFFLE = 0;  /* シャッフル回数 */
int CARD_NO = 0;  /* 選択カード番号 */
int data[BUFFER_SIZE];  /* 行読み込み用バッファ */
unsigned int card_array[2][NR_CARD_MAX]; /* カード配列データ */


int main(int argc,char *argv[])
{
  FILE *fp;
  unsigned int i;
  char str_line[BUFFER_SIZE];  /* 文字列読み込み用バッファ */
  int cut_start_no = 0;  /* シャッフル開始位置番号読み込み用 */
  int nr_cut = 0;    /* シャッフル時のカット数読み込み用 */
  int current_card_array = 0; /* 今使ってるカード山の番号 */
  int new_card_array = 1; /* 次のカード山の番号 */
  int new_card_no = 0;    /* カード並び替えカウント用 */
  int answer = 0;       /* 解答 */
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
  fgets((char *)str_line, BUFFER_SIZE ,fp);
  TEST_CASE = atoi(str_line);
#ifdef DEBUG_OUT
  printf("test case = %d\n",TEST_CASE);
#endif

  test_case_no = 1;
  while(test_case_no <= TEST_CASE){
    /* カード数（NR_CARDS），シャッフル回数(NR_SHUFFLE)
       ，選択カード番号（CARD_NO） の読み込み */
    fgets((char *)str_line, BUFFER_SIZE ,fp);
    get_data(str_line,3);
    NR_CARDS = data[0];
    NR_SHUFFLE = data[1];
    CARD_NO = data[2];
#ifdef DEBUG_OUT
    printf("NR_CARDS = %d\n",NR_CARDS);
    printf("NR_SHUFFLE = %d\n",NR_SHUFFLE);
    printf("CARD_NO = %d\n",CARD_NO);
#endif
    /* カード配列の作成 */
    for(i=1;i<=NR_CARDS;i++) {
      card_array[0][i] = i;
      card_array[1][i] = i;
    }

    while(NR_SHUFFLE>0) {
      /* シャッフル（カット処理） */
      fgets((char *)str_line, BUFFER_SIZE ,fp);
      get_data(str_line,2);
      cut_start_no = data[0];
      nr_cut = data[1];
#ifdef DEBUG_OUT
      printf("cut_start_no = %d\n",cut_start_no);
      printf("nr_cut = %d\n",nr_cut);
#endif

      new_card_no = 1;
      if(new_card_no <= NR_CARDS) {
	/* カット分のカード束が新しい山の先頭に来る。 */
	for(i=cut_start_no; i<(cut_start_no + nr_cut); i++) {
	  card_array[new_card_array][new_card_no++]
	    = card_array[current_card_array][i];
	}
      }
      if(new_card_no <= NR_CARDS) {
      /* 次に、カット分の前のカード束が来る。 */
	for(i=1;i<cut_start_no;i++) {
	  card_array[new_card_array][new_card_no++]
	    = card_array[current_card_array][i];
	}
      }
      if(new_card_no <= NR_CARDS) {
	/* 次に、カット分の後のカード束が来る。 */
	for(i=(cut_start_no + nr_cut);i<=NR_CARDS;i++) {
	  card_array[new_card_array][new_card_no++]
	    = card_array[current_card_array][i];
	}
      }
#ifdef DEBUG_OUT
      disp_cards(new_card_array);
#endif
      /* この時点での指定カードの番号 */
      answer = card_array[new_card_array][CARD_NO];
      /* カード山の変更 */
      if(current_card_array==0){
	current_card_array=1;
	new_card_array=0;
      } else  {
	current_card_array=0;
	new_card_array=1;
      }

      /* 次のシャッフルへ */
      NR_SHUFFLE--;
    }

    /* 解答表示 */
    printf("Case #%d: %d\n",test_case_no,answer);

    /* 次のテストケースへ。 */
    test_case_no++;
  }
  /* 終了処理 */
  fclose(fp);
  return 0;

}



/* テストケースをdata配列に読み込む */
int get_data(char *str, int count)
{
  int i;
  int strp=0;
  int bufp=0;
  char num_str[BUFFER_SIZE];

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


/* カード配列表示 */
void disp_cards(int card_array_no)
{
  int i;

  for(i=1;i<=NR_CARDS;i++) {
    printf("%d ",card_array[card_array_no][i]);
  }
  printf("\n");

}

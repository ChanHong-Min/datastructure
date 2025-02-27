#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SORT_BY_WORD    0 // 단어 순 정렬
#define SORT_BY_FREQ    1 // 빈도 순 정렬
#define MAX_WORD_LEN    100 // 최대 단어 길이

// 구조체 선언
// 단어 구조체
typedef struct {
    char    *word;      // 단어
    int     freq;       // 빈도
} tWord;

// 사전(dictionary) 구조체
typedef struct {
    int     len;        // 배열에 저장된 단어의 수
    int     capacity;   // 배열의 용량 (배열에 저장 가능한 단어의 수)
    tWord   *data;      // 단어 구조체 배열에 대한 포인터
} tWordDic;

// 함수 원형 선언
void word_count(FILE *fp, tWordDic *dic);
void print_dic(tWordDic *dic);
void destroy_dic(tWordDic *dic);
int compare_by_word(const void *n1, const void *n2);
int compare_by_freq(const void *n1, const void *n2);
int binary_search(const char *key, const tWord *base, int nmemb, int *found);

// 단어를 사전에 저장
void word_count(FILE *fp, tWordDic *dic) {
    char word[MAX_WORD_LEN];

    while (fscanf(fp, "%s", word) != EOF) {
        int found = 0;
        int index = binary_search(word, dic->data, dic->len, &found);

        if (found) {
            // 이미 있는 단어의 빈도를 증가
            (dic->data + index)->freq++;
        } else {
            // 새로운 단어를 추가
            if (dic->len >= dic->capacity) {
                // 사전의 용량이 부족할 경우 용량을 늘림
                dic->capacity += 1000;
                dic->data = (tWord *)realloc(dic->data, dic->capacity * sizeof(tWord));
            }
            // 삽입할 위치부터의 데이터들을 이동시킴
            memmove(dic->data + index + 1, dic->data + index, (dic->len - index) * sizeof(tWord));
            // 새로운 단어 삽입
            (dic->data + index)->word = strdup(word); 
            (dic->data + index)->freq = 1; //빈도 업데이트
            dic->len++; //배열에 저장된 단어 수 1 증가
        }
    }
}

// 사전을 화면에 출력
void print_dic(tWordDic *dic) {
    for (int i = 0; i < dic->len; i++) {
        printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq); //(dic->data+i)->word와 동일
    }
}

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic) {
    for (int i = 0; i < dic->len; i++) {
        free(dic->data[i].word);
    }
    free(dic->data);
    free(dic);
}

// qsort를 위한 비교 함수 (정렬 기준: 단어)
int compare_by_word(const void *n1, const void *n2) {
    return strcmp(((tWord *)n1)->word, ((tWord *)n2)->word);
}

// qsort를 위한 비교 함수 (정렬 기준: 빈도 내림차순)
int compare_by_freq(const void *n1, const void *n2) {
    int freq_diff = ((tWord *)n2)->freq - ((tWord *)n1)->freq;
    if (freq_diff != 0) {
        return freq_diff;
    } else {
        return compare_by_word(n1, n2);
    }
}

// 이진탐색 함수
int binary_search(const char *key, const tWord *base, int nmemb, int *found) {
    int left = 0;
    int right = nmemb - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = strcmp(key, (base + mid)->word);
        if (cmp == 0) {
            *found = 1;
            return mid;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    *found = 0;
    return left;
}

// 사전을 초기화 (빈 사전을 생성, 메모리 할당)
tWordDic *create_dic(void) {
    tWordDic *dic = (tWordDic *)malloc(sizeof(tWordDic));
    dic->len = 0;
    dic->capacity = 1000;
    dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));
    return dic;
}

int main(int argc, char **argv) {
    tWordDic *dic;
    FILE *fp;
    int option;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s option FILE\n\n", argv[0]);
        fprintf(stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
        return 1;
    }

    if (strcmp(argv[1], "-n") == 0) option = SORT_BY_WORD;
    else if (strcmp(argv[1], "-f") == 0) option = SORT_BY_FREQ;
    else {
        fprintf(stderr, "unknown option : %s\n", argv[1]);
        return 1;
    }

    // 사전 초기화
    dic = create_dic();

    // 입력 파일 열기
    if ((fp = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "cannot open file : %s\n", argv[2]);
        return 1;
    }

    // 입력 파일로부터 단어와 빈도를 사전에 저장
    word_count(fp, dic);
    fclose(fp);

    // 정렬 (빈도 내림차순, 빈도가 같은 경우 단어순)
    if (option == SORT_BY_FREQ) {
        qsort(dic->data, dic->len, sizeof(tWord), compare_by_freq);
    }

    // 사전을 화면에 출력
    print_dic(dic);

    // 사전 메모리 해제
    destroy_dic(dic);

    return 0;
}


#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, qsort
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

// 사전(dictionary) 구조체
typedef struct {
	int		len;		// 배열에 저장된 단어의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 단어의 수)
	tWord	*data;		// 단어 구조체 배열에 대한 포인터
} tWordDic;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
// 단어를 찾아 인덱스 반환
// 단어가 존재하지 않으면 -1 반환
int find_word(tWordDic *dic, const char *word) {
	for (int i = 0; i < dic->len; i++) {
		if (strcmp(dic->data[i].word, word) == 0) {
			return i; // 단어를 찾은 경우 인덱스 반환
		}
	}
	return -1; // 단어를 찾지 못한 경우 -1 반환
}

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count(FILE *fp, tWordDic *dic) {
    char buffer[100]; // 단어를 읽을 버퍼
    int freq; // 현재 단어의 빈도
    int index; // 사전에서 단어를 찾은 인덱스

    // 단어를 사전에 저장
    // 새로 등장한 단어는 사전에 추가
    // 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
    // capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
    while (fscanf(fp, "%s %d", buffer, &freq) != EOF) {
        // 단어를 찾아 인덱스 반환
        index = find_word(dic, buffer);

        // 단어가 이미 사전에 존재하는 경우
        if (index != -1) {
            // 빈도 업데이트
            dic->data[index].freq++;
        } else { // 단어가 새로운 경우
			// 사전의 용량(capacity)을 확인하여 필요에 따라 확장
            if (dic->len >= dic->capacity) {
                dic->capacity += 1000;
                tWord *temp = realloc(dic->data, dic->capacity * sizeof(tWord));
                if (temp == NULL) {
                    fprintf(stderr, "Memory allocation error!\n");
                    exit(1);
                }
                dic->data = temp;
            }
            // 새로운 단어를 사전에 추가
            dic->data[dic->len].word = strdup(buffer);
            dic->data[dic->len].freq = 1;
            dic->len++; // 사전의 길이 증가
        }
    }
}

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic(tWordDic *dic){
	for (int i = 0; i < dic->len; i++) {
        printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq);
    }
    fflush(stdout); // 출력 버퍼 비우기
}

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic) {
    // 단어 문자열 해제
    for (int i = 0; i < dic->len; i++) {
        free(dic->data[i].word);
    }

    // 배열 해제
    free(dic->data);

    // 사전 구조체 해제
    free(dic);
}

// qsort를 위한 비교 함수 (정렬 기준: 단어)
int compare_by_word(const void *n1, const void *n2) {
    // n1과 n2를 tWord 포인터로 변환
    const tWord *word1 = (const tWord *)n1;
    const tWord *word2 = (const tWord *)n2;

    // 대소문자 구분 없이 단어를 비교하여 반환
    return strcasecmp(word1->word, word2->word);
}

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq(const void *n1, const void *n2) {
    // n1과 n2를 tWord 포인터로 변환
    const tWord *word1 = (const tWord *)n1;
    const tWord *word2 = (const tWord *)n2;

    // 빈도를 비교하여 내림차순으로 정렬
    if (word1->freq > word2->freq) {
        return -1;
    } else if (word1->freq < word2->freq) {
        return 1;
    } else { // 빈도가 같을 경우 단어를 알파벳 순으로 정렬
        return strcasecmp(word1->word, word2->word);
    }
}

// 사전을 초기화 (빈 사전을 생성, 메모리 할당)
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tWordDic *create_dic(void)
{
	tWordDic *dic = (tWordDic *)malloc(sizeof(tWordDic));

	dic->len = 0;
	dic->capacity = 1000;
	dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));

	return dic;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tWordDic *dic;
	int option;
	FILE *fp;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf(stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp(argv[1], "-n") == 0)
		option = SORT_BY_WORD;
	else if (strcmp(argv[1], "-f") == 0)
		option = SORT_BY_FREQ;
	else {
		fprintf(stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	// 사전 초기화
	dic = create_dic();

	// 입력 파일 열기
	if ((fp = fopen(argv[2], "r")) == NULL)
	{
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

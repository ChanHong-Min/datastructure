#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// User structure type definition
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tWord		*dataPtr;
	struct node	*link; // 단어순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 단어순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
int compare_by_freq( const void *n1, const void *n2);
tWord *createWord(char *word);
void destroyWord(tWord *pNode);

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) {
    LIST *list = (LIST *)malloc(sizeof(LIST));
    if (list) { //메모리가 할당되었다면
        list->count = 0;
        list->head = NULL;
        list->head2 = NULL;
    }
    return list;
}

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList(LIST *pList) { 
    if (!pList) return; // 리스트 포인터가 유효한지 확인

    // 단어 순 리스트의 노드 메모리 해제
    NODE *pCur = pList->head;
    while (pCur) {
        NODE *pNext = pCur->link;

        destroyWord(pCur->dataPtr); // 먼저 단어 구조체 내의 메모리를 해제
        free(pCur); // 그 후에 노드의 메모리를 해제
        pCur = pNext;
    }
	 pList->count = 0;
    // 리스트 구조체 자체의 메모리 해제
    free(pList);
	
}


// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu) {
    *pPre = NULL;
    *pLoc = pList->head;

    while (*pLoc) {
        int cmp = strcmp((*pLoc)->dataPtr->word, pArgu->word);
        if (cmp >= 0) break;

        *pPre = *pLoc;
        *pLoc = (*pLoc)->link;
    }

    if (*pLoc && strcmp((*pLoc)->dataPtr->word, pArgu->word) == 0) return 1; //단어를 찾으면 1 반환
    return 0;
}


static int _search_by_freq(LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu) {
    *pPre = NULL;
    *pLoc = pList->head2; 

    while (*pLoc) {
        int cmp = compare_by_freq((*pLoc)->dataPtr, pArgu);
        if (cmp > 0) break;

        *pPre = *pLoc;
        *pLoc = (*pLoc)->link2;
    }

    if (*pLoc && compare_by_freq((*pLoc)->dataPtr, pArgu) == 0) return 1;
    return 0;
}

// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert(LIST *pList, NODE *pPre, tWord *dataInPtr) {
    NODE *pNew = (NODE *)malloc(sizeof(NODE));
    if (!pNew) return 0;

    pNew->dataPtr = dataInPtr;

    if (!pPre) { //새로운 노드를 리스트 맨 앞에 삽입하는 경우
        pNew->link = pList->head;
        pList->head = pNew;
    } else {
        pNew->link = pPre->link;
        pPre->link = pNew;
    }

    pList->count++;
    return 1;
}


// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic(LIST *list, char *word) {
    tWord *newWord = createWord(word);
    if (!newWord) return;

    NODE *pPre, *pLoc;
    int found = _search(list, &pPre,&pLoc, newWord);
    if (found) { //이미 사전에 존재하는 단어 빈도 갱신
        pLoc->dataPtr->freq++;
        destroyWord(newWord);
    } else { //새로 등장한 단어 사전에 추가
        _insert(list, pPre, newWord);
    }
}



// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq(LIST *pList, NODE *pPre, NODE *pLoc) {
    if (!pPre) { //삽입할 노드가 맨 앞에 삽입되는 경우
        pLoc->link2 = pList->head2;
        pList->head2 = pLoc;
    } else { //삽입할 노드가 중간 또는 끝에 삽입되는 경우
        pLoc->link2 = pPre->link2;
        pPre->link2 = pLoc;
    }
}


// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency(LIST *list) {
    list->head2 = NULL; // 빈도순 리스트 초기화
    NODE *pCurWord = list->head; // 단어순 리스트의 현재 노드를 가리키는 포인터

    while (pCurWord != NULL) {
        NODE *pPreFreq, *pLocFreq;
        _search_by_freq(list, &pPreFreq, &pLocFreq, pCurWord->dataPtr);
        _link_by_freq(list, pPreFreq, pCurWord); // 빈도순 리스트에 삽입
        pCurWord = pCurWord->link; // 다음 단어로 이동
    }
}
    
    

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic(LIST *pList) {
    NODE *pCur = pList->head;
    while (pCur) {
        printf("%s\t%d\n", pCur->dataPtr->word, pCur->dataPtr->freq);
        pCur = pCur->link;
    }
} // 단어순

void print_dic_by_freq(LIST *pList) {
    NODE *pCur = pList->head2;
    while (pCur) {
        printf("%s\t%d\n", pCur->dataPtr->word, pCur->dataPtr->freq);
        pCur = pCur->link2;
    }
}
 // 빈도순

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord(char *word) {
    tWord *newWord = (tWord *)malloc(sizeof(tWord));
    if (!newWord) return NULL;

    newWord->word = strdup(word);
    if (!newWord->word) {
        free(newWord);
        return NULL;
    }

    newWord->freq = 1;
    return newWord;
}


//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord(tWord *pNode) {
    if (!pNode) return;
    free(pNode->word);
    free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}
////////////////////////////////////////////////////////////////////////////////
// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	int ret = (int) p2->freq - p1->freq;
	
	if (ret != 0) return ret;
	
	return strcmp( p1->word, p2->word); //빈도가 동일한 경우
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	char word[1000];
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_WORD;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	// creates an empty list
	list = createList();
	
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		// 사전(단어순 리스트) 업데이트
		update_dic( list, word);
	}
	
	fclose( fp);

	if (option == SORT_BY_WORD) {
		
		// 단어순 리스트를 화면에 출력
		print_dic( list);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_dic_by_freq( list);
	}
	
	// 단어 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}


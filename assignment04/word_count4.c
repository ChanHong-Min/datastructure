#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

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
	struct node	*llink; // backward pointer
	struct node	*rlink; // forward pointer
} NODE;

typedef struct
{
	int		count;
	NODE	*head; //맨 앞 가리키는 포인터
	NODE	*rear; //맨 뒤 가리키는 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
void destroyWord(tWord *pNode);
LIST *createList(void);
void destroyList(LIST *pList);
int addNode(LIST *pList, tWord *dataInPtr);
int removeNode(LIST *pList, tWord *keyPtr, tWord **dataOutPtr);
int searchNode(LIST *pList, tWord *pArgu, tWord **dataOutPtr);
int countList(LIST *pList);
int emptyList(LIST *pList);
void traverseList(LIST *pList, void (*callback)(const tWord *));
void traverseListR(LIST *pList, void (*callback)(const tWord *));
static int _insert(LIST *pList, NODE *pPre, tWord *dataInPtr);
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr);
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);
tWord *createWord(char *word);
int get_action();
int compare_by_word(const void *n1, const void *n2);
void print_word(const tWord *dataPtr);
void input_word(char *word);

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) {
    LIST *pList = (LIST *)malloc(sizeof(LIST)); // 리스트 구조체에 대한 메모리 할당
    
    if (pList != NULL) {
        pList->count = 0; // 리스트의 노드 개수 초기화
        pList->head = NULL; // 리스트의 헤드를 NULL로 초기화
        pList->rear = NULL; // 리스트의 리어를 NULL로 초기화
    }
    
    return pList; // 생성된 리스트 구조체 포인터 반환
}


//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList(LIST *pList) {
    NODE *pDel;
    
    //별해 while (pList->head != NULL) {
        // pDel = pList->head; // 삭제할 노드를 가리키는 포인터에 헤드 노드 주소 할당
        // pList->head = pList->head->rlink; // 헤드를 다음 노드로 이동
        // destroyWord(pDel->dataPtr); // 단어 구조체의 메모리 해제
        // free(pDel); // 노드의 메모리 해제
    // }
	for(int i=0; i<pList->count; i++){
		pDel=pList->head;
		pList->head=pDel->rlink;
		destroyWord(pDel->dataPtr);
		free(pDel);
	}
   
	pList->count=0;
	pList->head=NULL;
	pList->rear=NULL;
    free(pList); // 리스트 구조체의 메모리 해제
}

// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가)
//노드 하나 만들고 단어를 연결해서 넣겠지 return 값 3개
//중복된 단어의 경우 추가가 아닌 빈도만 증가시켜(내부에서) return 값은 2
int addNode(LIST *pList, tWord *dataInPtr) {
    NODE *pPre, *pLoc;
    
    // _search 함수를 사용하여 중복 단어가 있는지 확인
    if (_search(pList, &pPre, &pLoc, dataInPtr)) {
        // 중복된 단어인 경우
        (pLoc->dataPtr->freq)++; // 빈도 증가
        return 2; // 중복 반환
    }
    
    // 중복이 아닌 경우 노드 추가
    if (!_insert(pList, pPre, dataInPtr)) return 0; // 메모리 오버플로우인 경우 실패 반환
    
    return 1; // 성공 반환
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr){

		NODE *pPre;
		NODE *pLoc=pList->head;
		
		// searchNode 함수를 사용하여 삭제할 노드 위치 찾기
		if(_search(pList, &pPre, &pLoc, keyPtr)){
			_delete(pList, pPre, pLoc, dataOutPtr);
			return 1;
		}
		
		else return 0;
		
}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found


int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr){
	NODE *pPre, *pLoc;
	
	if(!_search(pList, &pPre, &pLoc, pArgu)) return 0;
	
	*dataOutPtr=pLoc->dataPtr;
	
	return 1;
	
}

// returns number of nodes in list
int countList( LIST *pList){
		return pList->count; // 리스트의 노드 개수 반환
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
		return pList->count==0 ? 1 : 0; // 리스트가 비어있으면 1, 그렇지 않으면 0 반환
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tWord *)){
		NODE *pLoc=pList->head;
		
		while(pLoc !=NULL){
				callback(pLoc->dataPtr);
				pLoc=pLoc->rlink;
		}
}	

// traverses data from list (backward)
void traverseListR(LIST *pList, void (*callback)(const tWord *)) {
    NODE *pLoc = pList->rear; // 리어 노드부터 시작
    
    while (pLoc != NULL) {
        callback(pLoc->dataPtr); // 콜백 함수 호출하여 노드의 데이터 출력
        pLoc = pLoc->llink; // 이전 노드로 이동
    }
}

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr){
	NODE *pNew= (NODE *)malloc(sizeof(NODE)); // 새로운 노드를 위한 메모리 할당
	if(pNew== NULL) return 0; // 메모리 오버플로우인 경우 실패 반환
		
	pNew->llink=NULL;
	pNew->rlink=NULL;
	pNew->dataPtr=dataInPtr; // 새로운 노드에 데이터 연결
			
	if(pList->count == 0){ //리스트가 비어 있는 경우
		pList->head=pNew;
		pList->rear=pNew;
	}
	
	else if(pPre == NULL){ //리스트의 헤드에 삽입되는 경우
		pNew->rlink=pList->head;
		pList->head->llink=pNew;
		pList->head=pNew;
	}
		
    else if(pPre == pList->rear){ //리스트의 리어에 삽입되는 경우
		pPre->rlink=pNew;	
		pNew->llink=pPre;
		pList->rear=pNew;
	}
	
	else{ //중간에 삽입되는 경우
		pNew->rlink=pPre->rlink;
		pNew->llink=pPre->rlink->llink;
		pPre->rlink->llink=pNew;
		pPre->rlink=pNew;
	}
		
		// 리스트의 노드 개수 증가
		(pList->count)++;
		
		return 1; // 성공 반환
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr){
	*dataOutPtr=pLoc->dataPtr;
	
	 // 삭제할 노드의 이전 노드가 있는 경우
	if(pPre != NULL){
		pPre->rlink=pLoc->rlink;
	}
	else{
		pList->head=pLoc->rlink;
	}
	
	// 삭제할 노드의 다음 노드가 있는 경우
	if(pLoc->rlink != NULL){
		pLoc->rlink->llink=pPre;
	}
	else{
		pList->rear=pPre;
	}
	
	free(pLoc);
	(pList->count)--;
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu) {
    *pPre = NULL; // 이전 노드 초기화
    *pLoc = pList->head; // 현재 노드를 헤드로 초기화
    
    // 리스트를 끝까지 순회하면서 탐색
    while (*pLoc != NULL && compare_by_word(pArgu, (*pLoc)->dataPtr) > 0) {
        *pPre = *pLoc; // 이전 노드 갱신
        *pLoc = (*pLoc)->rlink; // 현재 노드를 다음 노드로 이동
    }
    
	//찾지 못한 경우
	if(*pLoc == NULL) return 0; 
	
	else{
		if(compare_by_word(pArgu, (*pLoc)->dataPtr)==0) return 1;
		else return 0;
	}
  
}

////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return	word structure pointer
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word){
	tWord *pWord=(tWord *)malloc(sizeof(tWord));
	
	if(pWord != NULL){
		pWord->word=strdup(word);
		pWord->freq=1;
	
	}
	
	return pWord;
}

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode){
	if(pNode != NULL){
		free(pNode->word); //단어 메모리 해제
		free(pNode); //단어 구조체 메모리 해제
	}
}

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_word(const tWord *dataPtr)
{
	printf( "%s\t%d\n", dataPtr->word, dataPtr->freq);
}

// gets user's input
void input_word(char *word)
{
	fprintf( stderr, "Input a word to find: ");
	fscanf( stdin, "%s", word);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	
	char word[100];
	tWord *pWord;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		pWord = createWord(word);
		
		//이미 저장된 단어는 빈도 증가
		ret = addNode( list, pWord);
		
		if (ret == 0 || ret == 2) // failure or duplicated
		{
			destroyWord( pWord);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		tWord *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				traverseList( list, print_word);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_word);
				break;
			
			case SEARCH:
				input_word(word);
				
				pWord = createWord( word);

				if (searchNode( list, pWord, &ptr)) print_word( ptr);
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
				
			case DELETE:
				input_word(word);
				
				pWord = createWord( word);

				if (removeNode( list, pWord, &ptr))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ptr->word, ptr->freq);
					destroyWord( ptr);
				}
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}

#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert(LIST *pList, NODE *pPre, void *dataInPtr) {
	
    NODE *pNew = (NODE *)malloc(sizeof(NODE));
    if (pNew == NULL) return 0;

    pNew->llink = NULL;
    pNew->rlink = NULL;
    pNew->dataPtr = dataInPtr;

    if (pList->count == 0) { //리스트가 비어있는 경우
        pList->head = pNew;
        pList->rear = pNew;
    } else if (pPre == NULL) { //맨 앞에 삽입되는 경우
        pNew->rlink = pList->head;
        pList->head->llink = pNew;
        pList->head = pNew;
    } else if (pPre == pList->rear) { //맨 뒤에 삽입되는 경우
        pNew->llink = pList->rear;
        pList->rear->rlink = pNew;
        pList->rear = pNew;
    } else { //중간에 삽입되는 경우
        pNew->rlink = pPre->rlink;
        pNew->llink = pPre;
        pPre->rlink->llink = pNew;
        pPre->rlink = pNew;
    }

    (pList->count)++;
    return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete(LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr) {

    *dataOutPtr = pLoc->dataPtr;

	//선행자 유무
    if (pPre != NULL) { 
        pPre->rlink = pLoc->rlink;
    } else { 
        pList->head = pLoc->rlink;
    }

	//후행자 유무
    if (pLoc->rlink != NULL) {
        pLoc->rlink->llink = pPre;
    } else {
        pList->rear = pPre;
    }

    free(pLoc);
    (pList->count)--;
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {
	
    *pPre = NULL;
    *pLoc = pList->head;

    while (*pLoc != NULL && pList->compare(pArgu, (*pLoc)->dataPtr) > 0) { 
        *pPre = *pLoc;
        *pLoc = (*pLoc)->rlink;
    }

    if (*pLoc == NULL)
        return 0;
    else {
        if (pList->compare(pArgu, (*pLoc)->dataPtr) == 0)
            return 1;
        else
            return 0;
    }
}

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)){
	
	LIST *mylist= (LIST *)malloc(sizeof(LIST));
	
	if (mylist == NULL) {
        return NULL; // 메모리 할당 실패 시 NULL 반환
    }
	
	else{
	mylist->count=0;
	mylist->head=NULL;
	mylist->rear=NULL;
	mylist->compare=compare;
	}
	
	return mylist;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node)
void destroyList( LIST *pList, void (*callback)(void *)){
		NODE *current= pList-> head;
		NODE *next;
		
		while(current!=NULL){
			next=current->rlink;
			callback(current->dataPtr); //callback함수로 처리
			free(current);
			current=next;
		}
	pList->head=NULL; //빈 리스트임을 명시
	free(pList); //리스트구조체 메모리 해제
}

// Inserts data into list
// callback은 이미 리스트에 존재하는 데이터를 발견했을 때 호출하는 함수
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *)){ 
	
	NODE *pPre, *pLoc;
	
	if(_search(pList, &pPre, &pLoc, dataInPtr)){ //리스트에 이미 존재하는 노드
		callback(pLoc->dataPtr); //addNode에서 빈도 증가시킬 때 쓰임
		return 2;
	}
	
	if(!_insert(pList, pPre, dataInPtr)) return 0; //메모리 오버플로우
	
	return 1; //성공
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
	
	NODE *pPre;
	NODE *pLoc=pList->head;
	
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
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr){
	NODE *pPre, *pLoc;
	
	if(!_search(pList, &pPre, &pLoc, pArgu)) return 0;
	
	*dataOutPtr=pLoc->dataPtr;
	
	return 1;
}




// returns number of nodes in list
int countList( LIST *pList){
	return pList->count;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
	return pList->count==0 ? 1 : 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)){
	NODE *pLoc=pList->head;
	
	while(pLoc!=NULL){
		callback(pLoc->dataPtr);
		pLoc=pLoc->rlink;
	}
}


// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)){
	NODE *pLoc=pList->rear;
	
	while(pLoc!=NULL){
		callback(pLoc->dataPtr);
		pLoc=pLoc->llink;
	}
}


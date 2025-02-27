#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
   for heap_Insert function
*/
static void _reheapUp( HEAP *heap, int index){
	if(index==0) return;
	
	int parentindex=(index-1)/2;
	if(heap->compare(heap->heapArr[index], heap->heapArr[parentindex])>0){
		void *temp = heap->heapArr[index];
		heap->heapArr[index]=heap->heapArr[parentindex];
		heap->heapArr[parentindex]=temp;
		_reheapUp(heap, parentindex);
	}
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
   for heap_Delete function
*/
static void _reheapDown( HEAP *heap, int index){
	int leftchild=index*2+1;
	int rightchild=index*2+2;
	int largest=index;
	
	 if (leftchild <= heap->last) {
        if (rightchild <= heap->last) {
            // 오른쪽 자식이 힙의 범위 안에 있는 경우
            if (heap->compare(heap->heapArr[leftchild], heap->heapArr[rightchild]) > 0) {
                // 왼쪽 자식이 더 큰 경우
                if (heap->compare(heap->heapArr[leftchild], heap->heapArr[largest]) > 0) {
                    largest = leftchild;
                }
            } else {
                // 오른쪽 자식이 더 큰 경우
                if (heap->compare(heap->heapArr[rightchild], heap->heapArr[largest]) > 0) {
                    largest = rightchild;
                }
            }
        }  else {
            // 오른쪽 자식이 없는 경우(heap이라 왼쪽 자식이 없는 경우는 발생 ㄴㄴ)
            if (heap->compare(heap->heapArr[leftchild], heap->heapArr[largest]) > 0) {
                largest = leftchild;
            }
        }
    }
	
	if(largest != index){ //부모 노드가 더 작은 경우, 교환이 일어난 경우
		void *temp=heap->heapArr[index];
		heap->heapArr[index]=heap->heapArr[largest];
		heap->heapArr[largest]=temp;
		_reheapDown(heap, largest);
	}
}
	

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10
*/
HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2)){
	HEAP *heap=(HEAP *)malloc(sizeof(HEAP));
	if(heap==NULL) return NULL;
	
	heap->heapArr=(void **)malloc(10*sizeof(void *));
	if(heap-> heapArr == NULL){
			free(heap);
			return NULL;
	}
	
	heap->last=-1;
	heap->capacity=10;
	heap->compare=compare;
	
	return heap;
}

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr)){
	if(heap ==NULL) return;
	
	for(int i=0; i<=heap->last; i++){
		remove_data(heap->heapArr[i]);
	}
	free(heap->heapArr);
	free(heap);
}
	

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr){
	if(heap->last+1== heap->capacity){ //heap의 용량이 꽉 찬 경우, heap->last: 마지막 인덱스 나타냄
		void **temp=(void **)realloc(heap->heapArr, 2*heap->capacity*sizeof(void *));
		if(temp ==NULL) return 0;
		
		heap->heapArr=temp;
		heap->capacity*=2;
	}
	
	(heap->last)++;
	heap->heapArr[heap->last]=dataPtr;
	_reheapUp(heap, heap->last);
	return 1;
}
	
	

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr){
	if(heap->last ==-1) return 0;
	
	*dataOutPtr=heap->heapArr[0];
	heap->heapArr[0]=heap->heapArr[heap->last];
	(heap->last)--;
	_reheapDown(heap,0);
	return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap){
	return heap->last==-1;
}

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (const void *data)){
	for(int i=0; i<=heap->last; i++){
		print_func(heap->heapArr[i]);
	}
	printf("\n");
}


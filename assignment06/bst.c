#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert ret 1일 경우 정상 나머지 비정상
static int _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *)){
	if(!root || !newPtr || !compare) return 0;
	int cmp=compare(newPtr->dataPtr, root->dataPtr);
	
	if(cmp<0){ //왼쪽에 삽입하는 경우
		if(root->left){
			return _insert(root->left, newPtr, compare, callback);
		} else{
			root->left=newPtr;
			return 1;
		}
	} else if(cmp>0){ //오른쪽에 삽입하는 경우
		if(root->right){
			return _insert(root->right, newPtr, compare, callback);
		}
		else{
			root->right=newPtr;
			return 1;
		}
	}else{
		callback(root->dataPtr); // 중복된 경우 해당 노드의 데이터를 증가시킴
		return 2;
	}
}
	
// used in BST_Insert
static NODE *_makeNode( void *dataInPtr){
	NODE *node=(NODE *)malloc(sizeof(NODE));
	if(node){
		node->left=NULL;
		node->right=NULL;
		node->dataPtr=dataInPtr;
	}
	return node;
}

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
	if(root){
		_destroy(root->left, callback);
		_destroy(root->right, callback);
		if(callback){
			callback(root->dataPtr);
		}
		free(root);
	}
}
		

// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)){
	if(!root) return NULL;
	int cmp=compare(keyPtr, root->dataPtr);
	if(cmp<0){ //왼쪽 서브트리에 대해 재귀호출
		root->left=_delete(root->left, keyPtr, dataOutPtr, compare);
	}
	else if(cmp>0){ //오른쪽 서브트리에대해 재귀호출
		root->right=_delete(root->right, keyPtr, dataOutPtr, compare);
	}
	else{ 
		NODE *delNode= root;
		if(!root->left){ //왼쪽 자식이 없는 경우
			root=root->right;
		}
		else if(!root->right){ //오른쪽 자식이 없는 경우
			root=root->left;
		}
		else{ //양쪽 자식이 모두 있는 경우
			NODE *parent = root;
            NODE *successor = root->right;
			
			//오른쪽 서브 트리에서 가장 왼쪽 노드를 찾음
			if(!successor->left){ //오른쪽 서브 트리의 왼쪽 노드가 없는 경우
				successor->left=root->left;
				root=successor;
			}
			else{ //오른쪽 서브 트리의 왼쪽 노드가 있는 경우
				while(successor->left){
					parent=successor;
					successor=parent->left;
				}
				
				parent->left=successor->right;
				successor->left=root->left;
				successor->right=root->right;
				root=successor;
			}
		}
		*dataOutPtr=delNode->dataPtr;
		free(delNode);
	}
	return root;
}
			
			
			
			
			
// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)){
	if(!root) return NULL;
	int cmp=compare(keyPtr, root->dataPtr);
	if(cmp<0){
		return _search(root->left, keyPtr, compare);
	}
	else if(cmp>0){
		return _search(root->right, keyPtr, compare);
	}
	else{
		return root;
	}
}

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)){
	if(root){
		_traverse(root->left, callback);
		callback(root->dataPtr);
		_traverse(root->right, callback);
	}
}

// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)){
	if(root){
		_traverseR(root->right, callback);
		callback(root->dataPtr);
		_traverseR(root->left, callback);
	}
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)){
	if(root){
		_inorder_print(root->right, level+1, callback);
		for(int i=0; i<level; i++){ //레벨만큼 탭 출력
			printf("\t");
		}
		callback(root->dataPtr);
		_inorder_print(root->left, level+1, callback);
	}
}
	
/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( int (*compare)(const void *, const void *)){
	TREE *tree=(TREE *)malloc(sizeof(TREE));
	
	if(tree!=NULL){
	tree->count=0;
	tree->root=NULL;
	tree->compare=compare;
	}
	return tree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree, void (*callback)(void *)){
	if(pTree){
		_destroy(pTree->root, callback);
		free(pTree);
	}
}

/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	0 overflow
			1 success
			2 if duplicated key
*/
int BST_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *)){
	NODE *newPtr=_makeNode(dataInPtr);
	
	if(!newPtr) return 0;
	if(!pTree->root){
		pTree->root=newPtr;
	}
	else{
		int ret=_insert(pTree->root, newPtr, pTree->compare, callback);
		if(ret ==2){ //중복된 경우
			free(newPtr);
			return ret;
		}
	}
	(pTree->count)++;
	return 1;
}
	

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Delete( TREE *pTree, void *keyPtr){
	void *dataOut=NULL;
	if(pTree->root){
		pTree->root=_delete(pTree->root, keyPtr, &dataOut, pTree->compare);
		if(dataOut){
			pTree->count--;
		}
	}
	return dataOut;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Search( TREE *pTree, void *keyPtr){
	if(!pTree->root) return NULL;
	NODE *node=_search(pTree->root, keyPtr, pTree->compare);
	if(node) return node->dataPtr;
	return NULL;
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree, void (*callback)(const void *)){
	if(pTree->root){
		_traverse(pTree->root, callback); //print_word
	}
}

/* prints tree using right-to-left inorder traversal
*/
void BST_TraverseR( TREE *pTree, void (*callback)(const void *)){
	if(pTree->root){
		_traverseR(pTree->root, callback);
	}
}

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)){
	if(pTree->root){
		_inorder_print(pTree->root, 0, callback); //print_word_only(줄 바꿈 문자 포함)
	}
}

/* returns number of nodes in tree
*/
int BST_Count( TREE *pTree){
	return pTree->count;
}



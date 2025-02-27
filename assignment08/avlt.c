#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y)	(((x) > (y)) ? (x) : (y))

// Function prototypes
static NODE *rotateRight(NODE *root);
static NODE *rotateLeft(NODE *root);
static NODE *_insert(NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *), int *duplicated);
static NODE *_makeNode(void *dataInPtr);
static void _destroy(NODE *root, void (*callback)(void *));
static NODE *_delete(NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *));
static NODE *_search(NODE *root, void *keyPtr, int (*compare)(const void *, const void *));
static void _traverse(NODE *root, void (*callback)(const void *));
static void _traverseR(NODE *root, void (*callback)(const void *));
static void _inorder_print(NODE *root, int level, void (*callback)(const void *));
static int getHeight(NODE *root);

// internal functions (not mandatory)
// used in AVLT_Insert
// return 	pointer to root
static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *), int *duplicated){
	if(!root) return newPtr;
	
	int cmp=compare(newPtr->dataPtr, root->dataPtr);
	if(cmp>0){
		root->right=_insert(root->right, newPtr, compare, callback, duplicated);
	}
	else if(cmp<0){
		root->left=_insert(root->left, newPtr, compare, callback, duplicated);
	}
	else{
		*duplicated=1;
		if(callback) callback(root->dataPtr);
		return root;
	}
	
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	
	int height=getHeight(root->left)-getHeight(root->right);
	
	if(height>1){
		if(getHeight(root->left->left)>=getHeight(root->left->right)){ //LL
			return rotateRight(root);
		}
		else{ //LR
			root->left=rotateLeft(root->left);
			return rotateRight(root);
		}
	}
	
	else if(height<-1){
		if(getHeight(root->right->right)>=getHeight(root->right->left)){ //RR
			return rotateLeft(root);
		}
		else{ //RL
			root->right=rotateRight(root->right);
			return rotateLeft(root);
		}
	}
	
	if(!root) return root; //루트 노드가 NULL인 경우
	
	root->height=max(getHeight(root->left), getHeight(root->right))+1;
	
	return root;
}

// used in AVLT_Insert
static NODE *_makeNode( void *dataInPtr){
	NODE *node=(NODE *)malloc(sizeof(NODE));
	if(node){
		node->dataPtr=dataInPtr;
		node->left=node->right=NULL;
		node->height=1;
	}
	return node;
}

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
	if(!root) return; 
	//후위 순회 방식으로 삭제
	_destroy(root->left, callback); 
	_destroy(root->right, callback);
	if(callback) callback(root->dataPtr);
	free(root);
}
		

// used in AVLT_Delete
static NODE *_delete(NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)) {
    if (!root) return NULL;
    
    int cmp = compare(keyPtr, root->dataPtr);
    if (cmp < 0) {
        root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
    } else if (cmp > 0) {
        root->right = _delete(root->right, keyPtr, dataOutPtr, compare);
    } else {
        // 노드를 찾았을 때
		*dataOutPtr=root->dataPtr;
        if (root->left == NULL || root->right == NULL) { //자식이 1개 이하인 경우
            NODE *temp = root->left ? root->left : root->right; 
            if (temp == NULL) { //자식이 없음
                temp = root;
                root = NULL;
            } else { //자식이 1개
                *root = *temp;
            }
            free(temp);
        } else { //자식이 2개인 경우 
            NODE *temp = root->right;
            while (temp->left != NULL)
                temp = temp->left;

            root->dataPtr = temp->dataPtr;
            root->right = _delete(root->right, temp->dataPtr, &temp->dataPtr, compare);
        }
    }

    if (root == NULL) //삭제 후 트리가 비어있는 경우
        return root;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;

    int balance = getHeight(root->left) - getHeight(root->right);

    if (balance > 1) {
        if (getHeight(root->left->left) >= getHeight(root->left->right)) { //LL
            return rotateRight(root);
        } else { //LR
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
    } else if (balance < -1) {
        if (getHeight(root->right->right) >= getHeight(root->right->left)) { //RR
            return rotateLeft(root);
        } else { //RL
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
    }

    return root;
}

// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)){
	if(!root) return NULL;
	
	int cmp=compare(keyPtr, root->dataPtr);
	if(cmp>0){
		return _search(root->right, keyPtr,compare);
	}else if(cmp<0){
		return _search(root->left, keyPtr,compare);
	}else{
		return root;
	}
}

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)){
	if(root){
		_traverse(root->left,callback);
		callback(root->dataPtr);
		_traverse(root->right,callback);
	}
}

// used in AVLT_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)){
	if(root){
		_traverseR(root->right,callback);
		callback(root->dataPtr);
		_traverseR(root->left,callback);
	}
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)){
	if(root){
		_inorder_print(root->right, level+1, callback);
		for(int i=0; i<level; i++){
			printf("\t");
		}			
		callback(root->dataPtr);
		_inorder_print(root->left, level+1, callback);
	}
}



// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root){
	return root? root->height:0;
}
	
// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight( NODE *root){
	NODE *newroot=root->left;
	root->left=newroot->right;
	newroot->right=root;
	
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	newroot->height=max(getHeight(newroot->left),getHeight(newroot->right))+1;
	
	return newroot;
}

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root){
	NODE *newroot=root->right;
	
	root->right=newroot->left;
	newroot->left=root;
	
	root->height=max(getHeight(root->left),getHeight(root->right))+1;
	newroot->height=max(getHeight(newroot->left),getHeight(newroot->right))+1;
	
	return newroot;
}


/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *AVLT_Create( int (*compare)(const void *, const void *)){
	TREE *tree=(TREE *)malloc(sizeof(TREE));
	if(tree){
		tree->count=0;
		tree->root=NULL;
		tree->compare=compare;
	}
	return tree;
}

/* Deletes all data in tree and recycles memory
*/
void AVLT_Destroy( TREE *pTree, void (*callback)(void *)){
	if(pTree){
		_destroy(pTree->root, callback);
		free(pTree);
	}
}
		
/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	1 success
			0 overflow
			2 if duplicated key
*/
int AVLT_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *)){
	if(!pTree) return 0;
	int duplicated=0;
	NODE *newNode=_makeNode(dataInPtr);
	if(!newNode) return 0;
	
	pTree->root=_insert(pTree->root, newNode, pTree->compare, callback, &duplicated);
	if(duplicated){
		free(newNode);
		return 2;
	}
	(pTree->count)++;
	return 1;
}
	
/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Delete( TREE *pTree, void *keyPtr){
	if(!pTree)  return NULL;
	void *dataOutPtr=NULL;
	pTree->root=_delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);
	if(dataOutPtr) (pTree->count)--;
	return dataOutPtr;
}
	
/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Search( TREE *pTree, void *keyPtr){
	if(pTree){
		NODE *node= _search(pTree->root, keyPtr, pTree->compare);
		if(node) return node->dataPtr;
	}
	return NULL;
}

/* prints tree using inorder traversal
*/
void AVLT_Traverse( TREE *pTree, void (*callback)(const void *)){
	if(pTree) _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
*/
void AVLT_TraverseR( TREE *pTree, void (*callback)(const void *)){
	if(pTree) _traverseR(pTree->root, callback);
}
	
/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)){
	if(pTree) _inorder_print(pTree->root, 0, callback);
}
	

/* returns number of nodes in tree
*/
int AVLT_Count( TREE *pTree){
	return pTree? pTree->count:0;
}

/* returns height of the tree
*/
int AVLT_Height( TREE *pTree){
	return pTree && pTree->root ? getHeight(pTree->root):0;
}

//문제점: 노드 하나있을 때 안되네 레벨 +1 하나 있는데 count도 0 나옴 이것만 수정

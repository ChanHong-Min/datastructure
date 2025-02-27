#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE 27 // 'a' ~ 'z' and EOW //각 노드에서 분기할 수 있는 방향이 27
#define EOW '$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x) (((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a')) //트라이 노드의 자식 배열에서 특정 문자가 저장될 위치를 결정

// TRIE type 
typedef struct trieNode {
    int index; // -1 (non-word), 0, 1, 2, ... 사전에 해당하는 인덱스 영어 사전 배열의 인덱스처럼 보면됨
    struct trieNode *subtrees[MAX_DEGREE]; //어느 방향으로 내려갈 것인가. 자식 노드 포인터
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
    return    node pointer
            NULL if overflow
*/
TRIE *trieCreateNode(void) {
    TRIE *newNode = (TRIE *)malloc(sizeof(TRIE));
    if (!newNode) return NULL;

    newNode->index = -1;
    for (int i = 0; i < MAX_DEGREE; i++) {
        newNode->subtrees[i] = NULL;
    }
    return newNode;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root) {
    if (!root) return;

    for (int i = 0; i < MAX_DEGREE; i++) {
        if (root->subtrees[i] != NULL) {
            trieDestroy(root->subtrees[i]);
        }
    }
    free(root);
}

/* Inserts new entry into the trie
    return    1 success
            0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str, int dic_index) {
    if (!root || !str) return 0;

    TRIE *current = root;
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalpha(str[i]) && str[i] != EOW) return 0; //영문자와 EOW 외 문자는 포함하지 않음
        char ch = tolower(str[i]);
        int index = getIndex(ch);
        if (current->subtrees[index] == NULL) {
            current->subtrees[index] = trieCreateNode();
            if (current->subtrees[index] == NULL) return 0;
        }
        current = current->subtrees[index];
    }
    
    if (current->index != -1) return 0; // 이미 있는 단어
    current->index = dic_index;
    return 1;
}


/* Retrieve trie for the requested key
    return    index in dictionary (trie) if key found
            -1 key not found
*/
int trieSearch(TRIE *root, char *str) {
    if (root == NULL || str == NULL) return -1;

    TRIE *current = root;
    for (int i = 0; str[i] != '\0'; i++) {
        char ch = tolower(str[i]);
        int index = getIndex(ch);
        if (current->subtrees[index] == NULL) { // 해당 문자가 TRIE에 없으면
            return -1;
        }
        current = current->subtrees[index]; // 다음 노드로 이동
    }

    // 마지막 문자가 EOW인지 확인
    int index = getIndex(EOW);
    if (current->subtrees[index] != NULL) {
        return current->subtrees[index]->index;
    }
    
    return -1;
}

static int trieList_main(TRIE *root, char *dic[], int count) { 
    //trieList에서 사용됨 출력할 때 마다 count 증가
    //dic[]: 단어를 저장한 배열. TRIE에 저장된 각 단어의 인덱스에 해당하는 문자열이 저장되어 있음.
    if (root == NULL) return count;

    if (root->index != -1) { //현재 노드가 단어의 끝을 나타내는 경우
       printf("[%d] %s\n", count + 1, dic[root->index]);
        count++;
    }

    for (int i = 0; i < MAX_DEGREE; i++) {
        count = trieList_main(root->subtrees[i], dic, count);
    }
    return count;
}

void trieList(TRIE *root, char *dic[]) {
    if (root == NULL) return;

    trieList_main(root, dic, 0);
}


/* prints all entries starting with str (as prefix) in trie
    ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
    this function uses trieList function
*/
void triePrefixList(TRIE *root, char *prefix, char *dic[]) {
	if(!root || !prefix) return;

    TRIE *current = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = getIndex(prefix[i]);
        if (current->subtrees[index] == NULL) {
            return;
        }
        current = current->subtrees[index];
    }

    // List all words in the subtree of the current node
    trieList(current, dic);
}



/* makes permuterms for given str
    ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
    return    number of permuterms
*/
int make_permuterms(char *str, char *permuterms[]) {
    int len = strlen(str);
    char temp[len + 2]; //EOW, NULL 문자 저장 위함
    strcpy(temp, str);
    temp[len] = EOW;
    temp[len + 1] = '\0';

    for (int i = 0; i <= len; i++) {
        permuterms[i] = strdup(temp);
        char first = temp[0];
        memmove(temp, temp + 1, len + 1); //복사할 dest, 복사할 원본, 복사할 바이트 수(널 문자까지 복사)
        temp[len] = first; //첫 번째 단어 마지막으로 이동
    }
    return len + 1; //number of permuterms
}

/* recycles memory for permuterms
*/
void clear_permuterms(char *permuterms[], int size) {
    for (int i = 0; i < size; i++) {
        free(permuterms[i]);
    }
}

/* wildcard search
    ex) "ab*", "*ab", "a*b", "*ab*"
    this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[])
{
	int len = strlen(str);
	char rotatedStr[len + 2]; //EOW, NULL 문자 저장 위함
    strcpy(rotatedStr, str);
    rotatedStr[len] = EOW;
    rotatedStr[len + 1] = '\0';

   // Case 1~3: '*' 끝, 시작, 중간
   if (str[len - 1] == '*' || str[0] == '*' || strchr(str, '*')) {
      // Handle case 1~3
      if (str[len - 1] == '*') {
         // Case 1: '*' 끝
         rotatedStr[len] = '\0'; // Remove EOW
         for (int i = len; i > 0; i--) {
            rotatedStr[i-1] = rotatedStr[i - 2]; 
         }
         rotatedStr[0] = EOW;
      } else if (str[0] == '*') {
         // Case 2: '*' 시작
         char temp[len];
         strcpy(temp, &str[1]); //str의 첫번째 문자열부터 복사
         strcat(temp, "$"); 
         strcpy(rotatedStr, temp);
      } else {
         // Case 3: '*' 중간
         char *starPos = strchr(str, '*'); //*의 위치 반환
         if (starPos && starPos != str && starPos != &str[len - 1]) { //*가 문자열에 존재, 첫번째, 마지막에 위치하지 않음
            char prefix[starPos - str + 1];
            strncpy(prefix, str, starPos - str); //*이전 문자열 복사.
            prefix[starPos - str] = '\0';
			
            char suffix[len - (starPos - str)];
            strcpy(suffix, starPos + 1);
            strcat(suffix, "$");	
			
            char temp[len + 2];
            strcpy(temp, suffix);
            strcat(temp, prefix);
            strcpy(rotatedStr, temp);
         }
      }
   }

   // Case 4: '*' 양쪽에 있는 경우
   if (str[0] == '*' && str[len - 1] == '*') {
      // Create a copy of the string without '*' characters
      char copy[len]; 
      int copyIndex = 0;
      for (int i = 1; i < len - 1; i++) {
         if (str[i] != '*') {
            copy[copyIndex++] = str[i];
         }
      }
      copy[copyIndex] = '\0'; 
    
       triePrefixList(root, copy, dic); 
   } else {
          triePrefixList(root, rotatedStr, dic);
   }
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
    TRIE *permute_trie; //루트 노드 가리킬 포인터
    char *dic[100000]; //배열 동적으로 할당하면 더 좋을것임

    int ret;
    char str[100]; //정교하면 하면 더 좋음
    FILE *fp;
    char *permuterms[100];
    int num_p; // # of permuterms
    int num_words = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "rt");
    if (fp == NULL) {
        fprintf(stderr, "File open error: %s\n", argv[1]);
        return 1;
    }

    permute_trie = trieCreateNode(); // trie for permuterm index

    while (fscanf(fp, "%s", str) != EOF) {
        num_p = make_permuterms(str, permuterms);

        for (int i = 0; i < num_p; i++)
            trieInsert(permute_trie, permuterms[i], num_words);

        clear_permuterms(permuterms, num_p);

        dic[num_words++] = strdup(str);
    }

    fclose(fp);

    printf("\nQuery: ");
    while (fscanf(stdin, "%s", str) != EOF) {
        // wildcard search term
        if (strchr(str, '*')) {
            trieSearchWildcard(permute_trie, str, dic);
        }
        // keyword search
        else {
            ret = trieSearch(permute_trie, str);

            if (ret == -1) printf("[%s] not found!\n", str);
            else printf("[%s] found!\n", dic[ret]);
        }
        printf("\nQuery: ");
    }

    for (int i = 0; i < num_words; i++)
        free(dic[i]);

    trieDestroy(permute_trie);

    return 0;
}

////////////////////////////////////////////////////////////////////////////////

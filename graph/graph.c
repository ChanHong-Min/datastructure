#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 깊이 우선 순회
void depth_first_Traversal(int *graph, int num_vertex)
{
    int *stack = (int *)malloc((num_vertex + 1) * sizeof(int));
    int *visited = (int *)calloc(num_vertex + 1, sizeof(int));
    int top = -1;
    int i, current;

    // Push the first vertex (1) onto the stack
    stack[++top] = 1;
    visited[1] = 1;

    while (top != -1)
    {
        current = stack[top--];
        printf("%d ", current);

        // Visit nodes in the correct order (1 to num_vertex)
        for (i = 1; i <= num_vertex; i++)
        {
            if (*(graph + current * num_vertex  + i) && !visited[i])
            {
                stack[++top] = i;
                visited[i] = 1;
            }
        }
    }

    free(stack);
    free(visited);
    printf("\n");
}


// 너비 우선 순회
void breadth_first_Traversal(int *graph, int num_vertex)
{
    int *queue = (int *)malloc((num_vertex + 1) * sizeof(int));
    int *visited = (int *)calloc(num_vertex + 1, sizeof(int)); //각 요소 0으로 초기화
    int front = 0, rear = 0;
    int i, current;

    // Enqueue the first vertex (1)
    queue[rear++] = 1;
    visited[1] = 1;

    while (front != rear)
    {
        current = queue[front++];
        printf("%d ", current);

        // Visit nodes in the correct order (1 to num_vertex)
        for (i = 1; i <= num_vertex; i++)
        {
            if (*(graph + current * num_vertex + i) && !visited[i])
            {
                queue[rear++] = i;
                visited[i] = 1;
            }
        }
    }

    free(queue);
    free(visited);
    printf("\n");
}


////////////////////////////////////////////////////////////////////////////////
int *load_graph( char *filename, int *num_vertex)
{
	char str[100];
	int num;
	int from, to;
	
	FILE *fp = fopen( filename, "r");
	if (fp == NULL)
	{
		printf( "Error: cannot open file [%s]\n", filename);
		return NULL;
	}
	
	fscanf( fp, "%s%d", str, &num);
	assert( num > 0);
	assert( strcmp( str, "*Vertices") == 0);
	
	int *graph = (int *)calloc( (num+1) * (num+1), sizeof(int));
	
	if (graph == NULL) return NULL;
	
	fscanf( fp, "%s", str);
	assert( strcmp( str, "*Edges") == 0);
	
	int ret;
	
	while( 2 == fscanf( fp, "%d%d", &from, &to))
	{
		*(graph + from * num + to) = 1; // graph[from][to]
		*(graph + to * num + from) = 1;
	}
	
	fclose( fp);

	*num_vertex = num;
	
	return graph;
}

////////////////////////////////////////////////////////////////////////////////
void print_graph( int *graph, int num)
{
	int i, j;
	
	for (i = 1; i <= num; i++)
	{
		for (j = 1; j <= num; j++)
			printf( "%d\t", *(graph + i * num + j));
		
		printf( "\n");
	}
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int *graph;
	int num_vertex = 0;

	if (argc != 2)
	{
		printf( "Usage: %s FILE(.net)\n", argv[0]);
		return 2;
	}

	// .net 파일 읽어서 adjacent matrix로 저장
	// (num_vertex+1) * (num_vertex+1)의 2차원 배열
	graph = load_graph( argv[1], &num_vertex);
	if (graph == NULL) return 0;
	
	// 그래프 출력
	print_graph( graph, num_vertex);

	// 각각 스택과 큐를 사용
	// 배열을 사용하나, 스택이나 큐에 포함될 원소의 수는 정점의 수와 동일하므로 overflow 상태가 될 위험이 없음
	printf( "DFS : ");
	depth_first_Traversal( graph, num_vertex);

	printf( "BFS : ");
	breadth_first_Traversal( graph, num_vertex);
	
	free( graph);
	
	return 0;
}


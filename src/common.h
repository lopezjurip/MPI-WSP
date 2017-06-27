#include <stdlib.h>

typedef struct graph {
  size_t N;
  size_t **adjacency; // No linked-list pls
} Graph;

Graph *read_graph(char *path);

void swap(size_t *a, size_t *b);

void paths(size_t **acc, size_t *position, size_t *array, size_t i, size_t length);

size_t factorial(size_t n);

size_t cost_for(Graph *graph, size_t *path);

void print_graph(Graph *graph);

size_t *range(size_t start, size_t end); // range [stat, end)

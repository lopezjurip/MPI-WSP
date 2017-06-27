#include <math.h>
#include <stdio.h>
#include <string.h>

#include "./common.h"

Graph *read_graph(char *path) {
  FILE *file = fopen(path, "r");
  Graph *graph = malloc(sizeof(Graph));

  char *line = NULL;
  size_t length = 0;
  ssize_t read;
  char *token;

  // Get Size
  read = getline(&line, &length, file);
  graph->N = atoi(line);
  graph->adjacency = calloc(graph->N, sizeof(size_t *));
  for (size_t k = 0; k < graph->N; k++) {
    graph->adjacency[k] = calloc(graph->N, sizeof(size_t));
    for (size_t p = 0; p < graph->N; p++) {
      graph->adjacency[k][p] = SIZE_MAX;
    }
  }

  // Build graph
  for (size_t k = 0; k < graph->N - 1; k++) {
    read = getline(&line, &length, file);
    size_t i = k + 1;
    while ((token = strsep(&line, " ")) != NULL) {
      size_t cost = atoi(token);
      graph->adjacency[k][i] = graph->adjacency[i][k] = cost;
      i += 1;
    }
  }
  return graph;
}

void swap(size_t *a, size_t *b) {
  int aux = *a;
  *a = *b;
  *b = aux;
}

void paths(size_t **acc, size_t *position, size_t *array, size_t i, size_t length) {
  if (length == i) {
    size_t val = *position;
    acc[val][0] = 0;
    for (size_t j = 0; j < i; j++) {
      acc[val][j + 1] = array[j];
    }
    *position = val + 1;
  }

  for (size_t j = i; j < length; j++) {
     swap(array + i, array + j);
     paths(acc, position, array, i + 1, length);
     swap(array + i, array + j);
  }
  return;
}

size_t factorial(size_t n) {
  size_t result = 1;
  for (size_t c = 1; c <= n; c++) {
    result = result * c;
  }
  return result;
}

size_t cost_for(Graph *graph, size_t *path) {
  size_t current_cost = 0;
  for (size_t k = 0; k < graph->N - 1; k++) {
    size_t from = path[k];
    size_t to = path[k + 1];
    size_t cost = graph->adjacency[from][to];
    // printf("%lu <- [%lu] -> %lu\n", from, cost, to);
    current_cost += cost;
  }
  return current_cost;
}

void print_graph(Graph *graph) {
  for (size_t k = 0; k < graph->N - 1; k++) {
    for (size_t i = 0; i < graph->N - 1; i++) {
      size_t cost = graph->adjacency[k][i];
      printf("%lu ", cost);
    }
    printf("%s\n", "");
  }
}

size_t *range(size_t start, size_t end) { // range [stat, end)
  size_t *arr = calloc(end - start, sizeof(size_t));
  for (size_t k = start; k < start + end - 1; k++) {
    arr[k - start] = k;
  }
  return arr;
}

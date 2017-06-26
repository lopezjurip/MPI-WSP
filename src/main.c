#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

typedef struct graph {
  size_t N;
  size_t **adjacency; // No linked-list pls
} Graph;

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
  }

  // Build graph
  for (size_t k = 0; k < graph->N - 1; k++) {
    graph->adjacency[k] = calloc(graph->N, sizeof(size_t));

    read = getline(&line, &length, file);
    size_t i = k + 1;
    while ((token = strsep(&line, " ")) != NULL) {
      size_t cost = atoi(token);
      // printf("%lu <-[%lu]-> %lu\n", k, cost, i);
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

void roads(size_t **acc, size_t *position, size_t *array, size_t i, size_t length) {
  if (length == i) {
    size_t val = *position;
    for (size_t j = 0; j < i; j++) {
      acc[val][j] = array[j];
    }
    *position = val + 1;
  }
  for (size_t j = i; j < length; j++) {
     swap(array + i, array + j);
     roads(acc, position, array, i + 1, length);
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

int main(int argc, char *argv[]) {
  int rank, size, rc;

  char *graph_in_path = argv[1];
  Graph *graph = read_graph(graph_in_path);

  // Roads from 0 to its adjacents
  // In our example: arr = [1,2,3,4]
  size_t *arr = calloc(graph->N - 1, sizeof(size_t));
  for (size_t k = 0; k < graph->N - 1; k++) {
    arr[k] = k + 1;
  }

  size_t combs = factorial(graph->N - 1);
  size_t **acc = calloc(combs, sizeof(size_t*));
  for (size_t i = 0; i < combs; i++) {
    acc[i] = calloc(graph->N - 1, sizeof(size_t));
  }

  size_t pos = 0;
  roads(acc, &pos, arr, 0, 4);

  for (size_t i = 0; i < combs; i++) {
    for (size_t j = 0; j < 4; j++) {
      printf("%lu ", acc[i][j]);
    }
    printf("\n");
  }

  // for (size_t k = 0; k < graph->N - 1; k++) {
  //   for (size_t i = 0; i < graph->N - 1; i++) {
  //     size_t cost = graph->adjacency[k][i];
  //   }
  // }

  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Finalize();

  return 0;
}

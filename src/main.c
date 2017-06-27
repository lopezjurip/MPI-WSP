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

int main(int argc, char *argv[]) {
  int rank, size, rc;
  char *graph_in_path;
  Graph *graph = NULL;
  size_t *distribution;
  size_t *arr;
  size_t combs = 0;
  size_t **options;
  size_t better = -1; // better options index
  size_t lowest_cost = SIZE_MAX;
  size_t elements_per_proc;

  rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  graph_in_path = argv[1];
  graph = read_graph(graph_in_path);

  // Adjacents of starting node (0)
  // In our example: arr = [1,2,3,4]
  arr = range(1, graph->N);

  // Create all possible paths
  // Each path size: graph->N
  combs = factorial(graph->N - 1);
  elements_per_proc = combs / size;
  options = calloc(combs, sizeof(size_t*));
  for (size_t i = 0; i < combs; i++) {
    options[i] = calloc(graph->N, sizeof(size_t));
  }
  size_t pos = 0;
  paths(options, &pos, arr, 0, 4); // Saved in 'options'

  if (rank == 0) {
    // distribution = calloc(elements_per_proc, sizeof(size_t));
    distribution = range(0, combs);
  }

  // Create a buffer that will hold a subset of the random numbers
  size_t *sub_paths = calloc(elements_per_proc, sizeof(size_t));

  // Scatter the random numbers to all processes
  MPI_Scatter(distribution, elements_per_proc, MPI_UNSIGNED_LONG, sub_paths,
              elements_per_proc, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);


  // Find best path in sub_paths
  for (size_t i = 0; i < elements_per_proc; i++) {
    size_t *path = options[sub_paths[i]];
    // printf("I %d Got %lu\n", rank, sub_paths[i]);
    // printf("---\nCombination %lu\n", i);
    size_t current_cost = cost_for(graph, path);
    // printf("Cost: %lu\n", current_cost);
    if (current_cost < lowest_cost) {
      better = sub_paths[i];
      lowest_cost = current_cost;
    }
  }

  size_t *results = NULL;
  if (rank == 0) {
    results = calloc(size, sizeof(size_t));
  }
  MPI_Gather(&better, 1, MPI_UNSIGNED_LONG, results, 1, MPI_UNSIGNED_LONG, 0,
             MPI_COMM_WORLD);

  if (rank == 0) {
    for (size_t i = 0; i < size; i++) {
      size_t *path = options[results[i]];
      size_t current_cost = cost_for(graph, path);
      if (current_cost < lowest_cost) {
        better = sub_paths[i];
        lowest_cost = current_cost;
      }
    }
    printf("Rank %d : Path: ", rank);
    for (size_t i = 0; i < graph->N; i++) {
      printf("%lu ", options[better][i]);
    }
    printf("\nCost: %lu\n", lowest_cost);
  }

  MPI_Finalize();

  return 0;
}

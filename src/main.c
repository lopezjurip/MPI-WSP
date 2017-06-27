#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./common.h"

int main(int argc, char *argv[]) {
  char *graph_in_path = argv[1];

  Graph *graph = NULL;
  size_t *adjacents = NULL;
  size_t **options = NULL;
  size_t pos = 0;
  size_t combs = 0;
  size_t better = -1; // better options index
  size_t lowest_cost = SIZE_MAX;

  // Read graph from file
  graph = read_graph(graph_in_path);

  // Adjacents of starting node (0)
  // In our example: adjacents = [1,2,3,4]
  adjacents = range(1, graph->N);

  // Create all possible paths in "options"
  // Each path size: graph->N
  combs = factorial(graph->N - 1); // All possible combinations
  options = calloc(combs, sizeof(size_t*));
  for (size_t i = 0; i < combs; i++) {
    options[i] = calloc(graph->N, sizeof(size_t));
  }
  paths(options, &pos, adjacents, 0, 4); // Saved in 'options'

  // Find best path
  better = -1; // better options index
  lowest_cost = SIZE_MAX;
  for (size_t i = 0; i < combs; i++) {
    size_t *path = options[i];
    size_t current_cost = cost_for(graph, path);
    if (current_cost < lowest_cost) {
      better = i;
      lowest_cost = current_cost;
    }
  }

  printf("Path:\n");
  for (size_t i = 0; i < graph->N; i++) {
    printf("%lu ", options[better][i]);
  }
  printf("\nCost: %lu\n", lowest_cost);

  return 0;
}

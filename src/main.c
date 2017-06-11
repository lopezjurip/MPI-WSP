#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct graph {
  size_t N;
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

  // Build graph
  for (size_t k = 0; k < graph->N - 1; k++) {
    read = getline(&line, &length, file);
    size_t i = k + 1;
    while ((token = strsep(&line, " ")) != NULL) {
      size_t cost = atoi(token);
      // TODO: how to save it?
      printf("%lu <-[%lu]-> %lu\n", k, cost, i);
      i += 1;
    }
  }
  return graph;
}

int main(int argc, char *argv[]) {
  char *graph_in_path = argv[1];
  read_graph(graph_in_path);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

#define MAX_DEVICES 100

typedef struct {
    int numDevices;
    bool adjMatrix[MAX_DEVICES][MAX_DEVICES];
} Graph;

void initializeGraph(Graph *graph, int numDevices) {
    if (numDevices > MAX_DEVICES) {
        printf("Numero de dispositivos excede o limite maximo de %d.\n", MAX_DEVICES);
        exit(EXIT_FAILURE);
    }
    graph->numDevices = numDevices;
    for (int i = 0; i < numDevices; i++) {
        for (int j = 0; j < numDevices; j++) {
            graph->adjMatrix[i][j] = false;
        }
    }
}

void addEdge(Graph *graph, int src, int dest) {
    graph->adjMatrix[src][dest] = true;
    graph->adjMatrix[dest][src] = true;
}

void DFSUtil(Graph *graph, int v, bool visited[], int component[], int *size) {
    visited[v] = true;
    component[(*size)++] = v;

    for (int i = 0; i < graph->numDevices; i++) {
        if (graph->adjMatrix[v][i] && !visited[i]) {
            DFSUtil(graph, i, visited, component, size);
        }
    }
}

void findConnectedComponents(Graph *graph, int componentList[MAX_DEVICES][MAX_DEVICES], int *componentSizes, int *numComponents) {
    bool visited[MAX_DEVICES] = {false};
    int component[MAX_DEVICES];
    int size;

    *numComponents = 0;

    for (int i = 0; i < graph->numDevices; i++) {
        if (!visited[i]) {
            size = 0;
            DFSUtil(graph, i, visited, component, &size);
            for (int j = 0; j < size; j++) {
                componentList[*numComponents][j] = component[j];
            }
            componentSizes[*numComponents] = size;
            (*numComponents)++;
        }
    }
}

void printConnectedComponentsPyramid(int componentList[MAX_DEVICES][MAX_DEVICES], int componentSizes[MAX_DEVICES], int numComponents) {
    for (int i = 0; i < numComponents; i++) {
        printf("Componente %d:\n", i);
        int index = 0;
        int level = 1;
        int maxLevels = (componentSizes[i] + 1) / 2;

        while (index < componentSizes[i] && level <= maxLevels) {
            for (int j = 0; j < maxLevels - level; j++) {
                printf(" ");
            }
            for (int j = 0; j < 2 * level - 1 && index < componentSizes[i]; j++) {
                printf("%d ", componentList[i][index++]);
            }
            printf("\n");
            level++;
        }
        printf("\n");
    }
}

void findShortestPath(Graph *graph, int start, int end) {
    int prev[MAX_DEVICES];
    bool visited[MAX_DEVICES] = {false};
    int queue[MAX_DEVICES], front = 0, rear = 0;

    for (int i = 0; i < graph->numDevices; i++) {
        prev[i] = -1;
    }

    visited[start] = true;
    queue[rear++] = start;

    while (front < rear) {
        int node = queue[front++];

        if (node == end) {
            break;
        }

        for (int i = 0; i < graph->numDevices; i++) {
            if (graph->adjMatrix[node][i] && !visited[i]) {
                visited[i] = true;
                prev[i] = node;
                queue[rear++] = i;
            }
        }
    }

    if (!visited[end]) {
        printf("Nao ha caminho entre %d e %d.\n", start, end);
        return;
    }

    printf("Menor caminho entre %d e %d: ", start, end);
    int path[MAX_DEVICES];
    int path_size = 0;
    for (int v = end; v != -1; v = prev[v]) {
        path[path_size++] = v;
    }

    for (int i = path_size - 1; i >= 0; i--) {
        printf("%d ", path[i]);
    }
    printf("\n");
}


void suggestMinConnections(Graph *graph, int componentList[MAX_DEVICES][MAX_DEVICES], int componentSizes[MAX_DEVICES], int numComponents) {
    if (numComponents <= 1) {
        printf("Todos os dispositivos ja estao conectados.\n");
        return;
    }

    printf("Conectar componentes desconectados requer %d conexao(oes).\n", numComponents - 1);

    for (int i = 0; i < numComponents - 1; i++) {
        int u = componentList[i][0];
        int v = componentList[i + 1][0];
        addEdge(graph, u, v);
        printf("Sugestao de conexao: dispositivo %d com dispositivo %d.\n", u, v);
    }
}

void generateRandomNetwork(Graph *graph) {
    srand(time(0));

    int numDevices = graph->numDevices;
    int numLinks = numDevices + rand() % (numDevices * 2);

    for (int i = 0; i < numLinks; i++) {
        int src = rand() % numDevices;
        int dest = rand() % numDevices;
        if (src != dest && !graph->adjMatrix[src][dest]) {
            addEdge(graph, src, dest);
        }
    }
}

void printAllShortestPaths(Graph *graph) {
    int numDevices = graph->numDevices;
    for (int start = 0; start < numDevices; start++) {
        for (int end = start + 1; end < numDevices; end++) {
            if (graph->adjMatrix[start][end]) {
                printf("Menor caminho entre %d e %d: ", start, end);
                findShortestPath(graph, start, end);
            }
        }
    }
}

int main() {
    int numDevices = 20 + rand() % (MAX_DEVICES - 20); // Número de dispositivos, no mínimo 20
    Graph graph;
    initializeGraph(&graph, numDevices);

    generateRandomNetwork(&graph);

    int componentList[MAX_DEVICES][MAX_DEVICES];
    int componentSizes[MAX_DEVICES];
    int numComponents;
    findConnectedComponents(&graph, componentList, componentSizes, &numComponents);

    printf("Numero de componentes conectados: %d\n", numComponents);

    printConnectedComponentsPyramid(componentList, componentSizes, numComponents);

    printAllShortestPaths(&graph);

    suggestMinConnections(&graph, componentList, componentSizes, numComponents);

    int start, end;
    printf("Digite o dispositivo de origem e destino: ");
    scanf("%d %d", &start, &end);

    if (start < 0 || start >= numDevices || end < 0 || end >= numDevices) {
        printf("Dispositivos invalidos.\n");
    } else {
        findShortestPath(&graph, start, end);
    }

    return 0;
}

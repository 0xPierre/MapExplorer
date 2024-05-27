#include "ShortestPath.h"
#include "BinaryHeap.h"
#include "Logger.h"

Path *Graph_shortestPath(Graph *graph, int start, int end) {
    int size = Graph_size(graph);

    assert(0 <= start && start < size);
    assert(end < size);

    int *predecessors = (int *)calloc(size, sizeof(int));
    AssertNew(predecessors);

    double *distances = (double *)calloc(size, sizeof(double));
    AssertNew(distances);

    Graph_dijkstra(graph, start, end, predecessors, distances);

    Path *path = Graph_dijkstraGetPath(predecessors, distances, end);

    free(predecessors);
    predecessors = NULL;
    free(distances);
    distances = NULL;

    return path;
}

//void Graph_dijkstra(Graph *graph, int start, int end, int *predecessors, double *distances) {
//    int size = Graph_size(graph);
//
//    assert(0 <= start && start < size);
//    assert(end < size);
//
//    bool *explored = (bool *) calloc(Graph_size(graph), sizeof(bool));
//    AssertNew(explored);
//
//    for (int i = 0; i < size; i++) {
//        predecessors[i] = -1;
//        distances[i] = INFINITY;
//    }
//    distances[start] = 0.0f;
//
//    while (true) {
//        // Recherche le noeud de distance minimale
//        int currID = -1;
//        double currDist = INFINITY;
//        for (int i = 0; i < size; ++i) {
//            if (explored[i] == false && distances[i] < currDist) {
//                currDist = distances[i];
//                currID = i;
//            }
//        }
//
//        // Condition d'arret
//        if (currID < 0 || currID == end)
//            break;
//
//        explored[currID] = true;
//
//        // Met à jour les voisins
//        for (ArcList *arc = Graph_getArcList(graph, currID);
//             arc != NULL; arc = arc->next) {
//            int nextID = arc->target;
//            double dist = distances[currID] + arc->weight;
//            if (distances[nextID] > dist) {
//                distances[nextID] = dist;
//                predecessors[nextID] = currID;
//            }
//        }
//    }
//    free(explored);
//}

void Graph_dijkstra(Graph *graph, int start, int end, int *predecessors, double *distances) {
    assert(0 <= start && start < Graph_size(graph));
    assert(end < Graph_size(graph));

    bool *explored = (bool *) calloc(Graph_size(graph), sizeof(bool));
    AssertNew(explored);

    BinaryHeap *heap = BinaryHeap_create(graph->size);

    for (int i = 0; i < Graph_size(graph); i++) {
        predecessors[i] = -1;
        distances[i] = INFINITY;
    }
    distances[start] = 0.0f;
    BinaryHeap_addNode(heap, start, 0.0f);

    while (!BinaryHeap_isEmpty(heap)) {
        int currID = BinaryHeap_extractMin(heap); // Get min distance

        // Stop condition
        if (currID == end) break;

        explored[currID] = true;

        for (ArcList *arc = graph->nodes[currID].arcList; arc != NULL; arc = arc->next) {
            int nextID = arc->target;
            if (!explored[nextID]) {
                double dist = distances[currID] + arc->weight;
                if (distances[nextID] > dist) {
                    distances[nextID] = dist;
                    predecessors[nextID] = currID;
                    BinaryHeap_addNode(heap, nextID, dist);
                }
            }
        }
    }
    free(explored);
    BinaryHeap_destroy(heap);
}

Path *Graph_dijkstraGetPath(int *predecessors, double *distances, int end) {
    assert(predecessors && distances);
    assert(end >= 0);

    if (predecessors[end] < 0) {
        return NULL;
    }

    Path *path = Path_create(end);
    int currID = end;

    while (predecessors[currID] >= 0) {
        currID = predecessors[currID];
        ListInt_insertFirst(path->list, currID);
    }

    path->distance = distances[end];

    return path;
}

Path *Path_create(int start) {
    Path *path = (Path *) calloc(1, sizeof(Path));
    AssertNew(path);

    path->distance = 0.0f;
    path->list = ListInt_create();
    ListInt_insertLast(path->list, start);

    return path;
}

void Path_destroy(Path *path) {
    if (path == NULL) return;

    ListInt_destroy(path->list);
    free(path);
}

void Path_print(Path *path) {
    if (path == NULL) {
        printf("path : NULL\n");
        return;
    }

    printf("path (distance = %f) : ", path->distance);
    ListInt_print(path->list);
}
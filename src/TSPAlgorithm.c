#include "Settings.h"
#include "ShortestPath.h"
#include "Graph.h"
#include "TSPAlgorithm.h"
#include "ListInt.h"

Path *Graph_tspFromHeuristic(Graph *graph, int station) {
    int prev = station;
    Path *path = Path_create(station);

    bool *summits_done = calloc(graph->size, sizeof(bool));
    AssertNew(summits_done);

    summits_done[station] = true;

    for (int i = 0; i < graph->size - 1; i++) {
        int next = -1;
        double min = INFINITY;

        ArcList *arc = graph->nodes[prev].arcList;
        while (arc) {
            if (arc->weight < min && !summits_done[arc->target]) {
                min = arc->weight;
                next = arc->target;
            }
            arc = arc->next;
        }

        if (next != -1) {
            summits_done[next] = true;
            ListInt_insertLast(path->list, next);
            prev = next;
            path->distance += min;
        }
    }

    ListInt_insertLast(path->list, station);

    free(summits_done);

    return path;
}


double *
Graph_acoGetProbabilities(Graph *graph, Graph *pheromones, int station, bool *explored, double alpha, double beta) {
    double *probabilities = (double *) calloc(graph->size, sizeof(double));
    AssertNew(probabilities);

    double sum = 0;


    for (int i = 0; i < graph->size; i++) {
        if (!explored[i]) {
            probabilities[i] = powf(*Graph_getArc(pheromones, station, i), alpha) *
                               powf(1 / *Graph_getArc(graph, station, i), beta);
            sum += probabilities[i];
        }
    }

    for (int i = 0; i < graph->size; i++) {
        probabilities[i] /= sum;
    }

    return probabilities;
}


Path *Graph_acoConstructPath(Graph *graph, Graph *pheromones, int station, double alpha, double beta) {
    Path *path = Path_create(station);
    bool *inserted = calloc(graph->size, sizeof(bool));
    AssertNew(inserted);

    inserted[station] = true;
    int current = station;

    for (int i = 0; i < graph->size - 1; i++) {
        double *probabilities = Graph_acoGetProbabilities(graph, pheromones, current, inserted, alpha, beta);
//        for (int k = 0; k < graph->size; k++) {
//            printf("%f ", probabilities[k]);
//        }
        double random = (double) rand() / RAND_MAX;

        double sum = 0;
        int next = -1;

        for (int j = 0; j < graph->size; j++) {
            if (inserted[j]) continue;

            sum += probabilities[j];
            if (random <= sum) {
                next = j;
                break;
            }
        }

        if (next == -1) {
            for (int j = 0; j < graph->size; j++) {
                if (!inserted[j]) {
                    next = j;
                    break;
                }
            }
        }


        // We replay the random to avoid to always take the same path from the same node. ( ask P ).
        /*
         * If the random is less than the ACO_CANCELLED_PROBABILITY, we take the path with the lowest probability.
         */
        if (USE_ACO_CANCELLED_PROBABILITY) {
            random = (double) rand() / RAND_MAX;
            if (random < ACO_CANCELLED_PROBABILITY) {
                double min = INFINITY;
                for (int j = 0; j < graph->size; j++) {
                    if (inserted[j]) continue;
                    if (probabilities[j] < min) {
                        min = probabilities[j];
                        next = j;
                    }
                }
            }
        }

        /*
         * If the random is less than the ACO_RANDOM_PROBABILITY, we take a random path.
         */
        if (USE_ACO_RANDOM_PROBABILITY) {
            random = (double) rand() / RAND_MAX;
            if (random < ACO_RANDOM_PROBABILITY) {
                int array_of_index[graph->size];
                int index = 0;

                for (int j = 0; j < graph->size; j++) {
                    if (!inserted[j]) {
                        array_of_index[index] = j;
                        index++;
                    }
                }

                next = array_of_index[rand() % index];
            }
        }

        /*
         * If the random is less than the ACO_SECOND_BEST, we take the second best path.
         */
        if (USE_ACO_SECOND_BEST) {
        }

        inserted[next] = true;
        ListInt_insertLast(path->list, next);
        path->distance += *Graph_getArc(graph, current, next);
        free(probabilities);
        current = next;
    }

    ListInt_insertLast(path->list, station);
    path->distance += *Graph_getArc(graph, current, station);

    free(inserted);

    return path;
}


void Graph_acoPheromoneGlobalUpdate(Graph *pheromones, double rho) {
    for (int i = 0; i < pheromones->size; i++) {
        for (int j = 0; j < pheromones->size; j++) {
            Graph_setArc(pheromones, i, j, *Graph_getArc(pheromones, i, j) * (1 - rho));
        }
    }
}

void Graph_acoPheromoneUpdatePath(Graph *pheromones, Path *path, double q) {
    ListIntIter *iter = ListIntIter_create(path->list);
    if (!ListIntIter_isValid(iter)) return;
    int prev = ListIntIter_get(iter);
    ListIntIter_next(iter);

    while (ListIntIter_isValid(iter)) {
        int next = ListIntIter_get(iter);

//		Graph_setArc(pheromones, prev, next, *Graph_getArc(pheromones, prev, next) + q / path->distance);
        *Graph_getArc(pheromones, prev, next) += q / path->distance;
        prev = next;
        ListIntIter_next(iter);
    }

    ListIntIter_destroy(iter);
}


Path *Graph_tspFromACO(Graph *graph, int station, int iterationCount, int antCount, double alpha, double beta, double rho,
                       double q) {
    Graph *pheromones = Graph_create(graph->size);

    // Initialize pheromones
    for (int i = 0; i < graph->size; i++) {
        for (int j = 0; j < graph->size; j++) {
            Graph_setArc(pheromones, i, j, 1.f);
        }
    }
    if (USE_GLOUTON_PHEROMONE_INIT) {
        debug("Using glouton pheromone init\n");
        Path *glouton = Graph_tspFromHeuristic(graph, station);
        Graph_acoPheromoneUpdatePath(pheromones, glouton, q);
        Path_destroy(glouton);
    }
    Path *bestPath = NULL;
    double bestDistance = INFINITY;

    for (int i = 0; i < iterationCount; i++) {
//        debug("Iteration %d\n", i);
        Path **paths = calloc(antCount, sizeof(Path *));
        AssertNew(paths);

        for (int j = 0; j < antCount; j++) {
            paths[j] = Graph_acoConstructPath(graph, pheromones, station, alpha, beta);

            if (paths[j]->distance < bestDistance) {
                bestDistance = paths[j]->distance;
                if (bestPath) {
                    //Path_destroy(bestPath);
                }
                bestPath = paths[j];
//				debug("New best %p\n", bestPath);
            } else {
                //Path_destroy(path);
            }

        }

        Graph_acoPheromoneGlobalUpdate(pheromones, rho);
        for (int j = 0; j < antCount; j++) {
            Graph_acoPheromoneUpdatePath(pheromones, paths[j], q);
            if (bestPath != paths[j])
                Path_destroy(paths[j]);
        }

//		debug("Best %p\n", bestPath);
        free(paths);
    }

    Graph_destroy(pheromones);

    return bestPath;
}

Path *Graph_FindTheBestPathNFactoriel(Graph *graph, int station) {
    ListInt *currentList = ListInt_create();
    AssertNew(currentList);
    ListInt_insertLast(currentList, station);

    bool *visited = calloc(graph->size, sizeof(bool));
    AssertNew(visited);
    visited[station] = true;

    Path *bestPath = Path_create(station);
    bestPath->distance = INFINITY;

    Graph_FindTheBestPathNFactorielHelper(graph, station, station, currentList, visited, 0, bestPath);

    ListInt_insertLast(bestPath->list, station);

    free(visited);
    ListInt_destroy(currentList);

    return bestPath;
}

void Graph_FindTheBestPathNFactorielHelper(Graph *graph, int start, int current, ListInt *currentList, bool *visited,
                                           double currentDistance, Path *bestPath) {
    if (currentList->nodeCount == graph->size) {
        double lastDistance = *Graph_getArc(graph, current, start);

        currentDistance += lastDistance;
        if (currentDistance < bestPath->distance) {
            bestPath->distance = currentDistance;
            ListInt_destroy(bestPath->list);
            bestPath->list = ListInt_copy(currentList);
        }
        return;
    }

    for (int i = 0; i < graph->size; i++) {
        if (!visited[i]) {
            visited[i] = true;
            ListInt_insertLast(currentList, i);
            double newDistance = currentDistance + *Graph_getArc(graph, current, i);
            Graph_FindTheBestPathNFactorielHelper(graph, start, i, currentList, visited, newDistance, bestPath);
            ListInt_popLast(currentList);
            visited[i] = false;
        }
    }
}

Path* twoOptSwap(Path *path, int i, int k) {
    Path *newPath = Path_create(path->list->sentinel.next->value);
    AssertNew(newPath);

    ListIntIter *iter = ListIntIter_create(path->list);
    AssertNew(iter);
    ListIntIter_next(iter);

    while (ListIntIter_isValid(iter)) {
        ListInt_insertLast(newPath->list, ListIntIter_get(iter));
        if (ListIntIter_get(iter) == i) {
            break;
        }
        ListIntIter_next(iter);
    }

    ListInt *tmp = ListInt_create();
    ListIntIter_next(iter);
    while (ListIntIter_isValid(iter)) {
        ListInt_insertFirst(tmp, ListIntIter_get(iter));
        if (ListIntIter_get(iter) == k) {
            break;
        }
        ListIntIter_next(iter);
    }

    ListIntIter *tmpIter = ListIntIter_create(tmp);
    while (ListIntIter_isValid(tmpIter)) {
        ListInt_insertLast(newPath->list, ListIntIter_get(tmpIter));
        ListIntIter_next(tmpIter);
    }

    if (ListIntIter_isValid(iter)) {
        ListIntIter_next(iter);
    }
    while (ListIntIter_isValid(iter)) {
        ListInt_insertLast(newPath->list, ListIntIter_get(iter));
        ListIntIter_next(iter);
    }

    ListInt_destroy(tmp);
    ListIntIter_destroy(tmpIter);
    ListIntIter_destroy(iter);

    return newPath;
}

void calculateDistance(Graph *graph, Path *path) {
    path->distance = 0;
    ListIntIter *iter = ListIntIter_create(path->list);
    AssertNew(iter);

    int prev = ListIntIter_get(iter);
    ListIntIter_next(iter);

    int next;
    while (ListIntIter_isValid(iter)) {
        next = ListIntIter_get(iter);
        path->distance += *Graph_getArc(graph, prev, next);
        prev = next;
        ListIntIter_next(iter);
    }

    if (next != path->list->sentinel.next->value) {
        path->distance = INFINITY;
    }

    ListIntIter_destroy(iter);
}

Path *twoOpt(Graph *graph, Path *path) {
//    debug("Actual distance : %f\n", path->distance);
    double bestDistance = path->distance;
    Path *bestPath = NULL;
    bool improved = true;

//    debug("%p", path);
//    Path *newPath = twoOptSwap(path, 28, 31);
//    calculateDistance(graph, newPath);
//    debug("%p", newPath);
//    exit(1);

    while (improved) {
        improved = false;
        for (int i = 0; i < ListInt_size(path->list) - 1; i++) {
            for (int k = 1; k < ListInt_size(path->list); k++) {
                Path *newPath = twoOptSwap(path, i, k);
                calculateDistance(graph, newPath);

                if (newPath->distance < bestDistance) {
                    bestDistance = newPath->distance;
                    improved = true;
                    if (bestPath) {
                        Path_destroy(bestPath);
                    }
                    bestPath = newPath;
                } else {
                    Path_destroy(newPath);
                }
            }
        }

    }

    if (bestPath) {
        Path_destroy(path);
        return bestPath;
    } else {
        return path;
    }
}
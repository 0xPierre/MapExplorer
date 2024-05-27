//#include "MaxFlow.h"
//#include "ListInt.h"
//
//ListInt *Graph_findImprovingPath(Graph *residual, int s, int t) {
//    int size = Graph_size(residual);
//
//    int *pred = (int *) calloc(size, sizeof(int));
//    AssertNew(pred);
//
//    for (int i = 0; i < size; i++) {
//        pred[i] = -1;
//    }
//    pred[s] = s;
//
//    ListInt *queue = ListInt_create();
//    ListInt_enqueue(queue, s);
//    while (ListInt_isEmpty(queue) == false) {
//        int currID = ListInt_dequeue(queue);
//        if (currID == t)
//            break;
//
//        for (ArcList *arc = Graph_getArcList(residual, currID);
//             arc != NULL; arc = arc->next) {
//            int nextID = arc->target;
//            if (pred[nextID] < 0 && arc->weight > 0.f) {
//                pred[nextID] = currID;
//
//                if (nextID == t) goto END_LOOP;
//
//                ListInt_enqueue(queue, nextID);
//            }
//
//        }
//    }
//    END_LOOP:
//    ListInt_destroy(queue);
//    queue = NULL;
//
//    if (pred[t] < 0) {
//        free(pred);
//        return NULL;
//    }
//
//    ListInt *path = ListInt_create();
//    int currID = t;
//    ListInt_insertFirst(path, currID);
//
//    while (currID != s) {
//        currID = pred[currID];
//        ListInt_insertFirst(path, currID);
//    }
//
//    free(pred);
//
//    return path;
//}
//
//double Graph_getImprovingPathScore(Graph *residual, ListInt *path) {
//    double score = INFINITY;
//    ListIntIter *iter = ListIntIter_create(path);
//    assert(ListIntIter_isValid(iter));
//
//    int prevID = ListIntIter_get(iter);
//    ListIntIter_next(iter);
//
//    while (ListIntIter_isValid(iter)) {
//        int currID = ListIntIter_get(iter);
//
//        ArcData *data = Graph_getArc(residual, prevID, currID);
//        AssertNew(data);
//        score = fminf(score, data->weight);
//
//        ListIntIter_next(iter);
//        prevID = currID;
//    }
//    ListIntIter_destroy(iter);
//    iter = NULL;
//    return score;
//}
//
//Graph *Graph_initResidual(Graph *capacities) {
//    int size = Graph_size(capacities);
//    Graph *residual = Graph_create(size);
//
//    for (int currID = 0; currID < size; currID++) {
//        for (ArcList *arc = Graph_getArcList(capacities, currID);
//             arc != NULL; arc = arc->next) {
//            int nextID = arc->target;
//            ArcData data = {0};
//
//            data.weight = arc->data.weight;
//            Graph_setArc(residual, currID, nextID, &data);
//
//            data.weight = 0.0f;
//            Graph_setArc(residual, nextID, currID, &data);
//        }
//    }
//
//    return residual;
//}
//
//void Graph_updateResidual(Graph *residual, ListInt *path, double score) {
//    ListIntIter *iter = ListIntIter_create(path);
//    int prevID = ListIntIter_get(iter);
//    ListIntIter_next(iter);
//
//    while (ListIntIter_isValid(iter)) {
//        int currID = ListIntIter_get(iter);
//
//        ArcData *residualData = Graph_getArc(residual, prevID, currID);
//        residualData->weight -= score;
//
//        residualData = Graph_getArc(residual, currID, prevID);
//        residualData->weight += score;
//
//        ListIntIter_next(iter);
//        prevID = currID;
//    }
//    ListIntIter_destroy(iter);
//    iter = NULL;
//}
//
//Graph *Graph_createFlowFromResidual(Graph *residual, Graph *capacities) {
//    int size = Graph_size(capacities);
//    Graph *flow = Graph_create(size);
//    for (int currID = 0; currID < size; currID++) {
//        for (ArcList *arc = Graph_getArcList(capacities, currID);
//             arc != NULL; arc = arc->next) {
//            int nextID = arc->target;
//            ArcData *residualData = Graph_getArc(residual, nextID, currID);
//            Graph_setArc(flow, currID, nextID, residualData);
//        }
//    }
//    return flow;
//}
//
//Graph *Graph_maxFlow(Graph *capacities, int s, int t) {
//    assert(capacities);
//    assert(s != t);
//
//    Graph *residual = Graph_initResidual(capacities);
//
//    while (true) {
//        ListInt *path = Graph_findImprovingPath(residual, s, t);
//        if (path == NULL || ListInt_size(path) < 2) {
//            ListInt_destroy(path);
//            break;
//        }
//
//        double score = Graph_getImprovingPathScore(residual, path);
//        Graph_updateResidual(residual, path, score);
//
//        ListInt_destroy(path);
//    }
//
//    Graph *flow = Graph_createFlowFromResidual(residual, capacities);
//    Graph_destroy(residual);
//
//    return flow;
//}
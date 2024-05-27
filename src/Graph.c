#include "Graph.h"

Graph *Graph_create(int size)
{
    Graph *graph = (Graph *)calloc(1, sizeof(Graph));
    AssertNew(graph);

    graph->size = size;

    graph->nodes = (GraphNode *)calloc(size, sizeof(GraphNode));
    AssertNew(graph->nodes);

    for (int i = 0; i < size; i++)
    {
        GraphNode *node = &(graph->nodes[i]);

        node->arcList = NULL;
        node->arcCount = 0;
        node->id = i;
    }

    return graph;
}

void Graph_destroy(Graph *graph)
{
    if (!graph) return;

    if (graph->nodes)
    {
        for (int i = 0; i < graph->size; i++)
        {
            GraphNode *node = &(graph->nodes[i]);
            ArcList *currArc = node->arcList;
            while (currArc != NULL)
            {
                ArcList *nextArc = currArc->next;
                free(currArc);
                currArc = nextArc;
            }
        }
        free(graph->nodes);
    }

    free(graph);
}

void Graph_setArc(Graph *graph, int u, int v, double weight)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    assert(0 <= v && v < graph->size);

    // Recherche si l'arc existe
    ArcList *currArc = graph->nodes[u].arcList;
    while (currArc != NULL)
    {
        if (currArc->target == v)
            break;

        currArc = currArc->next;
    }

    if (currArc)
    {
        // L'arc est déjà présent, on modifie ses données
        currArc->weight = weight;
    }
    else
    {
        // L'arc n'existe pas dans le graphe
        // On l'ajoute en début de list
        currArc = (ArcList *)calloc(1, sizeof(ArcList));
        AssertNew(currArc);

        currArc->next = graph->nodes[u].arcList;
        currArc->target = v;
        currArc->weight = weight;

        graph->nodes[u].arcList = currArc;

        graph->nodes[u].arcCount++;
    }
}

void Graph_removeArc(Graph *graph, int u, int v)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    assert(0 <= v && v < graph->size);

    // Recherche si l'arc existe
    ArcList *prevArc = NULL;
    ArcList *currArc = graph->nodes[u].arcList;
    while (currArc != NULL)
    {
        if (currArc->target == v)
            break;

        prevArc = currArc;
        currArc = currArc->next;
    }

    if (currArc)
    {
        // On supprime l'arc
        if (prevArc)
        {
            prevArc->next = currArc->next;
        }
        else
        {
            graph->nodes[u].arcList = currArc->next;
        }
        free(currArc);

        graph->nodes[u].arcCount--;
    }
}

double *Graph_getArc(Graph *graph, int u, int v)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    assert(0 <= v && v < graph->size);

    ArcList *currArc = graph->nodes[u].arcList;
    while (currArc)
    {
        if (currArc->target == v)
            break;

        currArc = currArc->next;
    }

    if (currArc)
        return &(currArc->weight);

    return NULL;
}

int Graph_maxDegree(Graph* graph)
{
    int max = 0;
    int size = graph->size;

    for (int u = 0; u < size; u++)
    {
        int degree = Graph_getArcCount(graph, u);
        if (degree > max)
        {
            max = degree;
        }
    }
    return max;
}

Graph *Graph_load(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        assert(false);
        return NULL;
    }

    Graph *graph = NULL;

    int scanCount, size, arcCount;
    scanCount = fscanf(file, "%d %d", &size, &arcCount);
    assert(scanCount == 2);

    graph = Graph_create(size);
    for (int i = 0; i < arcCount; i++)
    {
        int u, v;
        double weight;
        scanCount = fscanf(file, "%d %d %lf", &u, &v, &weight);
        if (scanCount != 3)
        {
            // It often happens when the weight is not provided in the file
            weight = 1.0;
        }

        Graph_setArc(graph, u, v, weight);
    }

    fclose(file);

    return graph;
}

void Graph_print(Graph *graph)
{
    assert(graph);

    int size = graph->size;
    for (int currID = 0; currID < size; currID++)
    {
        printf("Noeud %d : ", currID);
        for (ArcList *arc = Graph_getArcList(graph, currID);
             arc != NULL; arc = arc->next)
        {
            printf("(tgt %d, wgt %.2f), ", arc->target, arc->weight);
        }
        printf("\n");
    }
}

void Graph_save(Graph *graph, char *filename, int arc_count)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        assert(false);
        return;
    }

    fprintf(file, "%d %d\n", graph->size, arc_count);

    for (int u = 0; u < graph->size; u++)
    {
        for (ArcList *arc = Graph_getArcList(graph, u);
             arc != NULL; arc = arc->next)
        {
            fprintf(file, "%d %d %.2f\n", u, arc->target, arc->weight);
        }
    }

    fclose(file);
}


void Graph_write_to_file(const Graph* graph) {
    if (graph == NULL) {
        error("Empty GRAPH");
        return;
    }

    FILE* file = fopen(OUTPUT_DIR "/nodes.graph", "w");
    if (file == NULL) {
        error("Error while opening file\n");
        return;
    }

    fprintf(file, "%d\n", graph->size);

    for (int i = 0; i < graph->size; i++) {
        fprintf(file, "%d %d\n", i, graph->nodes[i].arcCount);

        ArcList* currentArc = graph->nodes[i].arcList;
        while (currentArc != NULL) {
            fprintf(file, "%d %.2f\n", currentArc->target, currentArc->weight);
            currentArc = currentArc->next;
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

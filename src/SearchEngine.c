#include "SearchEngine.h"
#include "ShortestPath.h"
#include "TSPAlgorithm.h"
#include "Graph.h"
#include "FileParser.h"

Address** IntersectQuery(Address** addresses, int address_count, City* city, int* address_count_found) {
    Address** result = (Address**) calloc(address_count, sizeof(Address*));
    *address_count_found = 0;

    debug("IntersectQuery %s %d\n", city->slug, address_count);

    for (int i = 0; i < address_count; i++) {
        if (strcmp(addresses[i]->slug_commune, city->slug) == 0) {
            info("HELLO !\n");
            result[*address_count_found] = addresses[i];
            (*address_count_found)++;
        }
    }

    return result;
}

Path *GenerateTourFromMap(double **positions, int positionCount, SpatialMap *map, Graph *graph, double **coords, Path****paths, int *nodes_id) {
    debug("Generating tour\n");

    for (int i = 0; i < positionCount; i++) {
        GraphNode *node = SpatialMap_FindNearestNode(map, positions[i][0], positions[i][1]);
        if (node == NULL) {
            warn("Node not found for %d\n", i);
            continue;
        }
        debug("Node found: %d %f %f\n", node->id, node->lat, node->lon);
        nodes_id[i] = node->id;
    }

    *paths = getPathMatrix(positionCount, nodes_id, graph);

    Graph *newGraph = getSubGraph(*paths, positionCount, nodes_id, graph);

    return Graph_tspFromACO(newGraph, 0, 1000, 100, 1, 2, 0.1, 10);
}


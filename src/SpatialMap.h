#pragma once
#include "Graph.h"


typedef struct Node {
    GraphNode* graph_node;
    struct Node* next;
} Node;

typedef struct SpatialIndexRegion {
    Node *node;
    int node_count;
} SpatialIndexRegion;

typedef struct SpatialMap {
    SpatialIndexRegion*** regions;
    int row_count;
    int col_count;
    double width;
    double height;
    double x;
    double y;
} SpatialMap;

SpatialMap* SpatialMap_Create(double x, double y, double height, double width, int col_count, int row_count);

void SpatialMap_insertNode(SpatialMap* map, GraphNode* graph_node);

GraphNode* SpatialMap_FindNearestNode(SpatialMap* map, double lat, double lon);

double haversine_km(double lat1, double long1, double lat2, double long2);

/*
 * Debug function to print the number of nodes in each region
 */
void DebugRegionsNodeCount(SpatialMap* map);

void SpatialMap_destroy(SpatialMap* map);

void DisplaySpatialGrid(SpatialMap* map);
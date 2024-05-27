#include "SpatialMap.h"
#include "Graph.h"
#include "FileParser.h"
#include "ShortestPath.h"

#define d2r (M_PI / 180.0)
#define radius 6371

double haversine_km(double lat1, double long1, double lat2, double long2) {
    double a = 0.5 - cos((lat2 - lat1) * d2r) / 2 +
               cos(lat1 * d2r) * cos(lat2 * d2r) *
               (1 - cos((long2 - long1) * d2r)) / 2;

    return 2 * radius * asin(sqrt(a));
}

void DebugRegionsNodeCount(SpatialMap* map) {
    for (int i = 0; i < map->col_count; i++) {
        for (int j = 0; j < map->row_count; j++) {
            SpatialIndexRegion* region = map->regions[i][j];
            debug("Region %d %d Node: %d\n", i, j, region->node_count);
        }
    }
}

SpatialMap* SpatialMap_Create(double x, double y, double height, double width, int col_count, int row_count){
    SpatialMap* map = calloc(1, sizeof(SpatialMap));
    AssertNew(map);
    map->col_count = col_count;
    map->row_count = row_count;
    map->width = width;
    map->height = height;
    map->x = x;
    map->y = y;

    map->regions = calloc(col_count * row_count, sizeof(SpatialIndexRegion*));
    AssertNew(map->regions);

    for (int i = 0; i < col_count; i++) {
        SpatialIndexRegion** regions = calloc(row_count, sizeof(SpatialIndexRegion*));
        AssertNew(regions);
        map->regions[i] = regions;

        for (int j = 0; j < row_count; j++) {
            SpatialIndexRegion* region = calloc(1, sizeof(SpatialIndexRegion));
            AssertNew(region);
            map->regions[i][j] = region;
        }
    }

    return map;
}

void SpatialMap_insertNode(SpatialMap* map, GraphNode* graph_node){
    double col_size = map->width / map->row_count;
    double row_size = map->height / map->col_count;

    int col = (int)floor((graph_node->lon - map->x) / col_size);
    int row = (int)floor((graph_node->lat - map->y) / row_size);


    if (col < 0 || col >= map->col_count || row < 0 || row >= map->row_count) {
        return;
    }

    SpatialIndexRegion* region = map->regions[col][row];
    if (region == NULL) {
        debug("Region not found\n");
        return;
    }

    Node* node = calloc(1, sizeof(Node));
    AssertNew(node);
    node->graph_node = graph_node;

    if (region->node == NULL)
        region->node = node;
    else {
        node->next = region->node;
        region->node = node;
    }

    region->node_count++;
}

GraphNode* SpatialMap_findNearestNodeInRegion(SpatialIndexRegion *region, double lat, double lon, double *return_distance) {
    Node* node = region->node;

    GraphNode* nearest = node->graph_node;
    double min_distance = haversine_km(lat, lon, nearest->lat, nearest->lon);
    while (node != NULL) {
        double distance = haversine_km(lat, lon, node->graph_node->lat, node->graph_node->lon);
        if (distance < min_distance) {
            min_distance = distance;
            nearest = node->graph_node;
        }
        node = node->next;
    }

    *return_distance = min_distance;
    return nearest;
}

GraphNode* SpatialMap_FindNearestNode(SpatialMap* map, double lat, double lon) {
    double col_size = map->width / map->row_count;
    double row_size = map->height / map->col_count;

    int col = (int)floor((lon - map->x) / col_size);
    int row = (int)floor((lat - map->y) / row_size);

    if (col < 0 || col >= map->col_count || row < 0 || row >= map->row_count) {
        warn("Region out of rect | not found\n");
        return NULL;
    }

    SpatialIndexRegion* region = map->regions[col][row];
    if (region == NULL) {
        debug("Region not found\n");
        return NULL;
    }

    debug("Region found %d \n", region->node_count);

    // First check in the region, if not found, check in the adjacent regions
    double min_distance = INFINITY;
    GraphNode* nearest;
    if (region->node_count > 0) {
        nearest = SpatialMap_findNearestNodeInRegion(region, lat, lon, &min_distance);
        if (nearest != NULL) {
            return nearest;
        }
    }

    // Check in the adjacent regions
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int new_col = col + i;
            int new_row = row + j;

            if (new_col < 0 || new_col >= map->col_count || new_row < 0 || new_row >= map->row_count) {
                continue;
            }

            SpatialIndexRegion* new_region = map->regions[new_col][new_row];
            if (new_region == NULL || new_region->node_count == 0) {
                continue;
            }

            double distance = INFINITY;
            GraphNode* new_nearest = SpatialMap_findNearestNodeInRegion(new_region, lat, lon, &distance);
            if (new_nearest != NULL) {
                if (distance < min_distance) {
                    min_distance = distance;
                    nearest = new_nearest;
                }
            }
        }
    }

    return nearest;
}

void SpatialMap_destroy(SpatialMap* map) {
    for (int i = 0; i < map->col_count; i++) {
        for (int j = 0; j < map->row_count; j++) {
            SpatialIndexRegion* region = map->regions[i][j];
            if (region != NULL) {
                Node* node = region->node;
                while (node != NULL) {
                    Node* next = node->next;
                    free(node);
                    node = next;
                }
                free(region);
            }
        }
        free(map->regions[i]);
    }
    free(map->regions);
    free(map);
}

void DisplaySpatialGrid(SpatialMap* map) {
    FILE* file = fopen("../output/grid.geojson", "w");
    AssertNew(file);

    GeoJSONHeaderWriter(file);

    double **coords;
    for (int i = 0; i < map->row_count; i++) {
        coords = calloc(map->col_count, sizeof(double*));
        for (int j = 0; j < map->col_count; j++) {
            AssertNew(coords);

            coords[j] = calloc(2, sizeof(double));
            AssertNew(coords[j]);

            coords[j][0] = map->x + j * map->width / map->col_count;
            coords[j][1] = map->y + i * map->height / map->row_count;
        }
        GeoJSONCoordsWriter(file, map->col_count, coords);
        // add ',' if not last iteration
        if (i != map->row_count - 1) {
            fprintf(file, ",\n");
        }
    }

    GeoJSONFooterWriter(file);
}
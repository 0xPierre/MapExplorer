#include "Settings.h"
#include "Graph.h"
#include "ShortestPath.h"
#include "ListInt.h"
#include "FileParser.h"
#include "TSPAlgorithm.h"
#include "Evolution.h"


int *ParseInputFile(char *filename, char *graph_path, char *coords_path, int *node_count) {
    FILE *file = fopen(filename, "r");
    assert(file);

    if (fscanf(file, "%s\n%s\n%d", graph_path, coords_path, node_count) != 3) {
        error("Parsing error\n");
        exit(1);
    }

    debug("Graph file: %s\nCoords File: %s\nNode count: %d\n", graph_path, coords_path, *node_count);
    int *nodes_id = (int *) calloc(*node_count, sizeof(int));
    AssertNew(nodes_id);

    for (int i = 0; i < *node_count; i++) {
        if (fscanf(file, "%d", &nodes_id[i]) != 1) {
            error("Parsing error\n");
            exit(1);
        }
    }
    fclose(file);

    return nodes_id;
}

double **loadLatituteAndLongitudeFromFile(char *filename, int *node_count, Graph *graph) {
    FILE *file = fopen(filename, "r");
    assert(file);

    if (fscanf(file, "%d", node_count) != 1) {
        error("Parsing error\n");
        exit(1);
    }

    double **coords = (double **) calloc(*node_count, sizeof(double *));
    AssertNew(coords);

    for (int i = 0; i < *node_count; i++) {
        coords[i] = (double *) calloc(2, sizeof(double));
        AssertNew(coords[i]);
        if (fscanf(file, "%lf %lf", &coords[i][0], &coords[i][1]) != 2) {
            error("Parsing error\n");
            exit(1);
        }

        if (graph != NULL) {
            graph->nodes[i].lon = coords[i][0];
            graph->nodes[i].lat = coords[i][1];
        }
    }

    fclose(file);

    return coords;
}

void GeoJSONNodeWriter(FILE *file, double **coords, int *nodes_id, int node_id) {
    fprintf(file, "        {\n");
    fprintf(file, "            \"type\": \"Feature\",\n");
    fprintf(file, "            \"properties\": {\n");
    fprintf(file, "                \"_umap_options\": {\n");
    fprintf(file, "                    \"color\": \"#2196F3\",\n");
    fprintf(file, "                    \"weight\": 6,\n");
    fprintf(file, "                    \"iconClass\": \"Drop\",\n");
    fprintf(file, "                    \"showLabel\": null\n");
    fprintf(file, "                },\n");
    fprintf(file, "                \"name\": \"Point %d\"\n", node_id);
    fprintf(file, "            },\n");
    fprintf(file, "            \"geometry\": {\n");
    fprintf(file, "                \"type\": \"Point\",\n");
    fprintf(file, "                \"coordinates\": [%f,%f]\n", coords[nodes_id[node_id]][0],
            coords[nodes_id[node_id]][1]);
    fprintf(file, "            }\n");
    fprintf(file, "        }");
}

void GeoJSONCoordsWriter(FILE *file, int coords_count, double **cords) {
    fprintf(file, "        {\n");
    fprintf(file, "            \"type\": \"Feature\",\n");
    fprintf(file, "            \"properties\": {\n");
    fprintf(file, "                \"_umap_options\": {\n");
    fprintf(file, "                    \"color\": \"#2196F3\",\n");
    fprintf(file, "                    \"weight\": 6,\n");
    fprintf(file, "                    \"showLabel\": null\n");
    fprintf(file, "                },\n");

    fprintf(file, "                \"name\": \"Line\"\n");
    fprintf(file, "            },\n");
    fprintf(file, "            \"geometry\": {\n");
    fprintf(file, "                \"type\": \"LineString\",\n");
    fprintf(file, "                \"coordinates\": [\n                    ");

    for (int i = 0; i < coords_count; i++) {
        fprintf(file, "[%f,%f]", cords[i][0], cords[i][1]);
        if (i != coords_count - 1) {
            fprintf(file, ", ");
        }
    }

    fprintf(file, "\n                ]\n");
    fprintf(file, "            }\n");
    fprintf(file, "        }");
}

void GeoJSONPathWriter(FILE *file, Path *path, int depart_id, int arrival_id, double **cords) {
    fprintf(file, "        {\n");
    fprintf(file, "            \"type\": \"Feature\",\n");
    fprintf(file, "            \"properties\": {\n");
    fprintf(file, "                \"_umap_options\": {\n");
    fprintf(file, "                    \"color\": \"#2196F3\",\n");
    fprintf(file, "                    \"weight\": 6,\n");
    fprintf(file, "                    \"showLabel\": null\n");
    fprintf(file, "                },\n");

    fprintf(file, "                \"name\": \"From %d to %d\"\n", depart_id, arrival_id);
    fprintf(file, "            },\n");
    fprintf(file, "            \"geometry\": {\n");
    fprintf(file, "                \"type\": \"LineString\",\n");
    fprintf(file, "                \"coordinates\": [\n                    ");

    ListIntIter *iter = ListIntIter_create(path->list);
    while (ListIntIter_isValid(iter)) {
        int nextID = ListIntIter_get(iter);
        fprintf(file, "[%f,%f]", cords[nextID][0], cords[nextID][1]);
        // Add the trailing comma only if the next element is valid
        ListIntIter_next(iter);
        if (ListIntIter_isValid(iter)) {
            fprintf(file, ", ");
        }
    }

    fprintf(file, "\n                ]\n");
    fprintf(file, "            }\n");
    fprintf(file, "        }");

    free(iter);
}

void GeoJSONHeaderWriter(FILE *file) {
    fprintf(file, "{\n");
    fprintf(file, "    \"type\": \"FeatureCollection\",\n");
    fprintf(file, "    \"features\": [\n");
}

void GeoJSONFooterWriter(FILE *file) {
    fprintf(file, "\n    ]\n");
    fprintf(file, "}\n");
}

/// This function writes the paths in a GeoJSON file
/// @param filename the name of the file to write
/// @param paths the paths to write
/// @param coords the coordinates of the nodes
/// @param node_count the number of nodes
/// @param nodes_id the id of the nodes in the graph
void GeoJSONWriteAllPaths(char *filename, Path ***paths, double **coords, int node_count, int *nodes_id) {
    FILE *geojson = fopen(filename, "w");
    assert(geojson);

    // Keep track of the arcs that have been written to avoid duplicates
    // From 0 to 1 is the same as from 1 to 0
    bool **arc_done = (bool **) calloc(node_count, sizeof(bool *));
    AssertNew(arc_done);
    for (int i = 0; i < node_count; i++) {
        arc_done[i] = (bool *) calloc(node_count, sizeof(bool));
        AssertNew(arc_done[i]);
    }

    // Initialize the diagonal to true
    // Made for trailing comma handling
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                arc_done[i][j] = true;
            }
        }
    }

    GeoJSONHeaderWriter(geojson);

    for (int i = 0; i < node_count; i++) {
        if (i != 0) {
            fprintf(geojson, ",\n");
        }

        GeoJSONNodeWriter(geojson, coords, nodes_id, i);

        for (int j = 0; j < node_count; j++) {
            if (i == j || paths[i][j] == NULL) {
                continue;
            }

            // Check if the arc has already been written
            if (arc_done[i][j] || arc_done[j][i]) {
                continue;
            } else {
                arc_done[i][j] = true;
                arc_done[j][i] = true;
            }
            fprintf(geojson, ",\n");
            GeoJSONPathWriter(geojson, paths[i][j], i, j, coords);
        }
    }

    GeoJSONFooterWriter(geojson);

    for (int i = 0; i < node_count; i++) {
        free(arc_done[i]);
    }
    free(arc_done);

    fclose(geojson);
}

void GeoJSONWriteTSP(char *filename, Path *shortestPath, Path ***paths, double **coords, int node_count, int *nodes_id) {
    assert(shortestPath);
    FILE *geojson = fopen(filename, "w");
    assert(geojson);

    GeoJSONHeaderWriter(geojson);

    for (int i = 0; i < node_count; i++) {
        if (i != 0) {
            fprintf(geojson, ",\n");
        }

        GeoJSONNodeWriter(geojson, coords, nodes_id, i);
    }


    ListIntIter *iter = ListIntIter_create(shortestPath->list);
    while (ListIntIter_isValid(iter)) {
        int nextID = ListIntIter_get(iter);
        ListIntIter_next(iter);
        if (ListIntIter_isValid(iter)) {
            fprintf(geojson, ",\n");
        }

        if (!ListIntIter_isValid(iter)) {
            break;
        }
        int nextNextID = ListIntIter_get(iter);

        if (nextNextID == -1) {
            break;
        }

        GeoJSONPathWriter(geojson, paths[nextID][nextNextID], nextID, nextNextID, coords);
    }

    free(iter);

    GeoJSONFooterWriter(geojson);

    fclose(geojson);
}

void WriteAllNodesFromGraphToFileGeoJSON(char *filename, Graph *graph) {
    FILE *geojson = fopen(filename, "w");

    if (geojson == NULL) {
        error("File not found WANFTFGJ\n");
        exit(1);
    }

    GeoJSONHeaderWriter(geojson);

    for (int i = 0; i < graph->size; i++) {
        if (i != 0) {
            fprintf(geojson, ",\n");
        }

        fprintf(geojson, "        {\n");
        fprintf(geojson, "            \"type\": \"Feature\",\n");
        fprintf(geojson, "            \"properties\": {\n");
        fprintf(geojson, "                \"_umap_options\": {\n");
        fprintf(geojson, "                    \"color\": \"#2196F3\",\n");
        fprintf(geojson, "                    \"weight\": 6,\n");
        fprintf(geojson, "                    \"iconClass\": \"Drop\",\n");
        fprintf(geojson, "                    \"showLabel\": null\n");
        fprintf(geojson, "                },\n");
        fprintf(geojson, "                \"name\": \"Point %d\"\n", i);
        fprintf(geojson, "            },\n");
        fprintf(geojson, "            \"geometry\": {\n");
        fprintf(geojson, "                \"type\": \"Point\",\n");
        fprintf(geojson, "                \"coordinates\": [%f,%f]\n", graph->nodes[i].lon, graph->nodes[i].lat);
        fprintf(geojson, "            }\n");
        fprintf(geojson, "        }");
    }

    GeoJSONFooterWriter(geojson);

    fclose(geojson);
}


Path ***getPathMatrix(int node_count, int *nodes_id, Graph *map_graph) {
    /*
    Calculate all the shortest paths between all the nodes
    */
    Path ***paths = (Path ***) calloc(node_count, sizeof(Path **));
    AssertNew(paths);

    for (int i = 0; i < node_count; i++) {
        paths[i] = (Path **) calloc(node_count, sizeof(Path *));
        AssertNew(paths[i]);

        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                continue;
            }

            int u = nodes_id[i];
            int v = nodes_id[j];

            Path *path = Graph_shortestPath(map_graph, u, v);
            if (path == NULL) {
                error("Impossible de trouver un path\n");
                return NULL;
            }
            paths[i][j] = path;
        }
    }

    return paths;
}

Graph* getSubGraph(Path ***paths, int node_count, int *nodes_id, Graph *map_graph) {
    Graph *newGraph = Graph_create(node_count);
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                continue;
            }
            Graph_setArc(newGraph, i, j, paths[i][j]->distance);
        }
    }

    return newGraph;
}

void ParseOneTest(char *filename) {

    char graph_path[128];
    char coords_path[128];
    int node_count;
    int *nodes_id = ParseInputFile(filename, graph_path, coords_path, &node_count);

    Graph *map_graph = Graph_load(graph_path);
    int loaded_graph_size;
    double **coords = loadLatituteAndLongitudeFromFile(coords_path, &loaded_graph_size, map_graph);

    Path ***paths = getPathMatrix(node_count, nodes_id, map_graph);

    // Write all the paths
    GeoJSONWriteAllPaths(OUTPUT_DIR "all_path_1.geojson", paths, coords, node_count, nodes_id);

    // Create a new graph with the distances from the paths
    Graph *newGraph = getSubGraph(paths, node_count, nodes_id, map_graph);

    Path *shortestPath = Graph_tspFromACO(
    newGraph,
    0,
    1000,
    50,
    2.0f,
    3.0f,
    0.1,
    2.0f
    );

#if ENABLE_PATH_PLANNER
    shortestPath = twoOpt(newGraph, shortestPath);
#endif

    // Calculate the TSP
//    Path *gloutonPath = Graph_tspFromHeuristic(newGraph, 0);
    debug("%p\n", shortestPath);
    //    debug("Evolution is running...\n");
//    Evotuion_run(newGraph);
//
    //// Write the TSP
    GeoJSONWriteTSP(OUTPUT_DIR "tspoutput.geojson", shortestPath, paths, coords, node_count, nodes_id);
//    GeoJSONWriteTSP(OUTPUT_DIR "tsoutput_glouton.geojson", gloutonPath, paths, coords, node_count, nodes_id);

    Path_destroy(shortestPath);
//    Path_destroy(gloutonPath);

    Graph_destroy(newGraph);
    // Free the paths
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (paths[i][j] != NULL) {
                Path_destroy(paths[i][j]);
            }
        }
        free(paths[i]);
    }
    free(paths);
    Graph_destroy(map_graph);
    // Free coords
    for (int i = 0; i < loaded_graph_size; i++) {
        free(coords[i]);
    }
    free(coords);
    free(nodes_id);
}
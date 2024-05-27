#include "Test.h"
#include <stdio.h>
#include <stdlib.h>
#include "Graph.h"
#include "TSPAlgorithm.h"
#include "ShortestPath.h"
#include "FileParser.h"
#include "SpatialMap.h"
#include "DataPipeline.h"
#include "SearchTrie.h"

void TestPheromonInitialisationAfterGlouton(char *filename) {
    char graph_path[128];
    char coords_path[128];
    int node_count;
    int *nodes_id = ParseInputFile(filename, graph_path, coords_path, &node_count);

    Graph *map_graph = Graph_load(graph_path);
    int loaded_graph_size;
    double **coords = loadLatituteAndLongitudeFromFile(coords_path, &loaded_graph_size, map_graph);


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
            AssertNew(path);
            paths[i][j] = path;
        }
    }


    // Create a new graph with the distances from the paths
    int arc_count = node_count * (node_count - 1);
    Graph *newGraph = Graph_create(node_count);
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                continue;
            }
            Graph_setArc(newGraph, i, j, paths[i][j]->distance);
        }
    }

    // Calculate the TSP
    Path *shortestPath = Graph_tspFromACO(newGraph, 0, 1000, 100, 1, 2, 0.1, 10);
    // Calculate Glouton
    Path *shortestPathGlouton = Graph_tspFromHeuristic(newGraph, 0);



    //// Write the TSP
    GeoJSONWriteTSP(OUTPUT_DIR "tspoutput.geojson", shortestPath, paths, coords, node_count, nodes_id);
    GeoJSONWriteTSP(OUTPUT_DIR "tspoutputglouton.geojson", shortestPathGlouton, paths, coords, node_count, nodes_id);

    Path_destroy(shortestPath);

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

void TestBruteForceTSP(char *filename) {
    char graph_path[128];
    char coords_path[128];
    int node_count;
    int *nodes_id = ParseInputFile(filename, graph_path, coords_path, &node_count);

    Graph *map_graph = Graph_load(graph_path);
    int loaded_graph_size;
    double **coords = loadLatituteAndLongitudeFromFile(coords_path, &loaded_graph_size, map_graph);


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
            AssertNew(path);
            paths[i][j] = path;
        }
    }


    // Create a new graph with the distances from the paths
    int arc_count = node_count * (node_count - 1);
    Graph *newGraph = Graph_create(node_count);
    for (int i = 0; i < node_count; i++) {
        for (int j = 0; j < node_count; j++) {
            if (i == j) {
                continue;
            }
            Graph_setArc(newGraph, i, j, paths[i][j]->distance);
        }
    }

    Path *shortestPathBruteForce = Graph_FindTheBestPathNFactoriel(newGraph, 0);

    debug("%p\n", shortestPathBruteForce);
    info("Output is available in %s\n", TS_GEOJSON_OUTPUT);

    GeoJSONWriteTSP(TS_GEOJSON_OUTPUT, shortestPathBruteForce, paths, coords, node_count, nodes_id);

    Path_destroy(shortestPathBruteForce);

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

void TestNodeSearchFromLatLon(void) {
    int city_count , address_count;
//    debug("Loading cities\n");f
//    City **cities = LoadCitiesFromFile("../data/communes-departement-region.csv", &city_count);
    //debug("Loading addresses\n");
////    Address **addresses = LoadAddressesFromFile("../data/addresses-france-minified.csv", &address_count);
    Address  **addresses = GetClearedCSVAddress(DATA_DIR "adresses-53.csv", &address_count);
//    debug("%d cities, %d addresses loaded\n", city_count, address_count);
    free(addresses);
    return;

//    SearchTrie *trie = SearchTrie_Create();
//    for (int i = 0; i < address_count; i++) {
//        SearchTrie_InsertAddress(trie, addresses[i]);
//    }
//
//    for (int i = 0; i < city_count; i++) {
//        SearchTrie_InsertCity(trie, cities[i]);
//    }
//    debug("Trie created\n");

//    while (true) {
//        char *str;
//        printf("Enter a search string: ");
//        scanf("%ms", &str);
//
//        int address_count_found = 0;
//        debug("Searching for %s\n", str);
//        Address** address_found = SearchTree_AutoCompleteAddressSearch(trie, str, &address_count_found, 10, 10);
//        for (int i = 0; i < address_count_found; i++) {
//            Address_print(address_found[i]);
//        }
//
//        free(address_found);
//    }
//    SearchTrie_Destroy(trie);
//
//    for (int i = 0; i < city_count; i++) {
//        free(cities[i]);
//    }
//    free(cities);
//
//    for (int i = 0; i < address_count; i++) {
//        free(addresses[i]);
//    }
//    free(addresses);

//    debug("Address Search done\n");
//    for (int i = 0; i < address_count_found; i++) {
//        Address_print(address_found[i]);
//    }
//
//    Graph *graph = Graph_load("../data/mayenne_graph.txt");
//    int loaded_graph_size;
//    double **coords = loadLatituteAndLongitudeFromFile("../data/mayenne_inter.txt", &loaded_graph_size, graph);
//
//    SpatialMap *map = GetIntersectionGraphSpatialMap(
//            graph,
//        FRANCE_FROM_LON, FRANCE_FROM_LAT,
//          FRANCE_HEIGHT, FRANCE_WIDTH,
//        FRANCE_COL, FRANCE_ROW
//    );
//
//    GraphNode *node = SpatialMap_FindNearestNode(map, address_found[0]->lat, address_found[0]->lon);
//    debug("%d %f %f\n", node->id, node->lat, node->lon);
}

void TestSimpleAco(char *filename) {
    char graph_path[128];
    char coords_path[128];
    int node_count;
    int *nodes_id = ParseInputFile(filename, graph_path, coords_path, &node_count);

    Graph *map_graph = Graph_load(graph_path);
    int loaded_graph_size;
    double **coords = loadLatituteAndLongitudeFromFile(coords_path, &loaded_graph_size, map_graph);

    Path ***paths = getPathMatrix(node_count, nodes_id, map_graph);

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

    debug("%p\n", shortestPath);
    info("Output is available in %s\n", TS_GEOJSON_OUTPUT);
    GeoJSONWriteTSP(TS_GEOJSON_OUTPUT, shortestPath, paths, coords, node_count, nodes_id);

    Path_destroy(shortestPath);

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

void TestAddressAndNearIntersection(void) {
    int city_count , address_count;
    Address  **addresses = GetClearedCSVAddress(TEST_ADDRESS_AUTOCOMPLETE_FILE, &address_count);

    SearchTrie *trie = SearchTrie_Create();
    for (int i = 0; i < address_count; i++) {
        SearchTrie_InsertAddress(trie, addresses[i]);
    }
    debug("Trie created\n");

    int address_count_found = 0;
    debug("Searching for %s\n", TEST_ADDRESS_AUTOCOMPLETE_SEARCH);
    char *str = (char *)calloc(strlen(TEST_ADDRESS_AUTOCOMPLETE_SEARCH) + 1, sizeof(char));
    strcpy(str, TEST_ADDRESS_AUTOCOMPLETE_SEARCH);
    str = ClearString(str);
    Address** address_found = SearchTree_AutoCompleteAddressSearch(trie, str, &address_count_found, 10, 10);

    SearchTrie_Destroy(trie);

    debug("Address Search done\n");
    for (int i = 0; i < address_count_found; i++) {
        Address_print(address_found[i]);
    }

    if (address_count_found == 0) {
        error("No address found\n");
        return;
    }

    Graph *graph = Graph_load(TEST_ADDRESS_AUTOCOMPLETE_GRAPH);
    int loaded_graph_size;
    double **coords = loadLatituteAndLongitudeFromFile(TEST_ADDRESS_AUTOCOMPLETE_COORDS, &loaded_graph_size, graph);

    SpatialMap *map = GetIntersectionGraphSpatialMap(
            graph,
        FRANCE_FROM_LON, FRANCE_FROM_LAT,
          FRANCE_HEIGHT, FRANCE_WIDTH,
        FRANCE_COL, FRANCE_ROW
    );

    GraphNode *node = SpatialMap_FindNearestNode(map, address_found[0]->lat, address_found[0]->lon);
    if (node == NULL) {
        error("No node found\n");
        return;
    }

    info("Nearest node is %d | Coords =  %f %f\n", node->id, node->lat, node->lon);

    for (int i = 0; i < address_count; i++) {
        free(addresses[i]);
    }
    free(addresses);
}

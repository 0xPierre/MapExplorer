#include "Settings.h"
#include "CommandLine.h"
#include "DataPipeline.h"
#include "SearchTrie.h"
#include "Graph.h"
#include "SpatialMap.h"
#include "FileParser.h"
#include "TSPAlgorithm.h"

InitMap *initMap;
SearchTrie *trieee;

void init(void) {
    initMap = (InitMap *) calloc(1, sizeof(InitMap));
    debug("MainLoop started\n");
    int city_count, address_count;
    debug("Loading cities\n");
    City **cities = LoadCitiesFromFile(UI_COMMUNE_FILE, &city_count);
    debug("Loading addresses\n");
//    Address **addresses = GetClearedCSVAddress(162320, DATA_DIR "adresses-53.csv", &address_count);
//    Address **addresses = LoadAddressesFromFile(DATA_DIR "addresses-france-minified.csv", &address_count);
    Address  **addresses = GetClearedCSVAddress(UI_ADDRESS_FILE, &address_count);
//    Address **addresses = LoadAddressesFromFile(DATA_DIR "addresses-france-minified.csv", &address_count);
    debug("%d cities, %d addresses loaded\n", city_count, address_count);

    debug("Loading Graph & Coords\n");
    Graph *graph = Graph_load(UI_GRAPH_FILE);
    int loaded_graph_size;
    debug("Graph loaded : %d nodes\n", graph->size);
    double **coords = loadLatituteAndLongitudeFromFile(UI_COORDS_FILE, &loaded_graph_size, graph);
    debug("Coords loaded\n");

    SpatialMap *map = GetIntersectionGraphSpatialMap(
            graph,
            FRANCE_FROM_LON, FRANCE_FROM_LAT,
            FRANCE_HEIGHT, FRANCE_WIDTH,
            FRANCE_COL, FRANCE_ROW
    );


    debug("Creating Search Trie\n");
    initMap->trie = SearchTrie_Create();
    for (int i = 0; i < address_count; i++) {
        SearchTrie_InsertAddress(initMap->trie, addresses[i]);
    }

    for (int i = 0; i < city_count; i++) {
        SearchTrie_InsertCity(initMap->trie, cities[i]);
    }
    debug("Search Trie created\n");


    initMap->coords = coords;
    initMap->addresses = addresses;
    initMap->cities = cities;
    initMap->graph = graph;
    initMap->map = map;
}

void MainLoop(void) {
    debug("MainLoop started\n");
    int address_count;
//    debug("Loading cities\n");
//    City **cities = LoadCitiesFromFile(DATA_DIR "communes-departement-region.csv", &city_count);
    debug("Loading addresses\n");
    Address  **addresses = GetClearedCSVAddress(CLI_ADDRESS_FILE, &address_count);
//    Address **addresses = LoadAddressesFromFile(DATA_DIR "addresses-france-minified.csv", &address_count);
    debug("%d addresses loaded\n", address_count);

    debug("Loading Graph & Coords\n");
    Graph *graph = Graph_load(CLI_GRAPH_FILE);
    int loaded_graph_size;
    debug("Graph loaded : %d nodes\n", graph->size);
    double **coords = loadLatituteAndLongitudeFromFile(CLI_COORDS_FILE, &loaded_graph_size, graph);
    debug("Coords loaded\n");

    debug("Creating Spatial Map\n");
    SpatialMap *map = GetIntersectionGraphSpatialMap(
            graph,
            FRANCE_FROM_LON, FRANCE_FROM_LAT,
            FRANCE_HEIGHT, FRANCE_WIDTH,
            FRANCE_COL, FRANCE_ROW
    );


    debug("Creating Search Trie\n");
    trieee = SearchTrie_Create();
    for (int i = 0; i < address_count; i++) {
        SearchTrie_InsertAddress(trieee, addresses[i]);
    }

//    for (int i = 0; i < city_count; i++) {
//        SearchTrie_InsertCity(trieee, cities[i]);
//    }
    debug("Search Trie created\n");

    while (true) {
        printf("Welcome to Map Explorer\n");
        printf("1. Generate a tour\n");
        printf("2. Quit\n");
        printf("Enter your choice: ");

        char choice;
        scanf(" %c", &choice);

        switch (choice) {
            case '1':
                TourLoop(trieee, map, graph, coords);
                break;
            case '2':
                return;
            default:
                error("Invalid choice\n");
        }


    }
}

Address* AddressLoop(SearchTrie *trie) {
    while (true) {
        printf("---------------------------------------------\n");
        char query[100];
        printf("(Max results is 9) | Enter the address: ");
        scanf(" %[^\n]", query);

        char *cleared_query = ClearString(query);

        int address_count_found = 0;
        Address** address_found = SearchTree_AutoCompleteAddressSearch(trie, cleared_query, &address_count_found, 9, 20);

        // Degueu
        address_count_found = address_count_found > 9 ? 9 : address_count_found;

        if (address_count_found == 0) {
            printf("No address found\n");
        } else {
            printf("Possible addresses:\n");
            for (int i = 0; i < address_count_found; i++) {
                printf("%d. %s %s %d\n", i + 1, address_found[i]->nom_voie, address_found[i]->nom_commune, address_found[i]->code_postal);
            }
        }
        printf("y. Continue search\n");
        printf("q. Quit\n");


        printf("Your choice: ");
        char choice;
        scanf(" %c", &choice);

        if (choice == 'q') {
            return NULL;
        }
        else if (choice == 'y') {
            continue;
        }
        else {

            int choice_int = choice - '0' - 1;
            if (choice_int < 0 || choice_int > address_count_found || address_count_found == 0)  {
                error("Invalid choice\n");
                continue;
            }
            return address_found[choice_int];
        }
    }
}

void TourLoop(SearchTrie *trie, SpatialMap* map, Graph* graph, double **coords) {
    Address** addresses_to_make_loop = (Address**) calloc(10, sizeof(Address*));
    int address_to_make_loop_count = 0;
    printf("---------------------------------------------\n");
    printf("Welcome to Tour Generator\n");

    while (true) {
        printf("--------------------------------------------------------\n");
        printf("|List of addresses :                                   |\n");
        for (int i = 0; i < address_to_make_loop_count; i++) {
            printf("|%d. %s %s %d\n", i + 1, addresses_to_make_loop[i]->nom_voie, addresses_to_make_loop[i]->nom_commune, addresses_to_make_loop[i]->code_postal);
        }
        if (address_to_make_loop_count == 0) {
            printf("|No addresses added                                     |\n");
        } else {
            printf("|%d Addresses                                           |\n", address_to_make_loop_count);
        }
        printf("--------------------------------------------------------\n");

        printf("1. Add an address\n");
        printf("2. Remove an address\n");
        printf("3. Generate the tour\n");
        printf("4. Quit\n");

        char choice;
        printf("Enter your choice: ");
        scanf(" %c", &choice);


        Address *address;
        switch (choice) {
            case '1':
                address = AddressLoop(trie);
                if (address != NULL) {
                    printf("Address added\n");
                    addresses_to_make_loop[address_to_make_loop_count++] = address;
                }
                break;
            case '2':
                if (address_to_make_loop_count == 0) {
                    printf("No address to remove\n");
                } else {
                    printf("Enter the address number to remove: ");
                    int address_number;
                    scanf("%d", &address_number);
                    if (address_number < 1 || address_number > address_to_make_loop_count) {
                        error("Invalid address number\n");
                    } else {
                        address_to_make_loop_count--;
                        for (int i = address_number - 1; i < address_to_make_loop_count; i++) {
                            addresses_to_make_loop[i] = addresses_to_make_loop[i + 1];
                        }
                        printf("Address removed\n");
                    }
                }
                break;
            case '3':
                printf("Generating tour\n");
                GenerateTour(addresses_to_make_loop, address_to_make_loop_count, map, graph, coords);
            case '4':
                return;
            default:
                error("Invalid choice\n");
        }
    }
}

void GenerateTour(Address** addresses, int address_count, SpatialMap* map, Graph* graph, double **coords) {
    debug("Generating tour\n");

    int nodes_id[address_count];
    for (int i = 0; i < address_count; i++) {
        GraphNode *node = SpatialMap_FindNearestNode(map, addresses[i]->lat, addresses[i]->lon);
        if (node == NULL) {
            warn("Node not found for %s\n", addresses[i]->nom_voie);
            continue;
        }
        debug("Node found: %d %f %f\n", node->id, node->lat, node->lon);
        nodes_id[i] = node->id;
    }

    /*
    Calculate all the shortest paths between all the nodes
    */
    Path ***paths = getPathMatrix(address_count, nodes_id, graph);
    if (paths == NULL) {
        error("Certaines destinations n'ont pas pu etre lié. Merci de changer d'adresses.\n");
        return;
    }

    Graph *newGraph = getSubGraph(paths, address_count, nodes_id, graph);
//
    Path *shortestPath = Graph_tspFromACO(newGraph, 0, 1000, 100, 1, 2, 0.1, 10);

    debug("%p\n", shortestPath);
    info("Tour generated, available in %s\n", TS_GEOJSON_OUTPUT);

    GeoJSONWriteTSP(TS_GEOJSON_OUTPUT, shortestPath, paths, coords, address_count, nodes_id);
}
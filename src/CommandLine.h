#pragma once

#include "DataPipeline.h"
#include "SearchTrie.h"

typedef struct InitMap InitMap;

struct InitMap
{
    City **cities;
    Address  **addresses;
    double **coords;
    SearchTrie *trie;
    Graph *graph;
    SpatialMap *map;
};

extern InitMap *initMap;

void init(void);

void MainLoop(void);

void TourLoop(SearchTrie *trie, SpatialMap* map, Graph* graph, double **coords);

Address* AddressLoop(SearchTrie *trie);

void GenerateTour(Address** addresses, int address_count, SpatialMap* map, Graph* graph, double **coords);
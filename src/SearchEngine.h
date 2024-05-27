#include "DataPipeline.h"
#include "ShortestPath.h"

/*
 * This function take the addresses and the selected city and return the addresses that are in the city
 */
Address** IntersectQuery(Address** addresses, int address_count, City* city, int* address_count_found);

Path *GenerateTourFromMap(double **positions, int positionCount, SpatialMap *map, Graph *graph, double **coords, Path****paths, int *nodes_id);
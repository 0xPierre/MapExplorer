#pragma once
#include "ShortestPath.h"

// Method use to test the parsing
void ParseOneTest(char* filename);

// Method use to load the latitude and longitude from a file
double **loadLatituteAndLongitudeFromFile(char *filename, int *node_count, Graph *graph);

// Method use to parse the input file
int* ParseInputFile(char* filename, char* graph_path, char* coords_path, int* node_count);

void GeoJSONNodeWriter(FILE* file, double** coords, int* nodes_id, int node_id);

void GeoJSONPathWriter(FILE* file, Path* path, int depart_id, int arrival_id, double** cords);

void GeoJSONHeaderWriter(FILE* file);

void GeoJSONFooterWriter(FILE* file);

// Method use to write the geojson file
void GeoJSONWriteAllPaths(char* filename, Path*** paths, double** coords, int node_count, int* nodes_id);

void WriteAllNodesFromGraphToFileGeoJSON(char *filename, Graph *graph);

void GeoJSONCoordsWriter(FILE *file, int coords_count, double **cords);

void GeoJSONWriteTSP(char *filename, Path *shortestPath, Path ***paths, double **coords, int node_count, int *nodes_id);

Path ***getPathMatrix(int node_count, int *nodes_id, Graph *map_graph);

Graph* getSubGraph(Path ***paths, int node_count, int *nodes_id, Graph *map_graph);
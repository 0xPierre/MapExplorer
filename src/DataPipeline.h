#pragma once
#include "SpatialMap.h"
#define FRANCE_FROM_LAT 41.354610
#define FRANCE_FROM_LON -5.307180
#define FRANCE_HEIGHT 9.80824
#define FRANCE_WIDTH 14.917099
#define FRANCE_COL 1000
#define FRANCE_ROW 1000

typedef struct Address Address;

struct Address {
    char id[15];
    char numero[10];
    char nom_voie[100];
    char slug[100];
    int code_postal;
    char nom_commune[100];
    char slug_commune[100];
    double lon;
    double lat;
};

typedef struct City City;
struct City {
    char id[15];
    char nom_commune[100];
    char slug[100];
    int code_postal;
};

Address **GetClearedCSVAddress(char *filename, int *address_count);

char *ClearString(char *str);

/*
 * Return the SpatialMap of the graph with the given parameters
 */
SpatialMap * GetIntersectionGraphSpatialMap(Graph* graph, double from_lat, double from_lon, double height, double width, int col_count, int row_count);

Address **GetClearedCSVAddressFromBigAddressesFile(int lines, char *filename, int *address_count);

void Address_print(Address *address);

/*
 * Save the addresses already cleared in a file.
 * Reduce from 22 millions to 2 millions
 */
void SaveAddressesInFile(Address **addresses, int address_count, char *filename);

/*
 * Use to load the already cleared addresses from a file
 */
Address **LoadAddressesFromFile(char *filename, int *address_count);

City **LoadCitiesFromFile(char *filename, int *city_count);

void City_print(City *city);
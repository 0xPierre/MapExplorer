#include "DataPipeline.h"
#include "Settings.h"
#include "SpatialMap.h"
#include "Graph.h"
#include "ctype.h"
#include "SearchTrie.h"
#include "TextUtils.h"
char *ClearString(char *str) {
    str = Utf8RemoveAccents(str);
    // remove special characters
    for (int i = 0; str[i]; i++) {
        if (str[i] == ' ' || isalnum(str[i])) {
            continue;
        }
        str[i] = ' ';
    }

    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }

    return str;
}

Address **GetClearedCSVAddress(char *filename, int *address_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("File not found GCCSVA\n");
        exit(1);
    }


    int lines = 0;
    fscanf(file, "%d\n", &lines);
    Address **addresses = (Address **) calloc(lines, sizeof(Address *));

    fscanf(file, "%*[^\n]\n");

    char lastId[15] = "";
    *address_count = 0;
    for (int i = 0; i < lines; i++) {
        Address* address = (Address *) calloc(1, sizeof(Address));

        char line[1000] = {0};

        fgets(line, 1000, file);

        char *token, *rest = line;

        token = strsep(&rest, ";");
        token[12] = '\0';
        strcpy(address->id, token);

        // We skip the line if the id is the same as the last one
        if (strcmp(address->id, lastId) == 0) {
            free(address);
            continue;
        }
        else {
            strcpy(lastId, address->id);
        }

        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        strcpy(address->numero, token);

        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        strcpy(address->nom_voie, token);

        char *slug = ClearString(token);
        strcpy(address->slug, slug);
        free(slug);

        token = strsep(&rest, ";");
        address->code_postal = atoi(token);

        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        strcpy(address->nom_commune, token);

        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        token = strsep(&rest, ";");
        address->lon = atof(token);

        token = strsep(&rest, ";");
        address->lat = atof(token);
////        debug("%s %s %s %s %d %s %f %f\n", address->id, address->numero, address->nom_voie, address->slug, address->code_postal, address->nom_commune, address->lon, address->lat);

        addresses[(*address_count)++] = address;
    }

    fclose(file);

    return addresses;
}

Address **GetClearedCSVAddressFromBigAddressesFile(int lines, char *filename, int *address_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("File not found GCALLA\n");
        exit(1);
    }

    Address **addresses = (Address **) calloc(lines, sizeof(Address *));

    char lastId[15] = "";
    *address_count = 0;
    for (int i = 0; i < lines; i++) {
        Address* address = (Address *) calloc(1, sizeof(Address));

        char *line = (char *) calloc(1000, sizeof(char));
        AssertNew(line);

        fgets(line, 1000, file);

        char *token, *rest = line;

        token = strsep(&rest, ",");
        if (strlen(token) < 9) {
            free(line);
            continue;
        }
        token[9] = '\0';
        strcpy(address->id, token);

        // We skip the line if the id is the same as the last one
        if (strcmp(address->id, lastId) == 0) {
            free(line);
            continue;
        } else {
            strcpy(lastId, address->id);
        }

        token = strsep(&rest, ",");
        strcpy(address->numero, token);

        token = strsep(&rest, ",");
        char str[100];
        // Only 99 characters are allowed
        strncpy(str, token, 99);
        strcpy(address->nom_voie, str);

        char *slug = ClearString(strdup(str));
        strcpy(address->slug, slug);
        free(slug);


        token = strsep(&rest, ",");
        address->code_postal = atoi(token);

        token = strsep(&rest, ",");
        strncpy(str, token, 99);
        strcpy(address->nom_commune, str);

        token = strsep(&rest, ",");
        token = strsep(&rest, ",");
        address->lat = atof(token);

        token = strsep(&rest, ",");
        address->lon = atof(token);

//        debug("%s %s %s %s %d %s %f %f\n", address->id, address->numero, address->nom_voie, address->slug, address->code_postal, address->nom_commune, address->lon, address->lat);


        addresses[(*address_count)++] = address;
        free(line);
    }

    return addresses;
}

SpatialMap * GetIntersectionGraphSpatialMap(Graph* graph, double from_lat, double from_lon, double height, double width, int col_count, int row_count) {
    SpatialMap* map = SpatialMap_Create(
            from_lat, from_lon, // Lon & lat
            height, width, // Height & Width
            col_count, row_count // Col & Row
    );


    for (int i = 0; i < graph->size; i++) {
        SpatialMap_insertNode(map, &(graph->nodes[i]));
    }

    return map;
}

void SaveAddressesInFile(Address **addresses, int address_count, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        error("File not found SAIF\n");
        exit(1);
    }

    fprintf(file, "%d\n", address_count);
    fprintf(file, "id;nom_voie;slug;code_postal;nom_commune;lon;lat\n");

    for (int i = 0; i < address_count; i++) {
        Address *address = addresses[i];
        fprintf(file, "%s;%s;%s;%d;%s;%f;%f\n", address->id, address->nom_voie, address->slug, address->code_postal, address->nom_commune, address->lon, address->lat);
    }
}

Address **LoadAddressesFromFile(char *filename, int *address_count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("File not found LAFF\n");
        assert(file);
    }

    fscanf(file, "%d\n", address_count);
    fscanf(file, "%*[^\n]\n");

    Address **addresses = (Address **) calloc(*address_count, sizeof(Address *));

    for (int i = 0; i < *address_count; i++) {
        Address* address = (Address *) calloc(1, sizeof(Address));

        char *line = (char *) calloc(1000, sizeof(char));
        AssertNew(line);

        fgets(line, 1000, file);

        char *token, *rest = line;

        token = strsep(&rest, ";");
        strcpy(address->id, token);

        token = strsep(&rest, ";");
        strcpy(address->nom_voie, token);

        token = strsep(&rest, ";");
        strcpy(address->slug, token);

        token = strsep(&rest, ";");
        address->code_postal = atoi(token);

        token = strsep(&rest, ";");
        strcpy(address->nom_commune, token);

        char *slug_commune = ClearString(strdup(token));
        strcpy(address->slug_commune, slug_commune);

        token = strsep(&rest, ";");
        address->lon = atof(token);

        token = strsep(&rest, ";");
        address->lat = atof(token);

        addresses[i] = address;

        free(line);
    }

    return addresses;
}

void Address_print(Address *address) {
    printf("Address(id:%s numero:%s nom_voie:%s slug:%s code_postal:%d nom_commune:%s Coords:[%f %f])\n",
           address->id, address->numero, address->nom_voie, address->slug, address->code_postal, address->nom_commune, address->lat, address->lon);
}

void City_print(City *city) {
    printf("City-id:%s nom_voie: %s slug:%s code_postal:%d\n", city->id, city->nom_commune, city->slug, city->code_postal);
}

City **LoadCitiesFromFile(char *filename, int *city_count) {
    debug("%s\n", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("File not found LCFF\n");
        assert(file);
    }

    fscanf(file, "%d\n", city_count);
    fscanf(file, "%*[^\n]\n");
    debug("City count: %d\n", *city_count);

    City **cities = (City **) calloc(*city_count, sizeof(City *));

    char last_id[15] = "";

    int real_city_count = 0;

    for (int i = 0; i < *city_count; i++) {
        City* city = (City *) calloc(1, sizeof(City));

        char *line = (char *) calloc(1000, sizeof(char));
        AssertNew(line);

        fgets(line, 1000, file);

        char *token, *rest = line;

        token = strsep(&rest, ",");

        if (strcmp(token, last_id) == 0) {
            free(line);
            continue;
        }
        strcpy(last_id, token);


        strcpy(city->id, token);

        token = strsep(&rest, ",");
        strcpy(city->nom_commune, token);

        char *slug = ClearString(token);
        strcpy(city->slug, slug);
        free(slug);

        token = strsep(&rest, ",");
        city->code_postal = atoi(token);

        cities[real_city_count] = city;

        real_city_count++;
        free(line);
    }

    *city_count = real_city_count;

    return cities;
}
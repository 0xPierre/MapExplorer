#pragma once

#define TEST_DIR "./data/ressources/Tests/"
#define OUTPUT_DIR "./output/"
//#define DATA_DIR "/app/share/com.potooFactory.eu/ressources/Data/" // SHOULD BE UCOMMENTED FOR GNOME BUILDER !!!
#define DATA_DIR "./data/ressources/Data/"

// Where the output file will be written
#define TS_GEOJSON_OUTPUT "./tsoutput.geojson"

// Activate UI.
// ATTENTION : UI can be compiled only from gnome-builder
// When activated, the UI will be displayed.
#define UI_ENABLED true
// If you want to use another addresses files. PLEASE ASK US and don't do it yourself.
#define UI_ADDRESS_FILE DATA_DIR "adresses-53.csv"
#define UI_GRAPH_FILE DATA_DIR "mayenne_graph.txt"
#define UI_COORDS_FILE DATA_DIR "mayenne_inter.txt"
#define UI_COMMUNE_FILE DATA_DIR "communes-departement-region.csv"

// From there, to compile go in src/ and run ./compile.sh or the gcc command
// To execute : ./a.out

/*
 * Commande Line Interface
 * It is an equivalent of the UI but in the terminal.
 * The output will be written in the output folder.
 */
// Put false to UI_ENABLED and the others
#define RUN_CLI true
// If you want to try, a file with
// If you want to use another addresses files. PLEASE ASK US and don't do it yourself.
#define CLI_ADDRESS_FILE DATA_DIR "adresses-53.csv"
#define CLI_GRAPH_FILE DATA_DIR "mayenne_graph.txt"
#define CLI_COORDS_FILE DATA_DIR "mayenne_inter.txt"

// Initialisation of the ACO using the best path found by the glouton heuristic
#define USE_GLOUTON_PHEROMONE_INIT false

// Sometimes take the worst next found by the probability of ACO
#define USE_ACO_CANCELLED_PROBABILITY false
#define ACO_CANCELLED_PROBABILITY 0.05f

// Sometimes take a random next found by the probability of ACO
#define USE_ACO_RANDOM_PROBABILITY false
#define ACO_RANDOM_PROBABILITY 0.05f

// Sometimes paths of the ACO cross themselves, this is a way to uncross them
// You can see it with "4_TSP_ACO/input4.txt" in FILENAME_SIMPLE_ACO
#define ENABLE_PATH_PLANNER true // Works in TEST_SIMPLE_ACO

/*
 * Test a simple ACO
*/
#define TEST_SIMPLE_ACO false
// Possible :
// 1. "4_TSP_ACO/input1.txt" use laval
// 2. "4_TSP_ACO/input2.txt" use mayenne
// 3. "4_TSP_ACO/input3.txt" use pays de la loire
// 4. "4_TSP_ACO/input4.txt" use mayenne
#define FILENAME_SIMPLE_ACO TEST_DIR "4_TSP_ACO/input1.txt"

#define TEST_ADDRESS_AUTOCOMPLETE false
#define TEST_ADDRESS_AUTOCOMPLETE_FILE DATA_DIR "adresses-53.csv"
#define TEST_ADDRESS_AUTOCOMPLETE_SEARCH "rue docteurs calmette"
#define TEST_ADDRESS_AUTOCOMPLETE_GRAPH DATA_DIR "mayenne_graph.txt"
#define TEST_ADDRESS_AUTOCOMPLETE_COORDS DATA_DIR "mayenne_inter.txt"

#define TEST_TSP_BRUTEFORCE false
// Possible :
// 1. "4_TSP_ACO/input1.txt" use laval 7 nodes
// 2. "4_TSP_ACO/input2.txt" use mayenne 17 nodes ATTENTION, A NE LANCER SEULEMENT SI VOUS ETES IMMORTEL
// 3. "4_TSP_ACO/input3.txt" use pays de la loire 7 nodes
// 4. "4_TSP_ACO/input4.txt" use mayenne 37 nodes ATTENTION, A NE LANCER SEULEMENT SI VOUS ETES IMMORTEL
#define TEST_TSP_BRUTEFORCE_FILE TEST_DIR "4_TSP_ACO/input1.txt"

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>

#include "Logger.h"
#include "Timer.h"
//#include "FileParser.h"
//#include "TSPAlgorithm.h"

// NOt done
#define USE_ACO_SECOND_BEST 1
#define ACO_SECOND_BEST 0.05f

#define AssertNew(ptr) { if (ptr == NULL) { assert(false); abort(); } }

#ifdef _MSC_VER
#  define INLINE inline
#else
#  define INLINE static inline
#endif

INLINE int minInt(int a, int b) { return a < b ? a : b; }
INLINE int maxInt(int a, int b) { return a > b ? a : b; }



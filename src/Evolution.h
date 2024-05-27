#include "Graph.h"

// GA parameters
#define POPULATION_SIZE 50
#define GENERATION_COUNT 50
#define MUTATION_PROBABILITY 0.3
#define MAX_ALPHA 30
#define MAX_BETA 30
#define MAX_RHO 1
#define MAX_Q 50

// ACO PARAMETERS
#define ANTS_COUNT 50
#define ITERATION_COUNT 400

typedef struct Chromosome Chromosome;
typedef struct Population Population;

typedef union Genes {
    struct {
        double alpha;
        double beta;
        double rho;
        double q;
    };
    double data[4];
} Genes;

struct Chromosome {
    Genes *genes;

    double score;
};

struct Population {
    Chromosome **chromosomes; // List of chromosomes
    double score; // The sum of score of each chromosome
    int size;
};

void Evotuion_run(Graph *graph);

Population *Evolution_createPopulation(int size);

Population *Evotuion_generatePopulation(Population *population);

void Evotuion_generateChromosome(Chromosome *chromosome);

void Evolution_fitness(Population *population, Graph *graph);

void Evolution_fitnessOrder(Population *population);

void Evolution_selectionPair(Population *population, Population *parents);

void Evolution_singleCrossOver(Population *parents);

void Evolution_mutation(Chromosome *chromosome);

void Evolution_destroyPopulation(Population *population);


INLINE double randomAlpha(void) { return ((double) rand() / (double) (RAND_MAX)) * MAX_ALPHA + 1;}
INLINE double randomBeta(void) { return (((double) rand() / (double) (RAND_MAX)) * MAX_BETA) + 1; }
INLINE double randomRho(void) { return (((double) rand() / (double) (RAND_MAX)) * MAX_RHO); }
INLINE double randomQ(void) { return (((double) rand() / (double) (RAND_MAX)) * MAX_Q); }

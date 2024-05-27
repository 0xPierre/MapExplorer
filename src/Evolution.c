#include "Settings.h"
#include "Evolution.h"
#include "TSPAlgorithm.h"

void Evotuion_run(Graph *graph) {
    // Generate the initial population
    Population *population = Evolution_createPopulation(POPULATION_SIZE);
    Evotuion_generatePopulation(population);
    assert(population);
    assert(population->chromosomes);


    for (int l = 0; l < GENERATION_COUNT; ++l) {
        Evolution_fitness(population, graph); // Compute score and ordered them by score
        // PRINT POPUTLAION
        for (int i = 0; i < population->size; ++i) {
            printf("Alpha: %f Beta: %f, rho: %f, q: %f, score: %f\n",
                   population->chromosomes[i]->genes->alpha,
                   population->chromosomes[i]->genes->beta,
                   population->chromosomes[i]->genes->rho,
                   population->chromosomes[i]->genes->q,
                   population->chromosomes[i]->score);
        }

        // Extract the two best solutions
        Chromosome **nextGeneration = (Chromosome **) calloc(population->size, sizeof(Chromosome *));
        nextGeneration[0] = population->chromosomes[0];
        nextGeneration[1] = population->chromosomes[1];
        int nextGenerationIndex = 2; // Index to know the position of insertion.

        printf("NEXT GENERATION %d!\n", l);

        for (int i = 0; i < population->size; ++i) {
            if (nextGeneration[i] != NULL)
                printf("%f\n", nextGeneration[i]->score);
            else printf("NULL\n");
        }

        //PRINT BEST SOLUTION
        printf("2 BEST SOLUTIONS \n");
        for (int i = 0; i < 2; ++i) {
            printf("Alpha: %f Beta: %f, rho: %f, q: %f, score: %f\n",
                   nextGeneration[i]->genes->alpha,
                   nextGeneration[i]->genes->beta,
                   nextGeneration[i]->genes->rho,
                   nextGeneration[i]->genes->q,
                   nextGeneration[i]->score);
        }
        printf("\n");

        for (int i = 0; i < population->size / 2 - 1; ++i) {
            Population *parents = Evolution_createPopulation(2);
            Evolution_selectionPair(population, parents);
            assert(parents->chromosomes[0] && parents->chromosomes[1]);

            printf("SELECTED PARENTS : \n");
            for (int j = 0; j < 2; ++j) {// DEBUG ONLY
                printf("Parent%d", j);
                printf(": %f %f %f %f score: %f\n",
                       parents->chromosomes[j]->genes->data[0],
                       parents->chromosomes[j]->genes->data[1],
                       parents->chromosomes[j]->genes->data[2],
                       parents->chromosomes[j]->genes->data[3],
                       parents->chromosomes[j]->score);
            }

            printf("CROSS OVER : ");

            Evolution_singleCrossOver(parents);

            for (int j = 0; j < 2; ++j) {// DEBUG ONLY
                printf("Parent%d", j);
                printf(": %f %f %f %f score: %f\n",
                       parents->chromosomes[j]->genes->data[0],
                       parents->chromosomes[j]->genes->data[1],
                       parents->chromosomes[j]->genes->data[2],
                       parents->chromosomes[j]->genes->data[3],
                       parents->chromosomes[j]->score);
            }
            printf("\n");

            Evolution_mutation(parents->chromosomes[0]);
            Evolution_mutation(parents->chromosomes[1]);
            nextGeneration[nextGenerationIndex++] = parents->chromosomes[0];
            nextGeneration[nextGenerationIndex++] = parents->chromosomes[1];
        }

        population->chromosomes = nextGeneration;
    }
    Evolution_fitness(population, graph); // Compute score and ordered them by score

    printf("BEST %f", population->chromosomes[0]->score);
    printf("Alpha: %f Beta: %f, rho: %f, q: %f, score: %f\n",
           population->chromosomes[0]->genes->alpha,
           population->chromosomes[0]->genes->beta,
           population->chromosomes[0]->genes->rho,
           population->chromosomes[0]->genes->q,
           population->chromosomes[0]->score);

    Evolution_destroyPopulation(population);
}

Population *Evolution_createPopulation(int size) {
    Population *population = calloc(1, sizeof(Population));
    assert(population);
    population->chromosomes = (Chromosome **) calloc(size, sizeof(Chromosome *));
    population->size = size;

    return population;
}

Population *Evotuion_generatePopulation(Population *population) {
    assert(population);

    for (int i = 0; i < population->size; ++i) {
        population->chromosomes[i] = (Chromosome *) calloc(1, sizeof(Chromosome));
        population->chromosomes[i]->genes = (Genes *) calloc(1, sizeof(Genes));
        Evotuion_generateChromosome(population->chromosomes[i]);
    }

    return population;
}

void Evotuion_generateChromosome(Chromosome *chromosome) {
    assert(chromosome);
    // Assign random values to the chromosome
    chromosome->genes->alpha = randomAlpha(); // Alpha
    chromosome->genes->beta = randomBeta(); // Beta
    chromosome->genes->rho = randomRho(); // rho
    chromosome->genes->q = randomQ(); // q
}

void Evolution_fitness(Population *population, Graph *graph) {
    assert(population);
    assert(graph);
    double totalScore = 0;
    for (int i = 0; i < population->size; ++i) {
        double alpha = population->chromosomes[i]->genes->alpha;
        double beta = population->chromosomes[i]->genes->beta;
        double rho = population->chromosomes[i]->genes->rho;
        double q = population->chromosomes[i]->genes->q;
        debug("%f %f %f %f\n", alpha, beta, rho, q);
//        double score = Graph_tspFromACO(graph, 0, ITERATION_COUNT, ANTS_COUNT, alpha, beta, rho, q)->distance;
        Path *path = Graph_tspFromACO(graph, 0, ITERATION_COUNT, ANTS_COUNT, alpha, beta, rho, q);
        debug("\n");
        double score = path->distance;
//        ListInt_print(path->list);

        totalScore += score;
        debug("%d score: %f\n", i, score);
        population->chromosomes[i]->score = (int) score == 0 ? INFINITY : score;
    }
    population->score = totalScore;
    Evolution_fitnessOrder(population);
}

void Evolution_fitnessOrder(Population *population) {
    assert(population);
    for (int i = 0; i < population->size - 1; i++)
        for (int j = 0; j < population->size - i - 1; j++)
            if (population->chromosomes[j]->score > population->chromosomes[j + 1]->score) {
                Chromosome *temp = population->chromosomes[j];
                population->chromosomes[j] = population->chromosomes[j + 1];
                population->chromosomes[j + 1] = temp;
            }
}

void Evolution_selectionPair(Population *population, Population *parents) {
    assert(population);
    assert(parents);

    for (int i = 0; i < 2; ++i) { // Populate parents
        double totalScore = population->score;
        double randomScore = (((double) rand() / (double) (RAND_MAX)) * totalScore);
        double sumScore = 0;
        for (int j = 0; j < population->size; ++j) {
            sumScore += population->chromosomes[j]->score;
            if (sumScore >= randomScore) {
                parents->chromosomes[i] = population->chromosomes[j];
                if (i == 1) return;
                break;
            }
        }
    }
}

void Evolution_singleCrossOver(Population *parents) {
    int crossIndex = rand() % 4;
//    printf("%d \n", crossIndex);
    Genes tmp = *parents->chromosomes[0]->genes;
    for (int i = 0; i < crossIndex; ++i) {
        parents->chromosomes[0]->genes->data[i] = parents->chromosomes[1]->genes->data[i];
        parents->chromosomes[1]->genes->data[i] = tmp.data[i];
    }
}

void Evolution_mutation(Chromosome * chromosome) {
    for (int i = 0; i < 4; ++i) {
        double random = (((double) rand() / (double) (RAND_MAX)) * 1);
        if (random < MUTATION_PROBABILITY) {
            switch (i) {
                case 0:
                    chromosome->genes->data[i] = randomAlpha();
                    break;
                case 1:
                    chromosome->genes->data[i] = randomBeta();
                    break;
                case 2:
                    chromosome->genes->data[i] = randomRho();
                    break;
                case 3:
                    chromosome->genes->data[i] = randomQ();
                    break;
                default:
                    break;
            }
        }
    }
}

void Evolution_destroyPopulation(Population *population) {
    if (population == NULL) return;
    if (population->chromosomes == NULL) return;
    for (int i = 0; i < population->size; ++i) {
        if (population->chromosomes[i] != NULL) {
            free(population->chromosomes[i]->genes);
            free(population->chromosomes[i]);
        }
    }
    free(population->chromosomes);
    free(population);
}
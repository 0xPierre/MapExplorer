#pragma once

#include "Settings.h"
#include "Graph.h"
#include "ListInt.h"
#include "ShortestPath.h"


/// @brief Calcule une toun�e dans un graphe en se basant sur un algorithme glouton.
/// @param graph le graphe des distances. Il doit �tre complet.
/// @param station l'identifiant du sommet de d�part et d'arriv�e de la tourn�e.
/// @return La tourn�e.
Path* Graph_tspFromHeuristic(Graph* graph, int station);

/// @brief Calcule une toun�e dans un graphe en se basant sur l'algorithme d'optimisation ACO.
/// @param graph le graphe des distances. Il doit �tre complet.
/// @param station l'identifiant du sommet de d�part et d'arriv�e de la tourn�e.
/// @param iterationCount le nombre d'it�ration de l'algorithme.
/// @param antCount le nombre de fourmis par it�ration.
/// @param alpha un r�el positif qui contr�le l'influence des ph�romones sur le choix des fourmis.
/// @param beta un r�el sup�rieur � 1.f qui contr�le l'influence de la distance r�elle sur le choix des fourmis.
/// @param rho un r�el entre 0.f et 1.f qui contr�le l'�vaporation des pheromones
/// @param q un r�el positif qui cont�le la quantit� de ph�romones d�pos�es par les fourmis.
/// @return La tourn�e.
Path* Graph_tspFromACO(
    Graph* graph, int station, int iterationCount, int antCount,
    double alpha, double beta, double rho, double q);

/// @brief Depuis un sommet, calcule les probabilit�s de passer aux sommets suivants.
/// @param graph le graphe des distances. Il doit �tre complet.
/// @param pheromones le graphe des ph�romones.
/// @param station l'identifiant du sommet actuel de la fourmi.
/// @param explored tableau indiquant les sommets d�j� parcourus par la fourmi.
/// @param alpha un r�el positif qui contr�le l'influence des ph�romones sur le choix des fourmis.
/// @param beta un r�el sup�rieur � 1.f qui contr�le l'influence de la distance r�elle sur le choix des fourmis.
/// @return Le tableau alou� dynamiquement des probabilit�s de la fourmi.
double* Graph_acoGetProbabilities(
    Graph* graph, Graph* pheromones, int station,
    bool* explored, double alpha, double beta);

/// @brief Construit la tourn�e d'une fourmi.
/// @param graph le graphe des distances. Il doit �tre complet.
/// @param pheromones le graphe des ph�romones.
/// @param station l'identifiant du sommet de d�part et d'arriv�e de la tourn�e.
/// @param alpha un r�el positif qui contr�le l'influence des ph�romones sur le choix de la fourmi.
/// @param beta un r�el sup�rieur � 1.f qui contr�le l'influence de la distance r�elle sur le choix de la fourmi.
/// @return La tourn�e de la fourmi.
Path* Graph_acoConstructPath(Graph* graph, Graph* pheromones, int station, double alpha, double beta);

/// @brief Ajoute des ph�romones en prenant en compte la tourn�e d'une fourmi et sa qualit�.
/// @param pheromones le graphe des ph�romones.
/// @param path la tourn�e de la fourmi.
/// @param q un r�el positif qui cont�le la quantit� de ph�romones d�pos�es par la fourmi.
void Graph_acoPheromoneUpdatePath(Graph* pheromones, Path* path, double q);

/// @brief Applique l'�vaporation des ph�romones � la fin d'une it�ration de l'algorithme ACO.
/// @param pheromones le graphe des ph�romones.
/// @param rho le coefficient d'�vaporation des ph�romones (entre 0.f et 1.f).
void Graph_acoPheromoneGlobalUpdate(Graph* pheromones, double rho);

void Graph_FindTheBestPathNFactorielHelper(Graph* graph, int start, int current, ListInt* currentPath, bool* visited, double currentDistance, Path* bestPath);

Path* Graph_FindTheBestPathNFactoriel(Graph* graph, int station);

Path* twoOptSwap(Path *path, int i, int k);

Path *twoOpt(Graph *graph, Path *path);

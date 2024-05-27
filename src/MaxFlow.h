//#pragma once
//
//#include "Settings.h"
//#include "Graph.h"
//#include "ListInt.h"
//
///// @brief Calcule le flot maximal dans un réseau de flot.
///// @param capacities le graphe des capacités du réseau de flot.
///// @param s l'identifiant de la source.
///// @param t l'identifiant du puits.
///// @return Le graphe de flot.
//Graph *Graph_maxFlow(Graph *capacities, int s, int t);
//
////--------------------------------------------------------------------------------------------------
//// Fonctions auxiliaires
//
///// @brief Crée le graphe résiduel d'un réseau de flot (vide).
///// @param capacities le graphe des capacités du réseau de flot.
///// @return Le graphe résiduel d'un réseau de flot vide spécifié par ses capacités.
//Graph *Graph_initResidual(Graph *capacities);
//
///// @brief Recherche une chaîne améliorante dans un graphe résiduel.
///// @param residual le graphe résiduel.
///// @param s l'identifiant de la source.
///// @param t l'identifiant du puits.
///// @return Une liste représentant une chaîne améliorante en cas d'existence,
///// une liste vide sinon.
//ListInt *Graph_findImprovingPath(Graph *residual, int s, int t);
//
///// @brief Calcule la valeur d'augmentation du flot associée à une chaîne augmentante.
///// @param residual le graphe résiduel.
///// @param path la chaîne améliorante. Elle doit être non vide.
///// @return La valeur d'augmentation du flot.
//double Graph_getImprovingPathScore(Graph *residual, ListInt *path);
//
///// @brief Met à jour les poids des arcs d'un graphe résiduel avec une chaîne améliorante.
///// @param residual le graphe résiduel.
///// @param path la chaîne améliorante. Elle doit être non vide.
///// @param score la valeur d'augmentation du flot associée à la chaîne.
//void Graph_updateResidual(Graph *residual, ListInt *path, double score);
//
///// @brief Crée le graphe de flot associé à un graphe résiduel.
///// @param residual le graphe résiduel.
///// @param capacities le graphe des capacités du réseau de flot
///// @return Le graphe de flot.
//Graph *Graph_createFlowFromResidual(Graph *residual, Graph *capacities);
//
//
//void addResidualEdge(Graph *residual, int u, int v, double capacity);
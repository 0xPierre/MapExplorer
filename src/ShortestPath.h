#pragma once

#include "Settings.h"
#include "Graph.h"
#include "ListInt.h"

/// @brief Structure représentant un chemin dans un graphe.
typedef struct Path
{
    /// @brief Liste des sommets du chemin.
    /// Le premier élément de la liste est le sommet et départ
    /// et le dernier élément le sommet d'arrivée.
    ListInt *list;

    /// @brief Longueur du chemin.
    /// Autrement dit la somme des poids des arcs qui composent le chemin.
    double distance;
} Path;

/// @brief Crée un nouveau chemin.
/// @param start le sommet de départ du chemin.
/// @return Le chemin créé.
Path *Path_create(int start);

/// @brief Détruit un chemin créé avec Path_create().
/// @param path le chemin à détruire.
void Path_destroy(Path *path);

/// @brief Affiche un chemin.
/// @param path le chemin.
void Path_print(Path *path);

/// @brief Renvoie un plus court chemin entre deux sommets d'un graphe.
/// Si aucun chemin n'existe, renvoie NULL.
/// Cette fonction suit l'algorithme de Dijkstra.
/// Elle a une complexité en O(n^2) où n désigne le nombre de noeuds du graphe.
///
/// @param graph le graphe.
/// @param start l'identifiant du sommet de départ.
/// @param end l'identifiant du sommet d'arrivée.
/// @return Un plus court chemin en cas d'existance, NULL sinon.
Path *Graph_shortestPath(Graph *graph, int start, int end);

/// @brief Effectue l'algorithme de Dijkstra.
/// Si end >= 0, cette fonction calcule un plus court chemin entre les noeuds
/// start et end.
/// Le chemin doit cependant être reconstruit à partir des tableaux
/// predecessors et distances en utilisant la fonction Graph_dijkstraGetPath().
/// Si end < 0, l'algorithme calcule un plus court chemin pour chaque noeud
/// accessible depuis start.
///
/// @param graph le graphe.
/// @param start l'identifiant du sommet de départ.
/// @param end l'identifiant du sommet d'arrivée ou un entier < 0 pour calculer
///     tous les chemins.
/// @param predecessors tableau péalablement alloué dont la taille est égale au
///     nombre de noeuds du graphe. Après l'appel à la fonction, il contient le
///     prédécesseur de chaque noeud.
/// @param distances tableau péalablement alloué dont la taille est égale au
///     nombre de noeuds du graphe. Après l'appel à la fonction, il contient
///     pour chaque noeud sa distance avec le noeud start.
void Graph_dijkstra(Graph *graph, int start, int end, int *predecessors, double *distances);

/// @brief Reconstruit un chemin à partir de la sortie de l'algorithme de
/// Dijkstra.
///
/// @param predecessors tableau des prédecesseurs.
/// @param distances tableau des distances.
/// @param end noeud d'arrivée.
/// @return Le plus court chemin arrivant au noeud end s'il existe, NULL sinon.
Path *Graph_dijkstraGetPath(int *predecessors, double *distances, int end);
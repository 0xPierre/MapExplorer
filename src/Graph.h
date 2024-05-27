#pragma once

#include "Settings.h"

typedef struct Graph Graph;
typedef struct GraphNode GraphNode;
typedef struct ArcList ArcList;

/// @brief Structure représentant un graphe orienté.
struct Graph
{
    /// @brief Tableau contenant les noeuds du graphe.
    GraphNode* nodes;

    /// @brief Nombre de noeuds du graphe.
    int size;
};

/// @brief Structure représentant un noeud d'un graphe.
struct GraphNode
{
    /// @brief Degré sortant du noeud.
    int arcCount;

    /// @brief Liste des arcs sortants du noeud.
    ArcList* arcList;

    int id;
    double lon;
    double lat;
};

/// @brief Structure représentant une liste simplement chaînée des arcs sortants d'un noeud.
struct ArcList
{
    /// @brief Pointeur vers l'élément suivant de la liste.
    /// Vaut NULL s'il s'agit du dernier élément.
    ArcList* next;

    int target;

    double weight;
};

/// @brief Crée un nouveau graphe.
/// Le nombre de noeuds doit être défini au moment de la création et ne peut
/// plus être modifié par la suite.
/// @param size Le nombre de noeuds du graphe.
/// @return Le graphe créé.
Graph *Graph_create(int size);

/// @brief Détruit un graphe créé avec Graph_create().
/// @param graph le graphe.
void Graph_destroy(Graph *graph);

/// @brief Crée un nouveau graphe à partir du graphe stocké dans le fichier filename.
/// @param filenamne Chemin vers le fichier texte contenant le graphe
/// @return Le graphe créé.
Graph *Graph_load(char *filename);

/// @brief Sauvegarde le graphe dans un fichier texte.
/// @param graph Le graphe
/// @param filename Chemin vers le fichier texte où sauvegarder le graphe
/// @param arc_count Nombre d'arcs à sauvegarder
void Graph_save(Graph *graph, char *filename, int arc_count);

/// @brief Affiche le graphe.
/// @param graph Le graphe 
void Graph_print(Graph *graph);

/// @brief Renvoie le nombre de noeuds d'un graphe.
/// @param graph le graphe.
/// @return Le nombre de noeuds du graphe.
INLINE int Graph_size(Graph *graph)
{
    assert(graph);
    return graph->size;
}

/// @brief Retourne le nombre de voisins du noeud u
/// @param graphe Le graphe
/// @param u L'id du noeud
/// @return Le degré sortant de u.
INLINE int Graph_getArcCount(Graph *graph, int u)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    return graph->nodes[u].arcCount;
}

/// @brief Retourne la liste d'arcs reliant u à ses voisins
/// @param graphe Le graphe
/// @param u L'id du noeud
/// @return La liste d'arcs reliant u à ses voisins
INLINE ArcList *Graph_getArcList(Graph *graph, int u)
{
    assert(graph);
    assert(0 <= u && u < graph->size);
    return graph->nodes[u].arcList;
}

/// @brief Retourne un pointeur sur l'arc entre les sommets u et v,
/// NULL s'il n'existe pas. Ne supprime pas l'arc.
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
/// @return un pointeur sur la donnée de l'arc.
double *Graph_getArc(Graph *graph, int u, int v);

/// @brief Ajoute un nouvel arc entre les sommets u et v
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
/// @param weight le poids de l'arc
void Graph_setArc(Graph *graph, int u, int v, double weight);

/// @brief Supprime l'arc entre les sommets u et v s'il existe
/// @param graphe Le graphe
/// @param u L'id du noeud source
/// @param v L'id du noeud destination
void Graph_removeArc(Graph *graph, int u, int v);

/// @brief Sous-fonction de Graph_dfsPrint() pour les appels r�cursifs.
/// @param graph le graphe.
/// @param currID l'identifiant du noeud courant.
/// @param explored tableau de bool�ens indiquant pour chaque identifiant de
/// noeud s'il a �t� marqu� comme ateint.
void Graph_dfsPrintRec(Graph *graph, int currID, bool *explored);

/// @brief Fonction principale pour le parcours en profondeur
/// @param graph le graphe.
/// @param start sommet de d�part d'exploration
void Graph_dfsPrint(Graph *graph, int start);

/// @brief Fonction principale pour le parcours en largeur
/// @param graph le graphe.
/// @param start sommet de d�part d'exploration
void Graph_bfsPrint(Graph *graph, int start);

int *Graph_dSatur(Graph *graph, int *colorCount);

int Graph_maxDegree(Graph *graph);

bool Graph_isColoringValid(Graph* graph, int* colors);

/// @brief Sauvegarde un graph dans un fichier pour le load ultérieurement
/// @param grap le Graph
void Graph_write_to_file(const Graph* graph);
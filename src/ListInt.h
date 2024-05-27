#pragma once

#include "Settings.h"

/// @brief Structure représentant un noeud d'une liste d'entiers.
typedef struct ListIntNode ListIntNode;
struct ListIntNode
{
    /// @brief Pointeur vers le noeud précédent.
    ListIntNode *prev;

    /// @brief Pointeur vers le noeud suivant.
    ListIntNode *next;

    /// @brief Valeur du noeud.
    int value;
};

/// @brief Structure représentant une liste doublement chaînée d'entiers.
typedef struct ListInt
{
    /// @brief Sentinelle.
    ListIntNode sentinel;

    /// @brief Le nombre de noeuds de la liste.
    int nodeCount;
} ListInt;

/// @brief Crée une liste vide.
/// Cette liste peut notamment être utilisée comme une pile ou comme une file.
/// @return La liste créée.
ListInt *ListInt_create(void);

/// @brief Détruit une liste d'entiers.
/// @param list la liste.
void ListInt_destroy(ListInt *list);

/// @brief Renvoie la taille d'une liste d'entiers.
/// @param list la liste.
/// @return La taille de la liste.
int ListInt_size(ListInt *list);

/// @brief Indique si une liste d'entiers est vide.
/// @param list la liste.
/// @return true si la liste est vide, false sinon.
bool ListInt_isEmpty(ListInt *list);

/// @brief Affiche une liste d'entiers.
/// @param list la liste.
void ListInt_print(ListInt *list);

/// @brief Renvoie le premier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier entier de la liste.
int ListInt_getFirst(ListInt *list);

/// @brief Renvoie le dernier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier dernier de la liste.
int ListInt_getLast(ListInt *list);

/// @brief Insert un entier au début d'une liste.
/// @param list la liste.
/// @param value l'entier à insérer.
void ListInt_insertFirst(ListInt *list, int value);

/// @brief Insert un entier à la fin d'une liste.
/// @param list la liste.
/// @param value l'entier à insérer.
void ListInt_insertLast(ListInt *list, int value);

/// @brief Insert un noeud dans une liste d'entiers après un noeud de référence.
/// @param list la liste.
/// @param ref le noeud de référence.
/// @param node le noeud à insérer.
void ListInt_insertNodeAfter(ListInt *list, ListIntNode *ref, ListIntNode *node);

/// @brief Supprime et renvoie le premier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le premier entier de la liste.
int ListInt_popFirst(ListInt *list);

/// @brief Supprime et renvoie le dernier entier d'une liste.
/// L'utilisateur doit au préalable vérifier que le liste est non vide.
/// @param list la liste.
/// @return Le dernier entier de la liste.
int ListInt_popLast(ListInt *list);

/// @brief Retire un noeud d'une liste d'entiers.
/// Le noeud n'est pas détruit, il est juste extrait de la liste.
/// @param list la liste.
/// @param node le noeud à retirer.
void ListInt_popNode(ListInt *list, ListIntNode *node);

/// @brief Enfile un entier dans une file.
/// @param list la liste utilisée comme une file.
/// @param value l'entier.
INLINE void ListInt_enqueue(ListInt *list, int value)
{
    ListInt_insertLast(list, value);
}

/// @brief Défile un entier dans une file.
/// @param list la liste utilisée comme une file.
/// @param value l'entier.
INLINE int ListInt_dequeue(ListInt *list)
{
    return ListInt_popFirst(list);
}

/// @brief Empile un entier dans une pile.
/// @param list la liste utilisée comme une pile.
/// @param value l'entier.
INLINE void ListInt_push(ListInt *list, int value)
{
    ListInt_insertLast(list, value);
}

/// @brief Dépile un entier dans une pile.
/// @param list la liste utilisée comme une pile.
/// @param value l'entier.
INLINE int ListInt_pop(ListInt *list)
{
    return ListInt_popLast(list);
}

/// @brief Teste la présence d'un élément dans une liste.
/// @param list la liste.
/// @param element l'élément recherché.
/// @return true si l'élément est présent dans la liste,
/// false sinon.
int ListInt_isIn(ListInt *list, int element);

/// @brief Copie une liste avec tous ses noeuds.
/// @param list la liste à copier.
/// @return Une copie profonde de la liste.
ListInt *ListInt_copy(ListInt *list);

/// @brief Concatène deux listes.
/// Les noeuds de la deuxième liste sont ajoutés en fin de la première liste.
/// La seconde liste devient vide après l'appel à cette fonction.
/// Cette méthode s'exécute en temps constant.
/// @param list1 la première liste.
/// @param list2 la seconde liste.
void ListInt_concatenate(ListInt *list1, ListInt *list2);

//--------------------------------------------------------------------------------------------------
// Itérateur sur une liste d'entiers

/// @brief Itérateur sur une liste d'entiers.
typedef struct ListIntIter
{
    /// @brief Pointeur vers la sentinelle.
    ListIntNode *sentinel;

    /// @brief Pointeur vers le noeud courant.
    ListIntNode *current;
} ListIntIter;

/// @brief Crée un itérateur sur une liste d'entiers.
/// Si la liste n'est pas vide, l'itérateur pointe vers le premier élément.
/// @param list la liste à parcourir.
/// @return Un itérateur sur la liste d'entiers.
ListIntIter *ListIntIter_create(ListInt *list);

/// @brief Détruit un itérateur sur une liste d'entiers.
/// @param iter l'itérateur.
void ListIntIter_destroy(ListIntIter *iter);

/// @brief Renvoie l'élément courant de l'itérateur.
/// L'utilisateur doit vérifier que l'itérateur est valide avant d'appeler cette méthode.
/// @param iter l'itérateur.
/// @return L'élément courant de l'itérateur.
int ListIntIter_get(ListIntIter *iter);

/// @brief Fait avancer l'itérateur sur vers l'élément suivant.
/// L'utilisateur doit vérifier que l'itérateur est valide avant d'appeler cette méthode.
/// @param iter l'itérateur.
void ListIntIter_next(ListIntIter *iter);

/// @brief Indique si un itérateur pointe sur un élément valide.
/// @param iter l'itérateur.
/// @return true si l'itérateur pointe sur un élément valide, false sinon.
bool ListIntIter_isValid(ListIntIter *iter);

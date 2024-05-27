#include "Settings.h"

typedef struct HeapNode HeapNode;
typedef struct BinaryHeap BinaryHeap;

struct HeapNode {
    int vertex;
    double distance;
};

struct BinaryHeap {
    HeapNode *array;
    int capacity;
    int size;
};

/// Create a BinaryHeap
/// \param capacity
/// \return binary heap structure
BinaryHeap *BinaryHeap_create(int capacity);

/// Add nod to a binary heap
/// \param heap
/// \param vertex
/// \param distance
void BinaryHeap_addNode(BinaryHeap *heap, int vertex, double distance);

/// Check if a binary heap is empty.
/// \param heap
/// \return true if empty or false if not
bool BinaryHeap_isEmpty(BinaryHeap *heap);

/// Destroy a binary heap.
/// \param heap
void BinaryHeap_destroy(BinaryHeap *heap);

/// Extract the min of the binary heap.
/// \param heap
/// \return
int BinaryHeap_extractMin(BinaryHeap *heap);

/// Swap to element in the binary heap.
/// \param heap
/// \param u node
/// \param v node
void BinaryHeap_swap(BinaryHeap *heap, int u, int v);

/// Get parent
/// \param i get the daddy from the current index
/// \return The daddy index
INLINE int BinaryHeap_getParent(int i) { return (int) floorf((double) (i - 1) / 2); }

/// Get child left
/// \param i get the child from the current index
/// \return the child left index
INLINE int BinaryHeap_getChildLeft(int i) { return 2 * i + 1; }


/// Get child right
/// \param i get the child from the current index
/// \return the child right index
INLINE int BinaryHeap_getChildRight(int i) { return 2 * i + 2; }

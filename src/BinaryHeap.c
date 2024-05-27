#include "BinaryHeap.h"
#include "Settings.h"

BinaryHeap *BinaryHeap_create(int capacity) {
    BinaryHeap *heap = (BinaryHeap *) calloc(1, sizeof(BinaryHeap));
    heap->capacity = capacity;

    heap->size = 0;
    heap->array = (HeapNode *) calloc(capacity, sizeof(HeapNode));

    return heap;
}

bool BinaryHeap_isEmpty(BinaryHeap *heap) {
    if (heap == NULL) return true;
    return heap->size <= 0;
}

void BinaryHeap_addNode(BinaryHeap *heap, int vertex, double distance) {
    if (heap->size >= heap->capacity) {
        error("Heap is full. Cannot add more nodes.\n");
        return;
    }

    int i = heap->size; // Node insert index.

    heap->array[i].distance = distance;
    heap->array[i].vertex = vertex;

    heap->size++;

    int parent = BinaryHeap_getParent(i);
    while (i > 0 && heap->array[parent].distance > heap->array[i].distance) { // pull up node
        BinaryHeap_swap(heap, i, parent);

        i = parent;
        parent = BinaryHeap_getParent(i);
    }
}

void BinaryHeap_swap(BinaryHeap *heap, int u, int v) {
    if (BinaryHeap_isEmpty(heap) || u >= heap->size || v >= heap->size) {
        error("Error while swapping elements");
        return;
    }
    HeapNode temp = heap->array[u];
    heap->array[u] = heap->array[v];
    heap->array[v] = temp;
}

int BinaryHeap_extractMin(BinaryHeap *heap) {
    if (BinaryHeap_isEmpty(heap)) return -1;

    HeapNode min_node = heap->array[0];
    int min_vertex = min_node.vertex;

    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    int i = 0;
    while (true) {
        int left_child = BinaryHeap_getChildLeft(i);
        int right_child = BinaryHeap_getChildRight(i);
        int smallest = i;

        if (left_child < heap->size && heap->array[left_child].distance < heap->array[smallest].distance)
            smallest = left_child;
        if (right_child < heap->size && heap->array[right_child].distance < heap->array[smallest].distance)
            smallest = right_child;

        if (smallest != i) { // Min has been found
            BinaryHeap_swap(heap, i, smallest);
            i = smallest;
        } else break;
    }

    return min_vertex;
}

void BinaryHeap_destroy(BinaryHeap *heap) {
    free(heap->array);
    free(heap);
}



#include "Settings.h"
#include "SearchTrie.h"

#define NUM_CHILDREN 37
#define INITIAL_KEY_CAPACITY 100

SearchTrieNode *SearchTrieNode_Create(void) {
    SearchTrieNode *node = (SearchTrieNode *) calloc(1, sizeof(SearchTrieNode));
    // Not using AssertNew because clion is de la merde
    if (!node) {
        error("Failed to allocate memory for SearchTrieNode\n");
        exit(EXIT_FAILURE);
    }
    node->address = (Address **) calloc(INITIAL_KEY_CAPACITY, sizeof(Address *));
    if (!node->address) {
        perror("Failed to allocate memory for address array");
        free(node);
        exit(EXIT_FAILURE);
    }
    node->cities = (City **) calloc(INITIAL_KEY_CAPACITY, sizeof(City *));
    node->childNodes = (SearchTrieNode **) calloc(NUM_CHILDREN, sizeof(SearchTrieNode *));
    if (!node->childNodes) {
        error("Failed to allocate memory for childNodes array");
        free(node->address);
        free(node);
        exit(EXIT_FAILURE);
    }

    node->address_count = 0;
    node->deep = 0;
    for (int i = 0; i < NUM_CHILDREN; i++) {
        node->childNodes[i] = NULL;
    }

    return node;
}

SearchTrie *SearchTrie_Create(void) {
    SearchTrie *trie = (SearchTrie *) calloc(1, sizeof(SearchTrie));
    if (!trie) {
        perror("Failed to allocate memory for SearchTrie");
        exit(EXIT_FAILURE);
    }
    trie->root = SearchTrieNode_Create();

    return trie;
}

void SearchTrieNode_Destroy(SearchTrieNode *node) {
    if (!node) return;

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (node->childNodes[i] != NULL) {
            SearchTrieNode_Destroy(node->childNodes[i]);
        }
    }

    free(node->address);
    free(node->childNodes);
    free(node->cities);
    free(node);
}

void SearchTrie_Destroy(SearchTrie *trie) {
    if (!trie) return;

    SearchTrieNode_Destroy(trie->root);
    free(trie);
}

int GetSearchTrieIndex(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0' + 26;
    } else if (c == ' ') {
        return 36;
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a';
    } else {
        return -1;
    }
}

void SearchTrie_InsertAddress(SearchTrie *tree, Address *address) {
    if (!tree || !address) {
        error("Invalid parameters for SearchTrie_InsertAddress\n");
        return;
    }

    SearchTrieNode *current = tree->root;
    char *slug = address->slug;

    int deep = 0;

    for (int i = 0; i < strlen(slug); i++) {
        int index = GetSearchTrieIndex(slug[i]);
        if (index == -1) {
            return;
        }
        if (current->childNodes[index] == NULL) {
            current->childNodes[index] = SearchTrieNode_Create();
            current->childNodes[index]->deep = deep;
            current->childNodes[index]->c = slug[i];
        }
        current = current->childNodes[index];
        deep++;
    }

    if (current->address_count % INITIAL_KEY_CAPACITY == 0) {
        Address **new_address_array = (Address **) realloc(current->address,
                                    (current->address_count + INITIAL_KEY_CAPACITY) * sizeof(Address *));
        if (!new_address_array) {
            error("Failed to reallocate memory for address array");
            exit(EXIT_FAILURE);
        }
        current->address = new_address_array;
    }

    current->address[current->address_count++] = address;
}

void SearchTrie_InsertCity(SearchTrie *tree, City *city) {
    if (!tree || !city) {
        error("Invalid parameters for SearchTrie_InsertCity\n");
        return;
    }

    SearchTrieNode *current = tree->root;
    char *slug = city->slug;

     for (int i = 0; i < strlen(slug); i++) {
        int index = GetSearchTrieIndex(slug[i]);
        if (index == -1) {
            return;
        }
        if (current->childNodes[index] == NULL) {
            current->childNodes[index] = SearchTrieNode_Create();
        }
        current = current->childNodes[index];
    }

    if (current->city_count % INITIAL_KEY_CAPACITY == 0) {
        City **new_city_array = (City **) realloc(current->cities,
                                    (current->city_count + INITIAL_KEY_CAPACITY) * sizeof(City *));
        if (!new_city_array) {
            error("Failed to reallocate memory for city array");
            exit(EXIT_FAILURE);
        }
        current->cities = new_city_array;
    }

    current->cities[current->city_count++] = city;
}


Address **SearchTree_AutoCompleteAddressSearch(SearchTrie *tree, char *query, int *address_count_found, int max_autocomplete_results, int deep_limit) {
    if (!tree || !query || !address_count_found) {
        error("Invalid parameters for SearchTrie_ExactAddressSearch\n");
        return NULL;
    }

    SearchTrieNode *current = tree->root;

    for (int i = 0; i < strlen(query); i++) {
        int index = GetSearchTrieIndex(query[i]);
        if (index == -1) {
            continue;
        }

        if (current->childNodes[index] == NULL) {
            *address_count_found = 0;
            return NULL;
        }
        current = current->childNodes[index];
    }

    // when the number of addresses found is greater than the max_autocomplete_results, we return the addresses
    if (current->address_count > max_autocomplete_results) {
        *address_count_found = current->address_count;
        return current->address;
    }

    Address **addresses_found = (Address **) calloc(max_autocomplete_results, sizeof(Address *));
    if (!addresses_found) {
        error("Failed to allocate memory for addresses array\n");
        exit(EXIT_FAILURE);
    }
    *address_count_found = current->address_count > max_autocomplete_results ? max_autocomplete_results : current->address_count;
    for (int i = 0; i < *address_count_found; i++) {
        addresses_found[i] = current->address[i];
    }

    // We will continue to search for addresses until we reach the deep limit of 4 characters or we reach the max_autocomplete_results
    FifoSearchTrieList *list = FifoSearchTrieList_Create();
    FifoSearchTrieList_Push(list, current);
    deep_limit += current->deep;

    while (true) {
        current = FifoSearchTrieList_PopFirst(list);
        if (current == NULL) {
            break;
        }
        if (current->deep >= deep_limit || *address_count_found > max_autocomplete_results) {
            break;
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (current->childNodes[i] != NULL) {
                // We add it to the list to go back on it after && we add it to the addresses_found
                FifoSearchTrieList_Push(list, current->childNodes[i]);

                for (int j = 0; j < current->childNodes[i]->address_count; j++) {
                    if (*address_count_found >= max_autocomplete_results)
                        break;
                    addresses_found[*address_count_found] = current->childNodes[i]->address[j];
                    (*address_count_found)++;

                }
            }

            if (*address_count_found >= max_autocomplete_results)
                break;
        }
    }

    FifoSearchTrieList_Destroy(list);
    return addresses_found;
}

City **SearchTree_AutoCompleteCitySearch(SearchTrie *tree, char *query, int *city_count_found, int max_autocomplete_results, int deep_limit) {
if (!tree || !query || !city_count_found) {
        error("Invalid parameters for SearchTrie_ExactCitySearch\n");
        return NULL;
    }

    SearchTrieNode *current = tree->root;

    for (int i = 0; i < strlen(query); i++) {
        int index = GetSearchTrieIndex(query[i]);
        if (index == -1) {
            continue;
        }

        if (current->childNodes[index] == NULL) {
            *city_count_found = 0;
            return NULL;
        }
        current = current->childNodes[index];
    }

    // when the number of cities found is greater than the max_autocomplete_results, we return the cities
    if (current->city_count > max_autocomplete_results) {
        *city_count_found = current->city_count;
        return current->cities;
    }

    City **cities_found = (City **) calloc(max_autocomplete_results, sizeof(City *));
    if (!cities_found) {
        error("Failed to allocate memory for cities array\n");
        exit(EXIT_FAILURE);
    }
    *city_count_found = current->city_count > max_autocomplete_results ? max_autocomplete_results : current->city_count;
    for (int i = 0; i < *city_count_found; i++) {
        cities_found[i] = current->cities[i];
    }

    // We will continue to search for cities until we reach the deep limit of 4 characters or we reach the max_autocomplete_results
    FifoSearchTrieList *list = FifoSearchTrieList_Create();
    FifoSearchTrieList_Push(list, current);
    deep_limit += current->deep;

    while (true) {
        current = FifoSearchTrieList_PopFirst(list);
        if (current == NULL) {
            break;
        }
        if (current->deep >= deep_limit || *city_count_found > max_autocomplete_results) {
            break;
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (current->childNodes[i] != NULL) {
                // We add it to the list to go back on it after && we add it to the cities_found
                FifoSearchTrieList_Push(list, current->childNodes[i]);

                for (int j = 0; j < current->childNodes[i]->city_count; j++) {
                    if (*city_count_found >= max_autocomplete_results)
                        break;
                    cities_found[*city_count_found] = current->childNodes[i]->cities[j];
                    (*city_count_found)++;

                }
            }

            if (*city_count_found >= max_autocomplete_results)
                break;
        }
    }

    FifoSearchTrieList_Destroy(list);
    return cities_found;
}

FifoSearchTrieList *FifoSearchTrieList_Create(void) {
    FifoSearchTrieList *list = (FifoSearchTrieList *) calloc(1, sizeof(FifoSearchTrieList));
    AssertNew(list);

    list->size = 0;

    return list;
}

void FifoSearchTrieList_Destroy(FifoSearchTrieList *list) {
    if (!list) return;

    FifoSearchTrieListNode *current = list->first;
    while (current != NULL) {
        FifoSearchTrieListNode *next = current->next;
        free(current);
        current = next;
    }

    free(list);
}

void FifoSearchTrieList_Push(FifoSearchTrieList *list, SearchTrieNode *node) {
    if (!list || !node) {
        error("Invalid parameters for FifoSearchTrieList_Push\n");
        exit(EXIT_FAILURE);
    }


    FifoSearchTrieListNode *new_node = (FifoSearchTrieListNode *) calloc(1, sizeof(FifoSearchTrieListNode));
    if (!new_node) {
        error("Failed to allocate memory for FifoSearchTrieListNode\n");
        exit(EXIT_FAILURE);
    }

    new_node->node = node;
    new_node->next = NULL;

    if (list->size == 0) {
        list->first = new_node;
        list->last = new_node;
    } else {
        list->last->next = new_node;
        list->last = new_node;
    }

    list->size++;
}

SearchTrieNode *FifoSearchTrieList_PopFirst(FifoSearchTrieList *list) {
    if (!list) {
        error("Invalid parameters for FifoSearchTrieList_PopFirst\n");
        exit(EXIT_FAILURE);
    }

    if (list->size == 0) {
        return NULL;
    }

    FifoSearchTrieListNode *first = list->first;
    list->first = first->next;
    list->size--;

    SearchTrieNode *node = first->node;
    free(first);

    return node;
}
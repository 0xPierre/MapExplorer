#pragma once
#include "DataPipeline.h"

typedef struct SearchTrieNode SearchTrieNode;
struct SearchTrieNode {
    Address **address;
    SearchTrieNode **childNodes; // 26 letters + 10 digits + 1 for space
    int address_count;
    City **cities;
    int city_count;
    int deep;
    char c;
};

typedef struct SearchTrie SearchTrie;
struct SearchTrie {
    SearchTrieNode *root;
};

SearchTrie *SearchTrie_Create(void);

SearchTrieNode *SearchTrieNode_Create(void);

void SearchTrieNode_Destroy(SearchTrieNode *node);

void SearchTrie_Destroy(SearchTrie *trie);

int GetSearchTrieIndex(char c);

void SearchTrie_InsertAddress(SearchTrie *tree, Address *address);

void SearchTrie_InsertCity(SearchTrie *tree, City *city);

Address **SearchTree_AutoCompleteAddressSearch(SearchTrie *tree, char *query, int *address_count_found, int max_autocomplete_results, int deep_limit);

City **SearchTree_AutoCompleteCitySearch(SearchTrie *tree, char *query, int *city_count_found, int max_autocomplete_results, int deep_limit);

typedef struct FifoSearchTrieListNode FifoSearchTrieListNode;
struct FifoSearchTrieListNode {
    SearchTrieNode *node;
    FifoSearchTrieListNode *next;
};

typedef struct FifoSearchTrieList FifoSearchTrieList;
struct FifoSearchTrieList {
    FifoSearchTrieListNode *first;
    FifoSearchTrieListNode *last;
    int size;
};

FifoSearchTrieList *FifoSearchTrieList_Create(void);

void FifoSearchTrieList_Destroy(FifoSearchTrieList *list);

void FifoSearchTrieList_Push(FifoSearchTrieList *list, SearchTrieNode *node);

SearchTrieNode *FifoSearchTrieList_PopFirst(FifoSearchTrieList *list);
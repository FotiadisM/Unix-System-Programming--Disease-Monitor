#ifndef MAXHEAP_H
#define MAXHEAP_H

typedef struct maxHeapNode {
    char* key;
    int count;
    struct maxHeapNode *left;
    struct maxHeapNode *right;
} maxHeapNode;

typedef maxHeapNode* maxHeapNodePtr;

typedef struct maxHeap {
    int len;
    maxHeapNodePtr root;
} maxHeap;

typedef maxHeap* maxHeapPtr;

maxHeapNodePtr maxHeapNode_Init(const char* str, int count);

maxHeapPtr maxHeap_Init();

void maxHeap_Close(maxHeapPtr heap);

void maxHeapNode_Close(maxHeapNodePtr node);

int maxHeapNode_Swap(maxHeapNodePtr node1,maxHeapNodePtr node2);

int CBTInsert(maxHeapPtr heap, maxHeapNodePtr newNode);

maxHeapNodePtr CBTGetLast(maxHeapPtr heap);

int maxHeap_Insert(maxHeapPtr heap, const char* key, const int count);

int maxHeapNode_Remove(maxHeapPtr heap);

#endif
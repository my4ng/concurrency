#ifndef CONCURRENCY_ADDITIVE_LIST_H
#define CONCURRENCY_ADDITIVE_LIST_H

#include <stdint.h>
#include <malloc.h>
#include <string.h>

// potential performance improvement with blockMultiplier being a power of 2 (for modulo)

typedef struct AdditiveList_t {
    uint16_t elementSize;
    uint16_t blockMultiplier;
    uint16_t maxBlock;

    uint64_t offset;
    uint16_t currentBlockNumber;
    uint32_t currentBlockStartIndex;
    uint32_t listLength;
    uint16_t arrayOffset;
    char **blockArrayPointer;
} AdditiveList;


int additive_list_init(AdditiveList *additiveList, uint16_t elementSize, uint16_t blockMultiplier, uint16_t maxBlock,
                       uint64_t offset);

int additive_list_add(AdditiveList *additiveList, void *data);

void *additive_list_get(AdditiveList *additiveList, uint64_t offsetIndex);

int additive_list_get_copy(AdditiveList *additiveList, uint64_t offsetIndex, void *copy);

void additive_list_remove_before(AdditiveList *additiveList, uint64_t offsetIndex);

void additive_list_free(AdditiveList *additiveList);

#endif //CONCURRENCY_ADDITIVE_LIST_H

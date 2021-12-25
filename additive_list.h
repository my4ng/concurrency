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


char **get_array_pointer(AdditiveList *additiveList, uint16_t blockNumber) {
    return additiveList->blockArrayPointer + (additiveList->arrayOffset + blockNumber < additiveList->maxBlock ?
                                              additiveList->arrayOffset + blockNumber : additiveList->arrayOffset +
                                                                                        blockNumber -
                                                                                        additiveList->maxBlock);
}

// incorrect elementSize may lead to undefined behaviour

int additive_list_init(AdditiveList *additiveList, uint16_t elementSize, uint16_t blockMultiplier, uint16_t maxBlock,
                       uint64_t offset) {
    *additiveList = (AdditiveList) {.elementSize = elementSize, .blockMultiplier = blockMultiplier,
            .maxBlock = maxBlock, .offset = offset, .currentBlockNumber = -1,
            .currentBlockStartIndex = -blockMultiplier, .listLength = 0, .arrayOffset = 0};

    additiveList->blockArrayPointer = malloc(maxBlock * sizeof(void *));
    if (additiveList->blockArrayPointer == NULL) return 1;

    void *firstBlockPointer = malloc(elementSize * blockMultiplier);
    if (firstBlockPointer == NULL) {
        free(additiveList->blockArrayPointer);
        return 2;
    }
    additiveList->blockArrayPointer = firstBlockPointer;
    return 0;
}

// This will insert a copy of the data, not the original into the array list. If an original is desired,
// store the pointer to that data inside instead.

int additive_list_add(AdditiveList *additiveList, void *data) {
    // TODO: add functionality to scan through the bankAccounts to delete all entries in the list such that
    //       all current transactions have a later start time

    // TODO: implement a circular buffer queue with semaphores to allow p-c model for validation and write-back

    if (additiveList->listLength == additiveList->currentBlockStartIndex + additiveList->blockMultiplier) {
        if (additiveList->currentBlockNumber == additiveList->maxBlock - 1) return 2;
        char *nextBlockPointer = malloc(additiveList->blockMultiplier * additiveList->elementSize);
        if (nextBlockPointer == NULL) return 1;
        additiveList->currentBlockNumber += 1;
        *get_array_pointer(additiveList, additiveList->currentBlockNumber) = nextBlockPointer;
        additiveList->currentBlockStartIndex += additiveList->blockMultiplier;
    }

    memcpy(*get_array_pointer(additiveList, additiveList->currentBlockNumber) +
           ((additiveList->listLength - additiveList->currentBlockStartIndex) * additiveList->elementSize), data,
           additiveList->elementSize);
    additiveList->listLength += 1;

    return 0;
}

// does not create a copy of the returned element

void *additive_list_get(AdditiveList *additiveList, uint64_t offset_index) {

    uint32_t index = offset_index - additiveList->offset;
    if (index >= additiveList->listLength) return NULL;

    return *get_array_pointer(additiveList, index / additiveList->blockMultiplier) +
           ((index % additiveList->blockMultiplier) * additiveList->elementSize);
}

void additive_list_remove_first_block(AdditiveList *additiveList) {
    free(*get_array_pointer(additiveList, 0));

    additiveList->offset += additiveList->blockMultiplier;
    additiveList->currentBlockNumber -= 1;
    additiveList->currentBlockStartIndex -= additiveList->blockMultiplier;
    additiveList->listLength -= additiveList->blockMultiplier;
    additiveList->arrayOffset += 1;
}

// if the additive list itself is dynamically allocated, then it will not be freed in this function

void additive_list_free(AdditiveList *additiveList) {
    for (int i = 0; i <= additiveList->currentBlockNumber; i++) {
        free(get_array_pointer(additiveList, i));
    }
    free(additiveList->blockArrayPointer);
}

#endif //CONCURRENCY_ADDITIVE_LIST_H

#include "additive_list.h"

char **get_array_pointer(AdditiveList *additiveList, uint16_t blockNumber) {
    return additiveList->blockArrayPointer + (additiveList->arrayOffset + blockNumber < additiveList->maxBlock ?
                                              additiveList->arrayOffset + blockNumber : additiveList->arrayOffset +
                                                                                        blockNumber -
                                                                                        additiveList->maxBlock);
}

// incorrect elementSize may lead to undefined behaviour

int additive_list_init(AdditiveList *additiveList, uint16_t elementSize, uint16_t blockMultiplier, uint16_t maxBlock,
                       uint64_t offset) {
    additiveList->blockArrayPointer = malloc(maxBlock * sizeof(char *));
    if (additiveList->blockArrayPointer == NULL) return 1;

    *additiveList = (AdditiveList) {.elementSize = elementSize, .blockMultiplier = blockMultiplier,
            .maxBlock = maxBlock, .offset = offset, .currentBlockNumber = -1,
            .currentBlockStartIndex = -blockMultiplier, .listLength = 0, .arrayOffset = 0};
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
// can return a copy if a void * pointer is provided and use memcpy
// if used in MRSW then no need since

void *additive_list_get(AdditiveList *additiveList, uint64_t offsetIndex) {

    uint32_t index = offsetIndex - additiveList->offset;
    if (index >= additiveList->listLength) return NULL;

    return *get_array_pointer(additiveList, index / additiveList->blockMultiplier) +
           ((index % additiveList->blockMultiplier) * additiveList->elementSize);
}

int additive_list_get_copy(AdditiveList *additiveList, uint64_t offsetIndex, void *copy) {
    uint32_t index = offsetIndex - additiveList->offset;
    if (index >= additiveList->listLength) return 1;

    memcpy(copy, *get_array_pointer(additiveList, index / additiveList->blockMultiplier) +
                 ((index % additiveList->blockMultiplier) * additiveList->elementSize), additiveList->elementSize);
    return 0;
}

// remove all elements that are in blocks before the offset index
// for validation list, safe to remove all elements before the

void additive_list_remove_before(AdditiveList *additiveList, uint64_t offsetIndex) {

    uint16_t blockIndex = (offsetIndex - additiveList->offset) / additiveList->blockMultiplier;
    for (uint16_t i = 0; i < blockIndex; i++) {
        free(*get_array_pointer(additiveList, i));
    }

    additiveList->offset += blockIndex * additiveList->blockMultiplier;
    additiveList->currentBlockNumber -= blockIndex;
    additiveList->currentBlockStartIndex -= blockIndex * additiveList->blockMultiplier;
    additiveList->listLength -= blockIndex * additiveList->blockMultiplier;
    additiveList->arrayOffset =
            additiveList->arrayOffset + blockIndex < additiveList->maxBlock ? additiveList->arrayOffset + blockIndex :
            additiveList->arrayOffset + blockIndex - additiveList->maxBlock;
}

// if the additive list itself is dynamically allocated, it will not be freed in this function

void additive_list_free(AdditiveList *additiveList) {
    for (int i = 0; i <= additiveList->currentBlockNumber; i++) {
        free(*get_array_pointer(additiveList, i));
    }
    free(additiveList->blockArrayPointer);
}
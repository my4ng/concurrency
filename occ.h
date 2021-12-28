#ifndef CONCURRENCY_OCC_H
#define CONCURRENCY_OCC_H

#include <stdbool.h>
#include <stdatomic.h>
#include "bank_account.h"
#include "additive_list.h"
#include "circular_queue.h"

typedef struct BankAccountData_t {
    BankAccount *bankAccount;
    uint64_t copyTimestamp;
    uint64_t id;
    int64_t balance;
} BankAccountData;

typedef struct ValidationRequest_t {
    uint64_t startTime;
    BankAccountData *bankAccountData1;
    BankAccountData *bankAccountData2;
    bool isBankAccount1Updated;
    bool isBankAccount2Updated;
} ValidationRequest;

typedef struct ValidationWriteBackResult_t {
    ValidationRequest *validationRequest;
    bool *result;
    pthread_mutex_t *resultMutex;
    pthread_cond_t *resultConditionVariable;
    uint64_t validationTimestamp;
} ValidationWriteBackResult;

// With internal padding, sizeof(ValidationEntry) is actually 24 bytes
// Hence TODO: add some extra data in if necessary

typedef struct ValidationEntry_t {
    BankAccount *bankAccount1;
    BankAccount *bankAccount2;
    bool hasWrittenBack;
} ValidationEntry;

typedef struct ValidationList_t {
    AdditiveList additiveList;
    pthread_mutex_t writeMutex;
    pthread_mutex_t readMutex;
    pthread_mutex_t turnMutex;
    uint16_t numberReaders;
} ValidationList;

ValidationList validationList_default;

typedef struct OCCContainer_t {
    ValidationList validationList;
    CircularQueue validationCircularQueue;
    CircularQueue writeBackCircularQueue;
} OCCContainer;

void get_shadow_copy(BankAccount *bankAccount, BankAccountData *bankAccountData);

uint64_t get_start_time(ValidationList *validationList);

bool submit_validation_request(ValidationRequest *validationRequest, CircularQueue *validationCircularQueue);

void validate_transaction(ValidationList *validationList, CircularQueue *validationCircularQueue,
                          CircularQueue *writeBackCircularQueue);

void write_back_transaction(ValidationList *validationList, CircularQueue *writeBackCircularQueue);

#endif //CONCURRENCY_OCC_H

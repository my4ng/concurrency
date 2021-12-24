#ifndef CONCURRENCY_OCC_H
#define CONCURRENCY_OCC_H

#include <stdbool.h>
#include "bank_account.h"
#include "glib.h"

typedef struct BankAccountData_t {
    BankAccount *bankAccount;
    uint64_t copyTimestamp;
    uint64_t id;
    int64_t balance;
    Transaction *lastTransaction;
} BankAccountData;

typedef struct ValidationRequest_t {
    Transaction *transaction;
    uint64_t startTime;
    BankAccountData *bankAccountData1;
    BankAccountData *bankAccountData2;
    bool isBankAccount1Updated;
    bool isBankAccount2Updated;
} ValidationRequest;

typedef struct ValidationResult_t {
    ValidationRequest *validationRequest;
    bool *result;
    pthread_mutex_t *resultMutex;
    pthread_cond_t *resultConditionVariable;
} ValidationResult;

typedef struct ValidationEntry_t {
    Transaction *transaction;
    uint64_t validationTimestamp;
    BankAccount *bankAccount1;
    BankAccount *bankAccount2;
    bool hasWrittenBack;
} ValidationEntry;

typedef struct ValidationList_t {
    GArray *arrayList;
    pthread_mutex_t writeMutex;
    pthread_mutex_t readMutex;
    pthread_mutex_t turnMutex;
    uint16_t numberReaders;
    uint64_t timeStamp;
} ValidationList;

ValidationList validationList_default;

GArray *get_new_arrayList(void);

typedef struct WriteBackEntry_t {
    BankAccountData *bankAccountData1;
    BankAccountData *bankAccountData2;
    uint64_t validationListIndex;
} WriteBackEntry;

typedef struct OCCContainer_t {
    ValidationList validationList;
    GAsyncQueue *validationAsyncQueue;
    GAsyncQueue *writeBackAsyncQueue;
} OCCContainer;

GAsyncQueue *get_new_async_queue(void);

BankAccountData *get_shadow_copy(BankAccount *bankAccount);

uint64_t get_start_time(ValidationList *validationList);

bool submit_validation_request(ValidationRequest *validationRequest, GAsyncQueue *validationAsyncQueue);

void validate_transaction(ValidationList *validationList, GAsyncQueue *validationAsyncQueue, GAsyncQueue *writeBackAsyncQueue);

void write_back(GAsyncQueue *writeBackAsyncQueue, ValidationList *validationList, const bool *isStopped);

#endif //CONCURRENCY_OCC_H

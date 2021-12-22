#ifndef CONCURRENCY_OCC_H
#define CONCURRENCY_OCC_H

#include <stdbool.h>
#include <semaphore.h>
#include "bank_account.h"
#include "glib.h"

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

typedef struct BankAccountData_t {
    BankAccount *bankAccount;
    uint64_t copyTimestamp;
    uint64_t id;
    int64_t balance;
    Transaction *lastTransaction;
} BankAccountData;

typedef struct WriteBackEntry_t {
    BankAccountData *bankAccountData1;
    BankAccountData *bankAccountData2;
    uint64_t validationListIndex;
} WriteBackEntry;

typedef struct WriteBackContainer_t {
    GHashTable *writeBackHashTable;
    GQueue *writeBackQueue;
    sem_t writeBackSemaphore;
} WriteBackContainer;

typedef struct OCCContainer_t {
    ValidationList validationList;
    WriteBackContainer writeBackContainer;
} OCCContainer;

BankAccountData get_shadow_copy(BankAccount *bankAccount, WriteBackContainer *writeBackContainer);
uint64_t get_start_time(ValidationList *validationList);
bool validate_transaction(Transaction *transaction, BankAccountData *bankAccountData1, BankAccountData *bankAccountData2,
                          uint64_t startTime, ValidationList *validationList, bool isBankAccount1Updated,
                          bool isBankAccount2Updated, WriteBackContainer *writeBackContainer);
void write_back(WriteBackContainer *writeBackContainer, ValidationList *validationList, const bool *isStopped);

#endif //CONCURRENCY_OCC_H

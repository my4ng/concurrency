#ifndef CONCURRENCY_BANK_ACCOUNT_H
#define CONCURRENCY_BANK_ACCOUNT_H

#include <stdint.h>
#include <pthread.h>

typedef struct Transaction_t {
    enum {
        deposit, withdraw, payment,
    } type;
    uint64_t transactingAccount;
    uint64_t amount;
    enum {
        success, failure
    } statusCode;
    struct Transaction_t *previousTransaction;
} Transaction;

typedef struct BankAccount_t {
    uint64_t id;
    int64_t balance;
    Transaction *lastTransaction;
    pthread_mutex_t mutex;
    pthread_mutex_t transactionMutex;
} BankAccount;

#endif //CONCURRENCY_BANK_ACCOUNT_H
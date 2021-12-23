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
    uint64_t writeBackTimestamp;
    pthread_mutex_t writeBackMutex;
} BankAccount;

BankAccount *init_account(uint64_t id, int64_t balance, uint64_t writeBackTimestamp);
void free_account(BankAccount *bankAccount);

#endif //CONCURRENCY_BANK_ACCOUNT_H
#include "operation.h"
#include "bank_account.h"
#include <stdlib.h>

BankAccount *init_account(uint64_t id, int64_t balance, Transaction *lastTransaction) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t transactionMutex = PTHREAD_MUTEX_INITIALIZER;
    BankAccount *ba = malloc(sizeof(BankAccount));
    if (ba != NULL) *ba = (BankAccount) {.id = id, .balance = balance, .lastTransaction = lastTransaction, .mutex = mutex, .transactionMutex = transactionMutex};
    return ba;
}

int credit_account(BankAccount *bankAccount, uint64_t amount, Transaction **previousTransaction) {
    pthread_mutex_lock(&(bankAccount->mutex));
    pthread_mutex_lock(&(bankAccount->transactionMutex));
    *previousTransaction = bankAccount->lastTransaction;
    pthread_mutex_unlock(&(bankAccount->transactionMutex));
    bankAccount->balance += (int64_t) amount;
    pthread_mutex_unlock(&(bankAccount->mutex));
    return 0;
}

int debit_account(BankAccount *bankAccount, uint64_t amount, Transaction **previousTransaction) {
    int code = 1;
    pthread_mutex_lock(&(bankAccount->mutex));
    pthread_mutex_lock(&(bankAccount->transactionMutex));
    *previousTransaction = bankAccount->lastTransaction;
    pthread_mutex_unlock(&(bankAccount->transactionMutex));
    if (bankAccount->balance >= amount) {
        bankAccount->balance -= (int64_t) amount;
        code = 0;
    }
    pthread_mutex_unlock(&(bankAccount->mutex));
    return code;
}

int destroy_account(BankAccount *bankAccount) {

    // Dangerous to use!!! We have to ensure that the pointer *bankAccount WILL NOT BE FOUND OR USED!!! If someone puts
    // it as the parameter to some function and then calls it, then undefined behaviours will occur.

    Transaction *transaction = bankAccount->lastTransaction;
    Transaction *previousTransaction = NULL;
    while(transaction != NULL) {
        previousTransaction = transaction->previousTransaction;
        free(transaction);
        transaction = previousTransaction;
    }
    free(bankAccount);
    return 0;
}
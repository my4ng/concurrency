#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <glib.h>
#include <stdbool.h>
#include "transaction.h"

/*
int insert_transaction(BankAccount *bankAccount, Transaction *transaction) {
    pthread_mutex_lock(&(bankAccount->transactionMutex));
    Transaction *previousTransaction = bankAccount->lastTransaction;
    Transaction *afterTransaction = NULL;
    while (previousTransaction != transaction->previousTransaction && previousTransaction != NULL) {
        afterTransaction = previousTransaction;
        previousTransaction = previousTransaction->previousTransaction;
    }
    if (afterTransaction != NULL) afterTransaction->previousTransaction = transaction;
    else bankAccount->lastTransaction = transaction;
    transaction->previousTransaction = previousTransaction;
    pthread_mutex_unlock(&(bankAccount->transactionMutex));
    return 0;
}

int withdraw_account(BankAccount *bankAccount, uint64_t amount) {
    Transaction *transaction = malloc(sizeof(Transaction));
    if (transaction == NULL) return 1;

    Transaction *tempTransaction = NULL;
    Transaction **previousTransaction = &tempTransaction;
    *transaction = (Transaction) {.type = withdraw, .amount = amount, .transactingAccount = 0};

    transaction->statusCode = (debit_account(bankAccount, amount, previousTransaction) == 0 ? success : failure);
    transaction->previousTransaction = *previousTransaction;
    return insert_transaction(bankAccount, transaction);
}

int deposit_account(BankAccount *bankAccount, uint64_t amount) {
    Transaction *transaction = malloc(sizeof(Transaction));
    if (transaction == NULL) return 1;

    Transaction *tempTransaction = NULL;
    Transaction **previousTransaction = &tempTransaction;
    *transaction = (Transaction) {.type = deposit, .amount = amount, .transactingAccount = 0};

    transaction->statusCode = (credit_account(bankAccount, amount, previousTransaction) == 0 ? success : failure);
    transaction->previousTransaction = *previousTransaction;
    return insert_transaction(bankAccount, transaction);
}

int pay_account(BankAccount *bankAccountFrom, BankAccount *bankAccountTo, uint64_t amount) {

}
*/

int main(void) {

    BankAccount *ba = init_account(0, 1000000, 0);
    BankAccount *ba1 = init_account(1, 1000, 0);

    OCCContainer occContainer;
    occContainer.validationList = validationList_default;
    occContainer.validationList.arrayList = get_new_arrayList();
    occContainer.writeBackAsyncQueue = get_new_async_queue();

    pay_account(ba, ba1, 1000, &occContainer, 10);
    pay_account(ba1, ba, 3000, &occContainer, 10);

    free(ba);
    free(ba1);


    return 0;
}
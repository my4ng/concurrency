#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <glib.h>
#include <stdbool.h>

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


struct Test {
    int a;
    struct Test* b;
};

int main() {
    struct Test t = {1, NULL};
    struct Test u = {2, NULL};

    GArray *g = g_array_new(false, false, sizeof(struct Test));
    g_array_append_val(g, t);
    g_array_append_val(g, u);
    g_array_append_val(g, u);
    g_array_append_val(g, u);
    g_array_append_val(g, u);
    g_array_append_val(g, u);


    return 0;
}
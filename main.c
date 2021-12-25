#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "transaction.h"
#include "additive_list.h"


OCCContainer occContainer;
BankAccount *bankAccount;
BankAccount *bankAccount1;

void *withdraw_100_times() {
    for (int i = 0; i < 10; i++) {
        withdraw_account(bankAccount, 50000, &occContainer, 10);
    }
    printf("Withdrawn 100 times\n");
    return NULL;
}

void *deposit_100_times() {
    for (int i = 0; i < 100; i++) {
        deposit_account(bankAccount, 10, &occContainer, 100);
    }
    printf("Deposited 100 times\n");
    return NULL;
}

void *pay_100_times() {
    for (int i = 0; i < 100; i++) {
        pay_account(bankAccount, bankAccount1, 10, &occContainer, 100);
    }
    printf("Paid 100 times\n");
    return NULL;
}

void *validate() {
    validate_transaction(&occContainer.validationList, occContainer.validationAsyncQueue, occContainer.writeBackAsyncQueue);
    return NULL;
}

void *write_back() {
    write_back_transaction(&occContainer.validationList, occContainer.writeBackAsyncQueue);
    return NULL;
}


int main(void) {

    /*
    printf("BankAccount: %ld\n", sizeof(WriteBackEntry));

    bankAccount = init_account(0, 1000000, 0);
    bankAccount1 = init_account(1, 1000, 0);

    occContainer.validationList = validationList_default;
    occContainer.validationList.arrayList = get_new_arrayList();
    occContainer.validationAsyncQueue = get_new_async_queue();
    occContainer.writeBackAsyncQueue = get_new_async_queue();

    pthread_t transactionThread1, transactionThread2, transactionThread3, validationThread, writeBackThread;
    int tt1, tt2, tt3, vt, wt;

    tt1 = pthread_create(&transactionThread1, NULL, withdraw_100_times, NULL);
    tt2 = pthread_create(&transactionThread2, NULL, deposit_100_times, NULL);
    tt3 = pthread_create(&transactionThread3, NULL, pay_100_times, NULL);
    vt = pthread_create(&validationThread, NULL, validate, NULL);
    wt = pthread_create(&writeBackThread, NULL, write_back, NULL);

    if (tt1 != 0 || tt2 != 0 || tt3 != 0 || vt != 0 || wt != 0) exit(1);

    pthread_join(transactionThread1, NULL);
    pthread_join(transactionThread2, NULL);
    pthread_join(transactionThread3, NULL);

    // pray to God that everything has finished before then!
    sleep(3);
    printf("BankAccount: %ld\n", bankAccount->balance);
    printf("BankAccount1: %ld\n", bankAccount1->balance);

    free_account(bankAccount);
    free_account(bankAccount1);
    */

    AdditiveList additiveList;
    additive_list_init(&additiveList, sizeof(int), 16, 16, 0);

    for (int i = 0; i < 35; i++) {
        additive_list_add(&additiveList, &i);
    }
    additive_list_remove_before(&additiveList, 33);
    for (int i = 0; i < 35; i++) {
        additive_list_add(&additiveList, &i);
    }
    printf("%d", *(int *)additive_list_get(&additiveList, 69));

    additive_list_free(&additiveList);

    return 0;
}
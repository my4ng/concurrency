#include <malloc.h>
#include <semaphore.h>
#include "occ.h"


BankAccountData *get_shadow_copy(BankAccount *bankAccount, WriteBackContainer *writeBackContainer) {
    BankAccountData *bankAccountData = malloc(sizeof(BankAccountData));
    // TODO: free in write-back
    pthread_mutex_lock(&(bankAccount->mutex));
    uint64_t timestamp = *((uint64_t *) g_hash_table_lookup(writeBackContainer->writeBackHashTable, bankAccount));
    *bankAccountData = (BankAccountData) {.bankAccount = bankAccount,
            .copyTimestamp = timestamp,
            .id = bankAccount->id,
            .balance = bankAccount->balance,
            .lastTransaction = bankAccount->lastTransaction};
    pthread_mutex_unlock(&(bankAccount->mutex));
    return bankAccountData;
}

uint64_t get_start_time(ValidationList *validationList) {
    ValidationEntry *validationEntry = NULL;
    for (unsigned int i = g_array_get_element_size(validationList->arrayList) - 1; i >= 0; i--) {
        if ((validationEntry = &g_array_index(validationList->arrayList, ValidationEntry, i))->hasWrittenBack ==
            true)
            break;
    }
    if (validationEntry == NULL) return 0;
    else return validationEntry->validationTimestamp;
}

bool read_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime) {
    if (bankAccountData1 != NULL && bankAccountData1->copyTimestamp > startTime) return false;
    if (bankAccountData2 != NULL && bankAccountData2->copyTimestamp > startTime) return false;
    return true;
}

bool serialisability_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime,
                              ValidationList *validationList) {
    ValidationEntry *validationEntry = NULL;

    for (unsigned int i = g_array_get_element_size(validationList->arrayList) - 1; i >= 0; i--) {
        validationEntry = &g_array_index(validationList->arrayList, ValidationEntry, i);
        if (bankAccountData1 != NULL && bankAccountData1->bankAccount == validationEntry->bankAccount1) return false;
        if (bankAccountData2 != NULL && bankAccountData2->bankAccount == validationEntry->bankAccount2) return false;
        if (startTime == validationEntry->validationTimestamp) break;
    }
    return true;
}

bool
validate_transaction(Transaction *transaction, BankAccountData *bankAccountData1, BankAccountData *bankAccountData2,
                     uint64_t startTime, ValidationList *validationList, bool isBankAccount1Updated,
                     bool isBankAccount2Updated, WriteBackContainer *writeBackContainer) {
    bool isValidated;
    uint64_t validationListIndex;
    if ((isValidated = read_validate(bankAccountData1, bankAccountData2, startTime) &&
        serialisability_validate(bankAccountData1, bankAccountData2, startTime, validationList))) {
        ValidationEntry validationEntry = {.transaction = transaction,
                .bankAccount1 = (bankAccountData1 != NULL && isBankAccount1Updated) ? bankAccountData1->bankAccount
                                                                                    : NULL,
                .bankAccount2 = (bankAccountData2 != NULL && isBankAccount2Updated) ? bankAccountData2->bankAccount
                                                                                    : NULL,
                .hasWrittenBack = false};
        pthread_mutex_lock(&(validationList->validationMutex));
        validationEntry.validationTimestamp = ++(validationList->timeStamp);
        validationListIndex = validationList->arrayList->len;
        g_array_append_val(validationList->arrayList, validationEntry);
        pthread_mutex_unlock(&(validationList->validationMutex));

        WriteBackEntry writeBackEntry = {.bankAccountData1 = isBankAccount1Updated ? bankAccountData1 : NULL,
                                         .bankAccountData2 = isBankAccount2Updated ? bankAccountData2 : NULL,
                                         .validationListIndex = validationListIndex};
        g_queue_push_tail(writeBackContainer->writeBackQueue, &writeBackEntry);
        sem_post(&(writeBackContainer->writeBackSemaphore));
    } else {
        free(bankAccountData1);
        free(bankAccountData2);
    }
    return isValidated;
}

void write_back(WriteBackContainer *writeBackContainer, ValidationList *validationList, const bool *isStopped) {
    while (!(*isStopped)) {
        sem_wait(&(writeBackContainer->writeBackSemaphore));
        WriteBackEntry *writeBackEntry = g_queue_pop_head(writeBackContainer->writeBackQueue);
        uint64_t validationTimestamp = g_array_index(validationList->arrayList, ValidationEntry, writeBackEntry->validationListIndex).validationTimestamp;

        if (writeBackEntry->bankAccountData1 != NULL) {
            pthread_mutex_lock(&(writeBackEntry->bankAccountData1->bankAccount->mutex));
            // TODO: Add more changes than balance
            writeBackEntry->bankAccountData1->bankAccount->balance = writeBackEntry->bankAccountData1->balance;
            g_hash_table_insert(writeBackContainer->writeBackHashTable,
                                writeBackEntry->bankAccountData1->bankAccount, &validationTimestamp);
            pthread_mutex_unlock(&(writeBackEntry->bankAccountData1->bankAccount->mutex));
        }
        if (writeBackEntry->bankAccountData2 != NULL) {
            pthread_mutex_lock(&(writeBackEntry->bankAccountData2->bankAccount->mutex));
            writeBackEntry->bankAccountData2->bankAccount->balance = writeBackEntry->bankAccountData2->balance;
            g_hash_table_insert(writeBackContainer->writeBackHashTable,
                                writeBackEntry->bankAccountData2->bankAccount, &validationTimestamp);
            pthread_mutex_unlock(&(writeBackEntry->bankAccountData2->bankAccount->mutex));
        }

        pthread_mutex_lock(&(validationList->validationMutex));
        g_array_index(validationList->arrayList, ValidationEntry, writeBackEntry->validationListIndex).hasWrittenBack = true;
        pthread_mutex_unlock(&(validationList->validationMutex));

        free(writeBackEntry->bankAccountData1);
        free(writeBackEntry->bankAccountData2);
    }
}

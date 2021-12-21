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
    bool isValidated = false;
    uint64_t validationListIndex;
    if (read_validate(bankAccountData1, bankAccountData2, startTime) &&
        serialisability_validate(bankAccountData1, bankAccountData2, startTime, validationList)) {
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

        WriteBackEntry writeBackEntry = {.bankAccountData1 = bankAccountData1, .bankAccountData2 = bankAccountData2,
                                         .validationListIndex = validationListIndex};
        g_queue_push_tail(writeBackContainer->writeBackQueue, &writeBackEntry);
        sem_post(&(writeBackContainer->writeBackSemaphore));

        isValidated = true;
    }
    return isValidated;
}

void write_back(WriteBackContainer *writeBackContainer, const bool *isStopped) {
    while (!(*isStopped)) {
        sem_wait(&(writeBackContainer->writeBackSemaphore));
        WriteBackEntry *writeBackEntry = g_queue_pop_head(writeBackContainer->writeBackQueue);

        writeBack
    }
}

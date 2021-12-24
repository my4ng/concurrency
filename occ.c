#include <malloc.h>
#include <semaphore.h>
#include "occ.h"

ValidationList validationList_default = {.writeMutex = PTHREAD_MUTEX_INITIALIZER, .readMutex = PTHREAD_MUTEX_INITIALIZER,
        .turnMutex = PTHREAD_MUTEX_INITIALIZER, .numberReaders = 0, .timeStamp = 0};

GArray *get_new_arrayList(void) { return g_array_new(false, false, sizeof(ValidationList)); }

GAsyncQueue *get_new_async_queue(void) { return g_async_queue_new(); }

void read_lock(ValidationList *validationList) {
    // turn mutex
    pthread_mutex_lock(&(validationList->turnMutex));
    pthread_mutex_unlock(&(validationList->turnMutex));

    // stop writes from entering
    pthread_mutex_lock(&(validationList->readMutex));
    validationList->numberReaders += 1;
    if (validationList->numberReaders == 1) pthread_mutex_lock(&(validationList->writeMutex));
    pthread_mutex_unlock(&(validationList->readMutex));
}

void read_unlock(ValidationList *validationList) {
    // allow writes if last leaving
    pthread_mutex_lock(&(validationList->readMutex));
    validationList->numberReaders -= 1;
    if (validationList->numberReaders == 0) pthread_mutex_unlock(&(validationList->writeMutex));
    pthread_mutex_unlock(&(validationList->readMutex));
}

void write_lock(ValidationList *validationList) {
    pthread_mutex_lock(&(validationList->turnMutex));
    pthread_mutex_lock(&(validationList->writeMutex));
}

void write_unlock(ValidationList *validationList) {
    pthread_mutex_unlock(&(validationList->turnMutex));
    pthread_mutex_unlock(&(validationList->writeMutex));
}

BankAccountData *get_shadow_copy(BankAccount *bankAccount) {
    pthread_mutex_lock(&(bankAccount->writeBackMutex));
    BankAccountData *bankAccountData = malloc(sizeof(BankAccountData));
    *bankAccountData = (BankAccountData)
            {
                    .bankAccount = bankAccount,
                    .copyTimestamp = bankAccount->writeBackTimestamp,
                    .id = bankAccount->id,
                    .balance = bankAccount->balance
            };
    pthread_mutex_unlock(&(bankAccount->writeBackMutex));
    return bankAccountData;
}

uint64_t get_start_time(ValidationList *validationList) {
    bool isEntryFound = false;
    ValidationEntry validationEntry;

    read_lock(validationList);
    for (int64_t i = (int64_t) validationList->arrayList->len - 1; i >= 0; i--) {
        if (g_array_index(validationList->arrayList, ValidationEntry, i).hasWrittenBack == true) {
            validationEntry = g_array_index(validationList->arrayList, ValidationEntry, i);
            isEntryFound = true;
            break;
        }
    }
    read_unlock(validationList);

    if (isEntryFound) return validationEntry.validationTimestamp;
    else return 0;
}

bool read_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime) {
    if (bankAccountData1 != NULL && bankAccountData1->copyTimestamp > startTime) return false;
    if (bankAccountData2 != NULL && bankAccountData2->copyTimestamp > startTime) return false;
    return true;
}

bool serialisability_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime,
                              ValidationList *validationList) {
    ValidationEntry *validationEntry = NULL;

    for (int64_t i = (int64_t) validationList->arrayList->len - 1; i >= 0; i--) {
        validationEntry = &g_array_index(validationList->arrayList, ValidationEntry, i);
        if (bankAccountData1 != NULL && bankAccountData1->bankAccount == validationEntry->bankAccount1) return false;
        if (bankAccountData2 != NULL && bankAccountData2->bankAccount == validationEntry->bankAccount2) return false;
        if (startTime == validationEntry->validationTimestamp) break;
    }
    return true;
}

bool submit_validation_request(ValidationRequest *validationRequest, GAsyncQueue *validationAsyncQueue) {
    bool result;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER;
    ValidationResult validationResult = {.validationRequest = validationRequest, .result = &result,
            .resultMutex = &mutex, .resultConditionVariable = &conditionVariable};

    pthread_mutex_lock(&mutex);
    g_async_queue_push(validationAsyncQueue, &validationResult);
    pthread_cond_wait(&conditionVariable, &mutex);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionVariable);
    return result;
}

void validate_transaction(ValidationList *validationList, GAsyncQueue *validationAsyncQueue,
                          GAsyncQueue *writeBackAsyncQueue) {

    ValidationResult validationResult = *((ValidationResult *) g_async_queue_pop(validationAsyncQueue));
    uint64_t validationListIndex;

    read_lock(validationList);
    bool isValidated = read_validate(validationResult.validationRequest->bankAccountData1,
                                     validationResult.validationRequest->bankAccountData2,
                                     validationResult.validationRequest->startTime) &&
                       serialisability_validate(validationResult.validationRequest->bankAccountData1,
                                                validationResult.validationRequest->bankAccountData2,
                                                validationResult.validationRequest->startTime, validationList);
    read_unlock(validationList);

    if (isValidated) {
        ValidationEntry validationEntry = {.transaction = validationResult.validationRequest->transaction,
                .bankAccount1 = (validationResult.validationRequest->bankAccountData1 != NULL &&
                                 validationResult.validationRequest->isBankAccount1Updated)
                                ? validationResult.validationRequest->bankAccountData1->bankAccount
                                : NULL,
                .bankAccount2 = (validationResult.validationRequest->bankAccountData2 != NULL &&
                                 validationResult.validationRequest->isBankAccount2Updated)
                                ? validationResult.validationRequest->bankAccountData2->bankAccount
                                : NULL,
                .hasWrittenBack = false};
        write_lock(validationList);
        validationEntry.validationTimestamp = ++(validationList->timeStamp);
        validationListIndex = validationList->arrayList->len;
        g_array_append_val(validationList->arrayList, validationEntry);
        write_unlock(validationList);

        WriteBackEntry writeBackEntry = {.bankAccountData1 = validationResult.validationRequest->isBankAccount1Updated
                                                             ? validationResult.validationRequest->bankAccountData1
                                                             : NULL,
                .bankAccountData2 = validationResult.validationRequest->isBankAccount2Updated
                                    ? validationResult.validationRequest->bankAccountData2 : NULL,
                .validationListIndex = validationListIndex};
        g_async_queue_push(writeBackAsyncQueue, &writeBackEntry);
    } else {
        free(validationResult.validationRequest->bankAccountData1);
        free(validationResult.validationRequest->bankAccountData2);
    }

    pthread_mutex_lock(validationResult.resultMutex);
    *(validationResult.result) = isValidated;
    pthread_mutex_unlock(validationResult.resultMutex);
    pthread_cond_signal(validationResult.resultConditionVariable);
}

void write_back(GAsyncQueue *writeBackAsyncQueue, ValidationList *validationList, const bool *isStopped) {
    while (!(*isStopped)) {
        WriteBackEntry writeBackEntry = *((WriteBackEntry *) g_async_queue_pop(writeBackAsyncQueue));

        read_lock(validationList);
        uint64_t validationTimestamp = g_array_index(validationList->arrayList, ValidationEntry,
                                                     writeBackEntry.validationListIndex).validationTimestamp;
        read_unlock(validationList);

        if (writeBackEntry.bankAccountData1 != NULL) {
            pthread_mutex_lock(&(writeBackEntry.bankAccountData1->bankAccount->writeBackMutex));
            // TODO: Add more changes than balance
            writeBackEntry.bankAccountData1->bankAccount->balance = writeBackEntry.bankAccountData1->balance;
            writeBackEntry.bankAccountData1->bankAccount->writeBackTimestamp = validationTimestamp;
            pthread_mutex_unlock(&(writeBackEntry.bankAccountData1->bankAccount->writeBackMutex));
        }
        if (writeBackEntry.bankAccountData2 != NULL) {
            pthread_mutex_lock(&(writeBackEntry.bankAccountData2->bankAccount->writeBackMutex));
            // TODO: Add more changes than balance
            writeBackEntry.bankAccountData2->bankAccount->balance = writeBackEntry.bankAccountData2->balance;
            writeBackEntry.bankAccountData2->bankAccount->writeBackTimestamp = validationTimestamp;
            pthread_mutex_unlock(&(writeBackEntry.bankAccountData2->bankAccount->writeBackMutex));
        }

        write_lock(validationList);
        g_array_index(validationList->arrayList, ValidationEntry,
                      writeBackEntry.validationListIndex).hasWrittenBack = true;
        write_unlock(validationList);

        free(writeBackEntry.bankAccountData1);
        free(writeBackEntry.bankAccountData2);
    }
}

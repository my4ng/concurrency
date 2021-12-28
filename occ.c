#include <semaphore.h>
#include "occ.h"

ValidationList validationList_default = {
        .writeMutex = PTHREAD_MUTEX_INITIALIZER,
        .readMutex = PTHREAD_MUTEX_INITIALIZER,
        .turnMutex = PTHREAD_MUTEX_INITIALIZER,
        .numberReaders = 0
};

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

void get_shadow_copy(BankAccount *bankAccount, BankAccountData *bankAccountData) {
    pthread_mutex_lock(&(bankAccount->mutex));
    *bankAccountData = (BankAccountData)
            {
                    .bankAccount = bankAccount,
                    .copyTimestamp = bankAccount->writeBackTimestamp,
                    .id = bankAccount->id,
                    .balance = bankAccount->balance
            };
    pthread_mutex_unlock(&(bankAccount->mutex));
}

// Always use 1 or greater as the additiveList offset to distinguish between 1st entry verified (1 or greater) or not (0 or greater)

uint64_t get_start_time(ValidationList *validationList) {
    read_lock(validationList);
    uint64_t startTime;
    for (startTime = validationList->additiveList.offset + validationList->additiveList.listLength - 1;
         startTime >= validationList->additiveList.offset; startTime--) {
        if (((ValidationEntry *) additive_list_get(&validationList->additiveList, startTime))->hasWrittenBack) {
            break;
        }
    }
    read_unlock(validationList);
    return startTime;
}

bool read_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime) {
    if (bankAccountData1 != NULL && bankAccountData1->copyTimestamp > startTime) return false;
    if (bankAccountData2 != NULL && bankAccountData2->copyTimestamp > startTime) return false;
    return true;
}

bool serialisability_validate(BankAccountData *bankAccountData1, BankAccountData *bankAccountData2, uint64_t startTime,
                              ValidationList *validationList) {
    ValidationEntry *validationEntry = NULL;

    for (uint64_t offsetIndex = validationList->additiveList.offset + validationList->additiveList.listLength - 1;
         offsetIndex >= validationList->additiveList.offset; offsetIndex--) {
        validationEntry = additive_list_get(&validationList->additiveList, offsetIndex);
        if (startTime == offsetIndex) break;
        if (bankAccountData1 != NULL && bankAccountData1->bankAccount == validationEntry->bankAccount1) return false;
        if (bankAccountData2 != NULL && bankAccountData2->bankAccount == validationEntry->bankAccount2) return false;
    }
    return true;
}

bool submit_validation_request(ValidationRequest *validationRequest, CircularQueue *validationCircularQueue) {
    bool result;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER;
    ValidationWriteBackResult validationResult = {.validationRequest = validationRequest, .result = &result,
            .resultMutex = &mutex, .resultConditionVariable = &conditionVariable};

    pthread_mutex_lock(&mutex);
    circular_queue_push(validationCircularQueue, &validationResult);
    pthread_cond_wait(&conditionVariable, &mutex);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionVariable);
    return result;
}

void validate_transaction(ValidationList *validationList, CircularQueue *validationCircularQueue,
                          CircularQueue *writeBackCircularQueue) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        ValidationWriteBackResult validationWriteBackResult;
        circular_queue_pop(validationCircularQueue, &validationWriteBackResult);

        read_lock(validationList);
        bool isValidated = read_validate(validationWriteBackResult.validationRequest->bankAccountData1,
                                         validationWriteBackResult.validationRequest->bankAccountData2,
                                         validationWriteBackResult.validationRequest->startTime) &&
                           serialisability_validate(validationWriteBackResult.validationRequest->bankAccountData1,
                                                    validationWriteBackResult.validationRequest->bankAccountData2,
                                                    validationWriteBackResult.validationRequest->startTime,
                                                    validationList);
        read_unlock(validationList);

        if (isValidated) {
            ValidationEntry validationEntry = {
                    .bankAccount1 = (validationWriteBackResult.validationRequest->bankAccountData1 != NULL &&
                                     validationWriteBackResult.validationRequest->isBankAccount1Updated)
                                    ? validationWriteBackResult.validationRequest->bankAccountData1->bankAccount
                                    : NULL,
                    .bankAccount2 = (validationWriteBackResult.validationRequest->bankAccountData2 != NULL &&
                                     validationWriteBackResult.validationRequest->isBankAccount2Updated)
                                    ? validationWriteBackResult.validationRequest->bankAccountData2->bankAccount
                                    : NULL,
                    .hasWrittenBack = false};
            write_lock(validationList);
            validationWriteBackResult.validationTimestamp = validationList->additiveList.offset +
                                                            validationList->additiveList.listLength;
            additive_list_add(&validationList->additiveList, &validationEntry);
            write_unlock(validationList);

            circular_queue_push(writeBackCircularQueue, &validationWriteBackResult);
        } else {
            pthread_mutex_lock(validationWriteBackResult.resultMutex);
            *(validationWriteBackResult.result) = false;
            pthread_mutex_unlock(validationWriteBackResult.resultMutex);
            pthread_cond_signal(validationWriteBackResult.resultConditionVariable);
        }
    }
#pragma clang diagnostic pop
}

void write_back_transaction(ValidationList *validationList, CircularQueue *writeBackCircularQueue) {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        ValidationWriteBackResult validationWriteBackResult;
        circular_queue_pop(writeBackCircularQueue, &validationWriteBackResult);

        if (validationWriteBackResult.validationRequest->bankAccountData1 != NULL &&
            validationWriteBackResult.validationRequest->isBankAccount1Updated) {
            pthread_mutex_lock(&(validationWriteBackResult.validationRequest->bankAccountData1->bankAccount->mutex));
            // TODO: Add more changes than balance
            validationWriteBackResult.validationRequest->bankAccountData1->bankAccount->balance =
                    validationWriteBackResult.validationRequest->bankAccountData1->balance;
            validationWriteBackResult.validationRequest->bankAccountData1->bankAccount->writeBackTimestamp =
                    validationWriteBackResult.validationTimestamp;
            pthread_mutex_unlock(&(validationWriteBackResult.validationRequest->bankAccountData1->bankAccount->mutex));
        }
        if (validationWriteBackResult.validationRequest->bankAccountData2 != NULL &&
            validationWriteBackResult.validationRequest->isBankAccount2Updated) {
            pthread_mutex_lock(&(validationWriteBackResult.validationRequest->bankAccountData2->bankAccount->mutex));
            // TODO: Add more changes than balance
            validationWriteBackResult.validationRequest->bankAccountData2->bankAccount->balance =
                    validationWriteBackResult.validationRequest->bankAccountData2->balance;
            validationWriteBackResult.validationRequest->bankAccountData2->bankAccount->writeBackTimestamp =
                    validationWriteBackResult.validationTimestamp;
            pthread_mutex_unlock(&(validationWriteBackResult.validationRequest->bankAccountData2->bankAccount->mutex));
        }

        write_lock(validationList);
        ((ValidationEntry *) additive_list_get(&validationList->additiveList,
                                               validationWriteBackResult.validationTimestamp))->hasWrittenBack = true;
        write_unlock(validationList);

        pthread_mutex_lock(validationWriteBackResult.resultMutex);
        *(validationWriteBackResult.result) = true;
        pthread_mutex_unlock(validationWriteBackResult.resultMutex);
        pthread_cond_signal(validationWriteBackResult.resultConditionVariable);
    }
#pragma clang diagnostic pop
}

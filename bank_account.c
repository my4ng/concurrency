#include <malloc.h>
#include "bank_account.h"


BankAccount *init_account(uint64_t id, int64_t balance, uint64_t writeBackTimestamp) {
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    BankAccount *ba = malloc(sizeof(BankAccount));
    if (ba != NULL)
        *ba = (BankAccount) {.id = id, .balance = balance, .writeBackMutex = mutex, .writeBackTimestamp = writeBackTimestamp};
    return ba;
}

void free_account(BankAccount *bankAccount) {
    // Dangerous to use!!! We have to ensure that the pointer *bankAccount WILL NOT BE FOUND OR USED!!! If someone puts
    // it as the parameter to some function and then calls it, then undefined behaviours will occur.
    free(bankAccount);
}

#ifndef CONCURRENCY_OPERATION_H
#define CONCURRENCY_OPERATION_H

#include "occ.h"

bool credit_account(BankAccountData *bankAccountData, uint64_t amount);
bool debit_account(BankAccountData *bankAccountData, uint64_t amount);

#endif //CONCURRENCY_OPERATION_H

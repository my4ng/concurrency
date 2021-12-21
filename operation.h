#ifndef CONCURRENCY_OPERATION_H
#define CONCURRENCY_OPERATION_H

#include "bank_account.h"

BankAccount *init_account(uint64_t, int64_t, Transaction *);
int credit_account(BankAccount *, uint64_t, Transaction **);
int debit_account(BankAccount *, uint64_t, Transaction **);
int destroy_account(BankAccount *);

#endif //CONCURRENCY_OPERATION_H

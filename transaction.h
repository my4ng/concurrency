#ifndef CONCURRENCY_TRANSACTION_H
#define CONCURRENCY_TRANSACTION_H

#include <stdint.h>
#include "occ.h"

bool withdraw_account(BankAccount *bankAccount, uint64_t amount, OCCContainer *occContainer, uint16_t numberTries);

bool deposit_account(BankAccount *bankAccount, uint64_t amount, OCCContainer *occContainer, uint16_t numberTries);

bool pay_account(BankAccount *bankAccountFrom, BankAccount *bankAccountTo, uint64_t amount, OCCContainer *occContainer,
                uint16_t numberTries);

#endif //CONCURRENCY_TRANSACTION_H

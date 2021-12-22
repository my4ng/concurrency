#include "operation.h"


bool credit_account(BankAccountData *bankAccountData, uint64_t amount) {
    bankAccountData->balance += (int64_t) amount;
    return true;
}

bool debit_account(BankAccountData *bankAccountData, uint64_t amount) {
    bool code;
    if ((code = bankAccountData->balance >= amount)) bankAccountData->balance -= (int64_t) amount;
    return code;
}

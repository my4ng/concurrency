#include "transaction.h"
#include "operation.h"
#include "occ.h"

bool withdraw_account(BankAccount *bankAccount, uint64_t amount, OCCContainer *occContainer, uint16_t numberTries) {
    while (numberTries > 0) {
        uint64_t startTime = get_start_time(&(occContainer->validationList));
        BankAccountData *bankAccountData = get_shadow_copy(bankAccount);
        if (!debit_account(bankAccountData, amount)) return false;
        // TODO: Add transaction details
        ValidationRequest validationRequest = {.bankAccountData1 = bankAccountData, .bankAccountData2 = NULL,
                .startTime = startTime, .isBankAccount1Updated = true,
                .isBankAccount2Updated = false, .transaction = NULL};
        if (submit_validation_request(&validationRequest,
                                      occContainer->validationAsyncQueue))
            return true;
        numberTries--;
    }
    return false;
}

bool deposit_account(BankAccount *bankAccount, uint64_t amount, OCCContainer *occContainer, uint16_t numberTries) {
    while (numberTries > 0) {
        uint64_t startTime = get_start_time(&(occContainer->validationList));
        BankAccountData *bankAccountData = get_shadow_copy(bankAccount);
        if (!credit_account(bankAccountData, amount)) return false;
        // TODO: Add transaction details
        ValidationRequest validationRequest = {.bankAccountData1 = bankAccountData, .bankAccountData2 = NULL,
                .startTime = startTime, .isBankAccount1Updated = true,
                .isBankAccount2Updated = false, .transaction = NULL};
        if (submit_validation_request(&validationRequest,
                                      occContainer->validationAsyncQueue))
            return true;
        numberTries--;
    }
    return false;
}

bool pay_account(BankAccount *bankAccountFrom, BankAccount *bankAccountTo, uint64_t amount, OCCContainer *occContainer,
                 uint16_t numberTries) {
    while (numberTries > 0) {
        uint64_t startTime = get_start_time(&(occContainer->validationList));
        BankAccountData *bankAccountDataFrom = get_shadow_copy(bankAccountFrom);
        BankAccountData *bankAccountDataTo = get_shadow_copy(bankAccountTo);

        if (!debit_account(bankAccountDataFrom, amount)) return false;
        if (!credit_account(bankAccountDataTo, amount)) return false;

        // TODO: Add transaction details
        ValidationRequest validationRequest = {.bankAccountData1 = bankAccountDataFrom,
                .bankAccountData2 = bankAccountDataTo,
                .startTime = startTime, .isBankAccount1Updated = true,
                .isBankAccount2Updated = true, .transaction = NULL};
        if (submit_validation_request(&validationRequest,
                                      occContainer->validationAsyncQueue))
            return true;
        numberTries--;
    }
    return false;
}

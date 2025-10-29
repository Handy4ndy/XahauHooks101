//**************************************************************
// Xahau Hook 101 Example ~ Basic State Counter Hook
// Author: @handy_4ndy
//
// Description:
//   This hook increments a counter in state each time a Payment transaction is processed on the hook account.
//   The counter is stored in hook state under the key 'CNT'.
//   It counts all Payment transactions (incoming and outgoing).
//   Additionally, the hook owner can manually update the counter via an Invoke transaction.
//
// Parameters:-
//   - 'CNT' (8 bytes): The new value to set for the counter.
//
// Usage:- 
//   - The hook triggers on Payment transactions, incrementing the counter.
//   - To manually update the counter: Send an Invoke transaction with 'CNT' parameter (only hook owner).
//
// Accepts:-
//   - Payment transactions (and increments counter).
//   - Invoke transactions from the hook owner with 'CNT' parameter (updates counter).
//   - Other transaction types (accepts without action).
//
// Rejects:-
//   - Invoke transactions not from the hook owner.
//   - Invoke transactions without valid CNT parameter.
//
//**************************************************************

#include "hookapi.h"
#include <stdint.h>

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(uint32_t reserved) {

    TRACESTR("BSU :: Basic State Update :: called");

    uint8_t hook_acct[20];
    hook_account(hook_acct, 20);

    uint8_t otx_acc[20];
    otxn_field(otx_acc, 20, sfAccount);

    int64_t tt = otxn_type();

    if (tt == 99) { // Invoke
        if (!BUFFER_EQUAL_20(hook_acct, otx_acc)) {
            NOPE("BSU :: Error :: Only hook owner can invoke this hook");
        }

        uint8_t cnt_buf[8];
        uint8_t cnt_param[3] = {'C', 'N', 'T'};
        int64_t cnt_len = otxn_param(SBUF(cnt_buf), SBUF(cnt_param));

        if (cnt_len != 8) {
            NOPE("BSU :: Error :: CNT parameter must be 8 bytes");
        }

        uint64_t new_count = UINT64_FROM_BUF(cnt_buf);

        if (state_set(SBUF(cnt_buf), SBUF(cnt_param)) < 0) {
            NOPE("BSU :: Error :: Could not update counter state on Invoke");
        }

        TRACESTR("BasicStateUpdate: Counter manually updated");

        DONE("Success: Counter manually updated via Invoke");

    } else if (tt == ttPAYMENT) {

        // Increment counter on Payment transactions
        uint8_t count_buf[8];
        uint8_t count_key[3] = {'C', 'N', 'T'};
        uint64_t count = 0;
        if (state(SBUF(count_buf), SBUF(count_key)) >= 0) {
            count = UINT64_FROM_BUF(count_buf);
        }

        count++;
        UINT64_TO_BUF(count_buf, count);

        if (state_set(SBUF(count_buf), SBUF(count_key)) < 0) {
            NOPE("BSU :: Error :: Could not update counter state on Payment");
        }

        TRACEVAR(count);

        DONE("BSU :: Success :: Counter incremented for Payment transaction");
        
    } else {

        DONE("BSU: Transaction type not handled, accepting");
    }

    _g(1,1); // Guard
    return 0;
}
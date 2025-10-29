//**************************************************************
// Xahau Hook 101 Example ~ Basic State Count Increase Hook
// Author: @handy_4ndy
//
// Description:
//   This hook increments a counter in state each time a Payment transaction is processed on the hook account.
//   The counter is stored in hook state under the key 'CNT'.
//   It counts all Payment transactions (incoming and outgoing).
//   Once the counter reaches 100, further Payment transactions are rejected.
//
// Accepts:-
//   - All non-Payment transactions.
//   - Payment transactions while the counter is below 100.
//
// Rejects:-
//   - Payment transactions when the counter is 100 or above.
//
//**************************************************************

#include "hookapi.h"
#include <stdint.h>

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(uint32_t reserved) {

    TRACESTR("BSC :: Basic State Counter :: called");

    int64_t tt = otxn_type();
    if (tt != ttPAYMENT) {
        DONE("BSC :: Accepted :: Not a Payment transaction, accepting");
    }

    // Retrieve current count from state
    uint8_t count_buf[8];
    uint8_t count_key[3] = {'C', 'N', 'T'};
    uint64_t count = 0;
    if (state(SBUF(count_buf), SBUF(count_key)) >= 0) {
        count = UINT64_FROM_BUF(count_buf);
    }

    // Check if limit reached
    if (count >= 100) {
        NOPE("Error: Counter reached 100, execution limit reached");
    }

    // Increment the counter
    count++;
    UINT64_TO_BUF(count_buf, count);

    // Store the new count in state
    if (state_set(SBUF(count_buf), SBUF(count_key)) < 0) {
        NOPE("BSC :: Error :: Could not update counter state");
    }

    

    TRAVCEVAR(count);

    DONE("BSC :: Accepted :: Counter incremented for Payment transaction");

    _g(1,1); // Guard
    return 0;
}
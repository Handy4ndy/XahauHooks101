//**************************************************************
// Xahau Hook 101 Example ~ Basic State Execution Limit Hook
// Author: @handy_4ndy
//
// Description:
//   This hook enforces an execution limit by decrementing a counter in state each time a Payment transaction is processed on the hook account.
//   The counter starts at a hardcoded value of 5 and decreases by 1 for each Payment transaction.
//   Once the counter reaches 0, further Payment transactions are rejected.
//
// Accepts:-
//   - All non-Payment transactions.
//   - Payment transactions while the counter is above 0.
//
// Rejects:-
//   - Payment transactions when the counter is 0 or below.
//
//**************************************************************

#include "hookapi.h"
#include <stdint.h>

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(uint32_t reserved) {
    GUARD(3);

    TRACESTR("BasicStateLimit: called");

    int64_t tt = otxn_type();
    if (tt != ttPAYMENT) {
        DONE("Info: Not a Payment transaction, accepting");
    }

    // Retrieve current count from state, default to 5 if not set
    uint8_t count_buf[8];
    uint8_t count_key[3] = {'C', 'N', 'T'};
    uint64_t count = 5;
    if (state(SBUF(count_buf), SBUF(count_key)) >= 0) {
        count = UINT64_FROM_BUF(count_buf);
    }

    // Check if limit reached
    if (count <= 0) {
        NOPE("Error: Execution limit reached");
    }

    // Decrement the counter
    count--;
    UINT64_TO_BUF(count_buf, count);

    // Store the new count in state
    if (state_set(SBUF(count_buf), SBUF(count_key)) < 0) {
        NOPE("Error: Could not update counter state");
    }

    TRACEVAR(count);

    DONE("Success: Counter decremented for Payment transaction");

    // Final guard
    GUARD(1);
    return 0;
}
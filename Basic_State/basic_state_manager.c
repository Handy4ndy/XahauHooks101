//**************************************************************
// Xahau Hook 101 Example ~ Basic State Manager Hook
// Author: @handy_4ndy
//
// Description:
//   This hook allows the owner to add, update, or delete state entries on-chain.
//   Only the hook owner can invoke this hook.
//   A counter tracks the number of entries.
//
// Parameters:-
//   'VAL' (bytes): The value to add (new entry) or update (if 'UPD' is also provided).
//   'UPD' (8 bytes): The key to update (must exist; replaces with 'VAL').
//   'DEL' (8 bytes): The key to delete.
//
// Usage:-
//   - To add: Send Invoke with 'VAL' parameter.
//   - To update: Send Invoke with 'UPD' (key) and 'VAL' (new value).
//   - To delete: Send Invoke with 'DEL' (key).
//
// Accepts:-
//   - Invoke transactions from the hook owner with 'VAL' parameter to add a new entry.
//   - Invoke transactions from the hook owner with 'UPD' and 'VAL' parameters to update an existing entry.
//   - Invoke transactions from the hook owner with 'DEL' parameter to delete an entry.
//
// Rejects:-
//   - Transactions not from the hook owner.
//   - Non-Invoke transactions.
//   - Invoke transactions without valid parameters.
//
//**************************************************************

#include "hookapi.h"
#include <stdint.h>

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(uint32_t reserved) {
    GUARD(5);

    TRACESTR("BSM :: Basic State Manager :: called");

    uint8_t hook_acct[20];
    hook_account(hook_acct, 20);

    uint8_t otx_acc[20];
    otxn_field(otx_acc, 20, sfAccount);

    if (!BUFFER_EQUAL_20(hook_acct, otx_acc)) {
        NOPE("BSM :: Error :: Only hook owner can invoke this hook");
    }

    int64_t tt = otxn_type();
    if (tt != 99) {
        NOPE("BSM :: Error :: Transaction must be an Invoke");
    }

    uint8_t val_buf[1024];
    uint8_t val_key[3] = {'V', 'A', 'L'};
    int64_t val_len = otxn_param(SBUF(val_buf), SBUF(val_key));

    uint8_t upd_buf[8];
    uint8_t upd_key[3] = {'U', 'P', 'D'};
    int64_t upd_len = otxn_param(SBUF(upd_buf), SBUF(upd_key));

    uint8_t del_buf[8];
    uint8_t del_key[3] = {'D', 'E', 'L'};
    int64_t del_len = otxn_param(SBUF(del_buf), SBUF(del_key));

    uint8_t count_buf[8];
    uint8_t count_key[3] = {'C', 'N', 'T'};
    uint64_t count = 0;
    if (state(SBUF(count_buf), SBUF(count_key)) >= 0) {
        count = UINT64_FROM_BUF(count_buf);
    }

    if (upd_len == 8 && val_len > 0) {
        // Update: replace value for existing key
        if (state_set(val_buf, val_len, upd_buf, 8) < 0) {
            NOPE("BSM :: Error :: Could not update state");
        }
        DONE("BSM :: Success :: State updated");
    }

    if (val_len > 0 && upd_len != 8) {
        // Add: new entry
        uint8_t val_num_buf[8];
        UINT64_TO_BUF(val_num_buf, count + 1);

        if (state_set(val_buf, val_len, val_num_buf, 8) < 0) {
            NOPE("BSM :: Error :: Could not set state");
        }

        count++;
        UINT64_TO_BUF(count_buf, count);
        if (state_set(SBUF(count_buf), SBUF(count_key)) < 0) {
            NOPE("BSM :: Error :: Could not update count state");
        }

        DONE("BSM :: Success :: VAL added");
    }

    if (del_len == 8) {
        // Delete
        if (state_set(0, 0, del_buf, 8) < 0) {
            NOPE("BSM :: Error :: Could not delete state");
        }

        DONE("BSM :: Success :: State deleted");
    }

    NOPE("BSM :: Error :: No valid operation specified");

    // Final guard
    GUARD(1);
    return 0;
}
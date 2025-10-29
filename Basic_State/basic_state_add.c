//**************************************************************
// Xahau Hook 101 Example ~ Basic State Add Hook 
// Author: @handy_4ndy
//
// Description:
//   This hook allows the owner to add custom state on-chain.
//   Only the hook owner can invoke this hook.
//
// Parameters:-
//   'KEY' (bytes): The custom key for the state.
//   'VAL' (bytes): The value to store for the key.
//
// Usage:-
//   - To add state: Send an Invoke transaction with 'KEY' and 'VAL' parameters.
//   - Only the hook owner can invoke this hook.
//   - Create hex strings. (https://transia-rnd.github.io/xrpl-hex-visualizer/)
//
//**************************************************************

#include "hookapi.h"
#include <stdint.h>

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

#define GUARD(maxiter) _g(__LINE__, (maxiter)+1)

int64_t hook(uint32_t reserved) {

    TRACESTR("BasicStateAdd: called");

    uint8_t hook_acct[20];
    hook_account(hook_acct, 20);

    uint8_t otx_acc[20];
    otxn_field(otx_acc, 20, sfAccount);

    if (!BUFFER_EQUAL_20(hook_acct, otx_acc)) {
        NOPE("Error: Only hook owner can invoke this hook");
    }

    int64_t tt = otxn_type();
    if (tt != 99) {
        NOPE("Error: Transaction must be an Invoke");
    }

    uint8_t key_buf[256];
    uint8_t key_param[3] = {'K', 'E', 'Y'};
    int64_t key_len = otxn_param(SBUF(key_buf), SBUF(key_param));

    uint8_t val_buf[1024];
    uint8_t val_param[3] = {'V', 'A', 'L'};
    int64_t val_len = otxn_param(SBUF(val_buf), SBUF(val_param));

    if (key_len <= 0 || val_len <= 0) {
        NOPE("Error: Both KEY and VAL parameters are required");
    }

    if (state_set(val_buf, val_len, key_buf, key_len) < 0) {
        NOPE("Error: Could not set state");
    }

    DONE("Success: State added");


    _g(1, 1);  // Guard
    return 0;
}
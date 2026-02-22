//**************************************************************
// Xahau Hook 101 Example ~ Basic State Toggle Hook
// Author: @Handy_4ndy
//
// Description:
//   Demonstrates a simple on/off toggle mechanism using hook state. The toggle controls whether the hook processes payments or skips them. When disabled (0), payments are accepted without action. When enabled (1), payments trigger a trace message.
//
// Triggers:
//   ttINVOKE (to set toggle)
//   ttPAYMENT (behavior depends on toggle state)
//
// Parameters:
//   'TGL' (1 byte): Set to 0 to disable, 1 to enable the hook.
//
// Usage:
//   - Send an Invoke transaction with 'TGL' parameter to enable/disable.
//   - Only the hook owner can change the toggle.
//   - Payments are always accepted, but behavior depends on toggle state.
//
// Accepts:
//   - All Invoke transactions from owner with valid 'TGL' parameter.
//   - All Payment transactions (with or without action based on toggle).
//
// Rejects:
//   - Invoke transactions not from the hook owner.
//   - Invoke transactions with invalid 'TGL' parameter.
//**************************************************************

#include "hookapi.h"

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)

int64_t hook(uint32_t reserved) {

    TRACESTR("BST :: Basic State Toggle :: Called");

    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    int64_t tt = otxn_type();

    uint8_t toggle_key[3] = {'T','G','L'};

    if (tt == 99) { // Invoke
        if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)) {
            NOPE("BST :: Error :: Only hook owner can toggle");
        }

        uint8_t toggle_param[1];
        int8_t toggle_len = otxn_param(SBUF(toggle_param), SBUF(toggle_key));

        if (toggle_len != 1) {
            NOPE("BST :: Error :: TGL parameter must be 1 byte");
        }

        if (toggle_param[0] > 1) {
            NOPE("BST :: Error :: TGL must be 0 or 1");
        }

        if (state_set(SBUF(toggle_param), SBUF(toggle_key)) < 0) {
            NOPE("BST :: Error :: Could not set toggle state");
        }

        TRACESTR("BST :: Toggle updated");
        DONE("BST :: Success :: Toggle updated");

    } else if (tt == ttPAYMENT) {

        uint8_t toggle_state[1];
        int64_t state_result = state(SBUF(toggle_state), SBUF(toggle_key));

        if (state_result < 0 || toggle_state[0] == 0) {
            DONE("BST :: Hook disabled, skipping");
        } else {
            TRACESTR("BST :: Hook is enabled");
            DONE("BST :: Success :: Hook enabled");
        }

    } else {

        DONE("BST :: Transaction type not handled, accepting");
    }

    _g(1,1); // Guard
    return 0;
}

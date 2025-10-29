//**************************************************************
// Xahau Hook 101 Example ~ Accept Outgoing Only Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts outgoing payments.
//   Rejects incoming payments.
//
// Accepts:-
//   - Outgoing payments.
//
// Rejects:-
//   - Incoming payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AOP :: Accept Outgoing Payment :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Reject all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc))
        rollback(SBUF("AOP :: Incoming payment rejected."), __LINE__);

    // Accept all outgoing payments
    accept(SBUF("AOP :: Outgoing payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
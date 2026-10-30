//**************************************************************
// Xahau Hook 101 Example ~ Accept Incoming Only Hook
// Author: @Handy_4ndy
//
// Description:
//   Accepts incoming payments. Rejects outgoing payments.
//
// Triggers:
//   ttPAYMENT (incoming and outgoing payments)
//
// Parameters:
//   None
//
// Usage:
//   - Deploy the hook to your account.
//   - Incoming payments are accepted; outgoing payments are rejected.
//
// Accepts:
//   - Incoming payments.
//
// Rejects:
//   - Outgoing payments.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AIP :: Accept Incoming Payment :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Reject all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc))
        rollback(SBUF("AIP :: Outgoing payment rejected."), __LINE__);

    // Accept all Incoming payments
    accept(SBUF("AIP :: Incoming payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Reject All Payments
// Author: @Handy_4ndy
//
// Description:
//   Rejects all incoming and outgoing payments to the Hook account, regardless of amount or asset type.
//
// Triggers:
//   ttPAYMENT (incoming and outgoing payments)
//
// Parameters:
//   None (all logic is hardcoded)
//
// Usage:
//   - Deploy to a Xahau account using Xahau Hooks Builder.
//   - Set trigger for ttPAYMENT.
//   - Any incoming or outgoing payment will be rejected.
//
// Accepts:
//   None (all payments are rejected).
//
// Rejects:
//   - All incoming payments.
//   - All outgoing payments.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("RIP :: Reject Incoming Payment :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // reject all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc))
        rollback(SBUF("RIP :: Outgoing payment rejected."), __LINE__);

    // Reject all Incoming payments
    rollback(SBUF("RIP :: Incoming payment rejected."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
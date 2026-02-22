//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing XAH
// Author: @Handy_4ndy
//
// Description:
//   Rejects incoming payments and outgoing XAH payments. Accepts outgoing IOU payments.
//
// Triggers:
//   ttPAYMENT (outgoing payments only)
//
// Parameters:
//   None (all logic is hardcoded)
//
// Usage:
//   - Deploy to a Xahau account using Xahau Hooks Builder.
//   - Set trigger for ttPAYMENT.
//   - Outgoing IOU payments will be accepted; others rejected.
//
// Accepts:
//   - Outgoing IOU payments.
//
// Rejects:
//   - Incoming payments.
//   - Outgoing XAH payments.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROI :: Reject Outgoing IOU:: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Reject all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("ROI :: Rejected :: Incoming payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];

    // Reject outgoing XAH payments
    if (otxn_field(SBUF(amount), sfAmount) == 8)
        rollback(SBUF("ROI :: Rejected :: Outgoing XAH payment."), __LINE__);

    // Accept outgoing IOU payments
    accept(SBUF("ROI :: Accepted :: Outgoing IOU payment."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
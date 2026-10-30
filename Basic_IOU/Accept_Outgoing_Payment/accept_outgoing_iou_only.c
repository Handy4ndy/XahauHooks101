//**************************************************************
// Xahau Hook 101 Example ~ Accept Outgoing IOU Only
// Author: @Handy_4ndy
//
// Description:
//   Accepts only outgoing IOU payments; rejects incoming payments and outgoing XAH payments.
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
//   - Only outgoing IOU payments will be accepted.
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

    TRACESTR("AOI :: Accept Outgoing IOU :: Called.");

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
        rollback(SBUF("AOI :: Incoming payment rejected."), __LINE__);

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];
    
    // Reject outgoing XAH payments
    if (otxn_field(SBUF(amount), sfAmount) != 48)
        rollback(SBUF("AOI :: Outgoing XAH payment rejected."), __LINE__);

    // Accept outgoing IOU payments
    accept(SBUF("AOI :: Outgoing IOU payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
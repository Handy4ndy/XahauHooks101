//**************************************************************
// Xahau Hook 101 Example ~ Accept Outgoing IOU
// Author: @Handy_4ndy
//
// Description:
//   Accepts outgoing IOU payments and incoming payments; rejects outgoing XAH payments.
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
//   - Outgoing IOU payments and incoming payments will be accepted.
//
// Accepts:
//   - Outgoing IOU payments.
//   - Incoming payments.
//
// Rejects:
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

    // Accept all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AOI :: Accepted :: Incoming payment."), __LINE__);
    }

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
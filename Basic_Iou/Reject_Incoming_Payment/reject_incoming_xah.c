//**************************************************************
// Xahau Hook 101 Example ~ Reject Incoming XAH Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects outgoing payments and incoming XAH payments.
//   Accepts incoming IOU payments.
//
// Accepts:-
//   - Incoming IOU payments.
//
// Rejects:-
//   - Outgoing payments.
//   - Incoming XAH payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("RII :: Reject Incoming XAH :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Reject all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("RII :: Rejected :: Outgoing payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];

    // Reject incoming XAH payments
    if (otxn_field(SBUF(amount), sfAmount) == 8)
        rollback(SBUF("RII :: Rejected :: Incoming XAH payment."), __LINE__);

    // Accept incoming IOU payments
    accept(SBUF("RII :: Accepted :: Incoming IOU payment."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
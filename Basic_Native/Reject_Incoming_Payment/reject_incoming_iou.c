//**************************************************************
// Xahau Hook 101 Example ~ Reject Incoming IOU Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects outgoing payments and incoming IOU payments.
//   Accepts incoming XAH payments.
//
// Accepts:-
//   - Incoming XAH payments.
//
// Rejects:-
//   - Outgoing payments.
//   - Incoming IOU payments.
//
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

    // Reject all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("RIP Hook :: Rejected :: Outgoing payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];

    // Reject incoming IOU payments
    if (otxn_field(SBUF(amount), sfAmount) != 8)
        rollback(SBUF("RIP :: Rejected :: Incoming IOU payment."), __LINE__);

    // Accept incoming XAH payments
    accept(SBUF("RIP :: Accepted :: Incoming XAH payment."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
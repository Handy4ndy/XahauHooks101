//**************************************************************
// Xahau Hook 101 Example ~ Accept Incoming XAH Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments and outgoing payments.
//   Rejects incoming IOU payments.
//
// Accepts:-
//   - Incoming XAH payments.
//   - Outgoing payments.
//
// Rejects:-
//   - Incoming IOU payments.
//
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

    // Accept all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AIP : Accepted :: Outgoing payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];
    
    // Reject incoming IOU payments
    if (otxn_field(SBUF(amount), sfAmount) != 8)
        rollback(SBUF("AIP :: Incoming IOU payment rejected."), __LINE__);

    // Accept incoming XAH payments 
    accept(SBUF("AIP :: Incoming XAH payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
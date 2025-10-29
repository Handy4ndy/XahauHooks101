/* 
 * Xahau-Hooks-101 ~ accept_outgoing_xah.c
 *
 * Accepts:- 
 * Outgoing XAH payments.
 * Incoming payments.
 * 
 * Rejects:-
 * Outgoing IOU payments.
 * 
 */

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

    // Accept all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AOP :: Accepted :: Incoming payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount[48];
    
    // Reject outgoing IOU payments
    if (otxn_field(SBUF(amount), sfAmount) != 8)
        rollback(SBUF("AOP :: Outgoing IOU payment rejected."), __LINE__);

    // Accept outgoing XAH payments
    accept(SBUF("AOP :: Outgoing XAH payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
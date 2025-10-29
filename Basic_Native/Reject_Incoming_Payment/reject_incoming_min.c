//**************************************************************
// Xahau Hook 101 Example ~ Reject Incoming Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects outgoing payments, incoming XAH payments below the minimum threshold (1 XAH), and incoming IOU payments.
//   Accepts incoming XAH payments above the minimum threshold.
//
// Accepts:-
//   - Incoming XAH payments above the minimum threshold.
//
// Rejects:-
//   - Outgoing payments.
//   - Incoming XAH payments below the minimum threshold (1 XAH).
//   - Incoming IOU payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("RIP :: Reject Incoming Payment :: Called.");

    // Define the minimum allowed amount in XAH
    uint64_t min_amount = 1; // 1 XAH
    uint64_t min_drops = min_amount * 1000000; // Convert to drops

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
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        rollback(SBUF("RIP :: Rejected :: Incoming IOU payment."), __LINE__);

    if (otxn_drops < min_drops)
        rollback(SBUF("RIP :: Rejected :: Incoming payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("RIP :: Accepted :: Incoming amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
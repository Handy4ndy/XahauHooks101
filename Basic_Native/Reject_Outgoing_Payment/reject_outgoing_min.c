//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects incoming payments, outgoing XAH payments below the minimum threshold (1 XAH), and outgoing IOU payments.
//   Accepts outgoing XAH payments above the minimum threshold.
//
// Accepts:-
//   - Outgoing XAH payments above the minimum threshold.
//
// Rejects:-
//   - Incoming payments.
//   - Outgoing XAH payments below the minimum threshold (1 XAH).
//   - Outgoing IOU payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROP :: Reject Outgoing Payment :: Called.");

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

    // Reject all Incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("ROP :: Rejected :: Incoming payment."), __LINE__);
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
        rollback(SBUF("ROP :: Rejected :: Outgoing IOU payment."), __LINE__);

    if (otxn_drops < min_drops)
        rollback(SBUF("ROP :: Rejected :: Outgoing payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("ROP :: Accepted :: Outgoing amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
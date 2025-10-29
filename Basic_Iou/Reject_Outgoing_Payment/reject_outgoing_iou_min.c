//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing IOU Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects incoming payments, outgoing IOU payments below the minimum threshold (1 IOU), and outgoing XAH payments.
//   Accepts outgoing IOU payments above the minimum threshold.
//
// Accepts:-
//   - Outgoing IOU payments above the minimum threshold.
//
// Rejects:-
//   - Incoming payments.
//   - Outgoing IOU payments below the minimum threshold (1 IOU).
//   - Outgoing XAH payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROI :: Reject Outgoing IOU :: Called.");

    // Define the minimum allowed amount in IOU
    uint64_t min_amount = 1; // 1 IOU
    uint64_t min_drops = min_amount * 1000000;

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
    uint8_t amount_buffer[48];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    
    // Reject if not IOU
    if (amount_len != 48)
        rollback(SBUF("ROI :: Rejected :: Outgoing XAH payment."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    // Check if below minimum
    if (amount_drops < min_drops)
        rollback(SBUF("ROI :: Rejected :: Outgoing IOU payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("ROI :: Accepted :: Outgoing IOU amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Accept Outgoing IOU Min Hook
// Author: @Handy_4ndy
//
// Description:
//   This hook accepts outgoing IOU payments when the amount is above the minimum threshold (1 IOU), incoming payments, and outgoing XAH payments.
//   Rejects payments below the minimum amount.
//
// Accepts:-
//   - Outgoing IOU payments when the amount is above the minimum threshold (1 IOU).
//   - Incoming payments.
//   - Outgoing XAH payments.
//
// Rejects:-
//   - Payments below the minimum amount.
//
//**************************************************************


#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AOI :: Accept Outgoing IOU :: Called.");

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

    // Accept all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AOI :: Accepted :: Incoming payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[48];

    // Retrieve the amount field from the originating transaction
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);

    // Accept xah payments
    if (amount_len != 48)
        accept(SBUF("AOI :: Outgoing XAH accepted."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    // Check if below minimum
    if (amount_drops < min_drops)
        rollback(SBUF("AOI :: Outgoing payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("AOI :: Outgoing IOU amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
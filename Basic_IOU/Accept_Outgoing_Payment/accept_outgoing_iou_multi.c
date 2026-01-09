//**************************************************************
// Xahau Hook 101 Example ~ Accept Outgoing IOU Multi Hook
// Author: @Handy_4ndy
//
// Description:
//   This hook accepts outgoing IOU payments that are a multiple of the fixed IOU amount (10 IOU), incoming payments, and outgoing XAH payments.
//   Rejects if the payment is less than the fixed amount or not a multiple.
//
// Accepts:-
//   - Outgoing IOU payments that are a multiple of the fixed IOU amount (10 IOU).
//   - Incoming payments.
//   - Outgoing XAH payments.
//
// Rejects:-
//   - If the payment is less than the fixed amount.
//   - If not a multiple.
//
//**************************************************************


#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AOI :: Accept Outgoing IOU:: Called.");

    // Define the fixed cost in IOU and convert to drops (1 IOU unit = 1,000,000 drops for comparison)
    uint64_t cost_iou = 10;
    uint64_t cost_drops = cost_iou * 1000000;

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
    
    // Accept Xah payments
    if (amount_len != 48)
        accept(SBUF("AOI :: Outgoing XAH accepted."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(cost_iou);
    TRACEVAR(amount_int);

    // If the payment exactly matches the cost, accept immediately
    if (amount_drops == cost_drops)
        accept(SBUF("AOI :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = amount_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("AOI :: Payment is less than the required amount!"), __LINE__);

    TRACEVAR(multi_count);
    
    // If the payment is not an exact multiple, reject
    if (amount_drops % cost_drops != 0)
        rollback(SBUF("AOI :: Payment amount must be a multiple of the required!"), __LINE__);

    // Accept the payment as it meets all criteria
    accept(SBUF("AOI :: Outgoing IOU payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
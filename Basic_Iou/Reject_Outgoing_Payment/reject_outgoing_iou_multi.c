//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing IOU Multi Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects incoming payments, outgoing IOU payments that are not multiples of the fixed amount (10 IOU), and outgoing XAH payments.
//   Accepts outgoing IOU payments that are multiples of the fixed amount.
//
// Accepts:-
//   - Outgoing IOU payments that are multiples of the fixed amount.
//
// Rejects:-
//   - Incoming payments.
//   - Outgoing IOU payments that are not multiples of the fixed amount (10 IOU).
//   - Outgoing XAH payments.
//
//**************************************************************


#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROI :: Reject Outgoing IOU :: Called.");

    // Define the fixed cost in IOU and convert to drops (1 IOU unit = 1,000,000 drops for comparison)
    uint64_t cost_iou = 10;
    uint64_t cost_drops = cost_iou * 100000;

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

    TRACEVAR(cost_iou);
    TRACEVAR(amount_int);

    // If the payment exactly matches the cost, accept immediately
    if (amount_drops == cost_drops)
        accept(SBUF("ROI :: Accepted :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = amount_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("ROI :: Rejected :: Payment is less than the required amount!"), __LINE__);

    // If the payment is not an exact multiple, reject
    if (amount_drops % cost_drops != 0)
        rollback(SBUF("ROI :: Rejected :: Payment amount must be a multiple of the required!"), __LINE__);

    // Accept the payment as it meets all criteria
    accept(SBUF("ROI :: Accepted :: Outgoing IOU payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
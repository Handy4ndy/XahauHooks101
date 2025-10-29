//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing Multi Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects incoming payments, outgoing XAH payments that are not multiples of the fixed amount (10 XAH), and outgoing IOU payments.
//   Accepts outgoing XAH payments that are multiples of the fixed amount.
//
// Accepts:-
//   - Outgoing XAH payments that are multiples of the fixed amount.
//
// Rejects:-
//   - Incoming payments.
//   - Outgoing XAH payments that are not multiples of the fixed amount (10 XAH).
//   - Outgoing IOU payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROP :: Reject Outgoing Payment :: Called.");

    // Define the fixed cost in XAH and convert to drops (1 XAH = 1,000,000 drops)
    uint64_t cost_xah = 10;
    uint64_t cost_drops = cost_xah * 1000000;

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
        rollback(SBUF("ROP :: Rejected :: Incoming payment."), __LINE__);
    }
 
     // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(cost_xah);
    TRACEVAR(amount_int);

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        rollback(SBUF("ROP :: Rejected :: Outgoing IOU payment."), __LINE__);

    // If the payment exactly matches the cost, accept immediately
    if (otxn_drops == cost_drops)
        accept(SBUF("ROP :: Accepted :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = otxn_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("ROP :: Rejected :: Payment is less than the required amount!"), __LINE__);

    // If the payment is not an exact multiple, reject
    if (otxn_drops % cost_drops != 0)
        rollback(SBUF("ROP :: Rejected :: Payment amount must be a multiple of the required!"), __LINE__);

    // Accept the payment as it meets all criteria
    accept(SBUF("ROP :: Accepted :: Outgoing XAH payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
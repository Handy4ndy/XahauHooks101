//**************************************************************
// Xahau Hook 101 Example ~ Accept Incoming Multi Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments that are a multiple of the fixed XAH amount (10 XAH), outgoing payments, and incoming IOU payments.
//   Rejects if the payment is less than the fixed amount or not a multiple.
//
// Accepts:-
//   - Incoming XAH payments that are a multiple of the fixed XAH amount.
//   - Outgoing payments.
//   - Incoming IOU payments.
//
// Rejects:-
//   - If the payment is less than the fixed amount.
//   - If not a multiple.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {
    
    TRACESTR("AIP :: Accept Incoming Payment :: Called.");

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

    // Accept all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AIP :: Accepted :: Outgoing payment."), __LINE__);
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
        accept(SBUF("AIP :: Incoming IOU Accepted."), __LINE__);

    // If the payment exactly matches the cost, accept immediately
    if (otxn_drops == cost_drops)
        accept(SBUF("AIP :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = otxn_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("AIP :: Payment is less than the required amount!"), __LINE__);

    TRACEVAR(multi_count);
    
    // If the payment is not an exact multiple, reject
    if (otxn_drops % cost_drops != 0)
        rollback(SBUF("AIP :: Payment amount must be a multiple of the required!"), __LINE__);

    // Accept the payment as it meets all criteria
    accept(SBUF("AIP :: Incoming XAH payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
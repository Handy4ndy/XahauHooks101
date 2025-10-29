//**************************************************************
// Xahau Hook 101 Example ~ Install Parameter Multi Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments that are a multiple of the fixed XAH amount set by install parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects if the payment is less than the fixed amount or not a multiple.
//
// Parameters:-
//   'MULTI' (8 bytes): The fixed amount in XAH (uint64_t) that payments must be a multiple of.
//
// Usage:-
//   - Set 'MULTI' to the required XAH amount during hook installation.
//   - Incoming XAH payments must be multiples of this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that are a multiple of the fixed amount.
//
// Rejects:-
//   - Incoming XAH payments that are less than the fixed amount.
//   - Incoming XAH payments that are not a multiple of the fixed amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Install Parameter :: Called.");

    // Get the fixed amount from install parameter
    uint8_t multi_buf[8];
    if (hook_param(SBUF(multi_buf), "MULTI", 5) != 8)
        rollback(SBUF("BIP :: Error :: MULTI parameter not set or invalid."), __LINE__);

    uint64_t cost_xah = UINT64_FROM_BUF(multi_buf);
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
        accept(SBUF("BIP :: Accepted :: Outgoing payment."), __LINE__);
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
        accept(SBUF("BIP :: Accepted :: Incoming IOU Accepted."), __LINE__);

    // If the payment exactly matches the cost, accept immediately
    if (otxn_drops == cost_drops)
        accept(SBUF("BIP :: Accepted :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = otxn_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("BIP :: Rejected :: Payment is less than the required amount!"), __LINE__);
    
    // If the payment is not an exact multiple, reject
    if (otxn_drops % cost_drops != 0)
        rollback(SBUF("BIP :: Rejected :: Payment amount must be a multiple of the required!"), __LINE__);

    TRACEVAR(multi_count);
    
    // Accept the payment as it meets all criteria
    accept(SBUF("BIP :: Accepted :: Incoming XAH payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Invoke Parameter Multi Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments that are a multiple of the fixed XAH amount set by Invoke parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects if the payment is less than the fixed amount or not a multiple.
//
// Parameters:-
//   'MULTI' (8 bytes): The fixed amount in XAH (uint64_t) to set in state.
//
// Usage:-
//   - Invoke from hook owner with 'MULTI' to set the amount.
//   - Incoming XAH payments must be multiples of this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Invoke transactions from the hook owner with 'MULTI' parameter to set the amount.
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that are a multiple of the fixed amount.
//
// Rejects:-
//   - Invoke transactions not from the hook owner.
//   - Invoke transactions without valid MULTI.
//   - Incoming XAH payments that are less than the fixed amount.
//   - Incoming XAH payments that are not a multiple of the fixed amount.
//   - Payment transactions if MULTI amount not set in state.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {
    
    TRACESTR("BIP :: Basic Invoke Parameter :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    // Get transaction type
    int64_t tt = otxn_type();

    if (tt == 99) { // Invoke
        if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
            rollback(SBUF("BIP :: Rejected :: Only hook owner can invoke."), __LINE__);
        }

        // Get MULTI parameter from Invoke
        uint8_t multi_buf[8];
        uint8_t multi_key[5] = {'M', 'U', 'L', 'T', 'I'};
        int64_t multi_len = otxn_param(SBUF(multi_buf), SBUF(multi_key));

        if (multi_len != 8)
            rollback(SBUF("BIP :: Error :: MULTI parameter not set or invalid."), __LINE__);

        // Set the multi amount in state
        if (state_set(SBUF(multi_buf), SBUF(multi_key)) < 0)
            rollback(SBUF("BIP :: Error :: Could not set multi amount in state."), __LINE__);

        // Successfully set multi amount
        accept(SBUF("BIP :: Accepted :: Multi amount set in state."), __LINE__);
    }

    if (tt != ttPAYMENT) {
        accept(SBUF("BIP :: Accepted :: Non-Payment transaction."), __LINE__);
    }

    // For Payment transactions
    // Get the fixed amount from state
    uint8_t multi_buf[8];
    uint8_t multi_key[5] = {'M', 'U', 'L', 'T', 'I'};
    if (state(SBUF(multi_buf), SBUF(multi_key)) != 8)
        rollback(SBUF("BIP :: Error :: Multi amount not set in state."), __LINE__);

    // Convert fixed amount to drops
    uint64_t cost_xah = UINT64_FROM_BUF(multi_buf);
    uint64_t cost_drops = cost_xah * 1000000;

    TRACEHEX(hook_acc);
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
        accept(SBUF("BIP :: Incoming IOU Accepted."), __LINE__);

    // If the payment exactly matches the cost, accept immediately
    if (otxn_drops == cost_drops)
        accept(SBUF("BIP :: Payment matches the required amount."), __LINE__);

    // Calculate how many times the fixed amount fits into the payment
    uint64_t multi_count = otxn_drops / cost_drops;

    // If the payment is less than the fixed amount, reject
    if (multi_count == 0)
        rollback(SBUF("BIP :: Payment is less than the required amount!"), __LINE__);
    
    // If the payment is not an exact multiple, reject
    if (otxn_drops % cost_drops != 0)
        rollback(SBUF("BIP :: Payment amount must be a multiple of the required!"), __LINE__);

    TRACEVAR(multi_count);
    
    // Accept the payment as it meets all criteria
    accept(SBUF("BIP :: Incoming XAH payment is a multiple of the required amount."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
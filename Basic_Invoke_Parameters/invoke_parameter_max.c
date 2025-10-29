//**************************************************************
// Xahau Hook 101 Example ~ Invoke Parameter Max Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments when the amount is below the maximum threshold set by Invoke parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects incoming payments above the maximum amount.
//
// Parameters:-
//   'MAX' (8 bytes): The maximum allowed amount in XAH (uint64_t) to set in state.
//
// Usage:-
//   - Invoke from hook owner with 'MAX' to set the amount.
//   - Incoming XAH payments must be below this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Invoke transactions from the hook owner with 'MAX' parameter to set the amount.
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments below the maximum threshold.
//
// Rejects:-
//   - Invoke transactions not from the hook owner.
//   - Invoke transactions without valid MAX.
//   - Incoming XAH payments above the maximum amount.
//   - Payment transactions if MAX amount not set in state.
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

    int64_t tt = otxn_type();

    if (tt == 99) { // Invoke
        if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
            rollback(SBUF("BIP :: Error :: Only hook owner can invoke."), __LINE__);
        }

        uint8_t max_buf[8];
        uint8_t max_key[3] = {'M', 'A', 'X'};
        int64_t max_len = otxn_param(SBUF(max_buf), SBUF(max_key));

        if (max_len != 8)
            rollback(SBUF("BIP :: Error :: MAX parameter not set or invalid."), __LINE__);

        // Set the max amount in state
        if (state_set(SBUF(max_buf), SBUF(max_key)) < 0)
            rollback(SBUF("BIP :: Error :: Could not set max amount in state."), __LINE__);

        accept(SBUF("BIP :: Accepted :: Max amount set in state."), __LINE__);
    }

    if (tt != ttPAYMENT) {
        accept(SBUF("BIP :: Accepted :: Non-Payment transaction."), __LINE__);
    }

    // For Payment transactions
    // Get the maximum allowed amount from state
    uint8_t max_buf[8];
    uint8_t max_key[3] = {'M', 'A', 'X'};
    if (state(SBUF(max_buf), SBUF(max_key)) != 8)
        rollback(SBUF("BIP :: Error :: Max amount not set in state."), __LINE__);

    uint64_t max_amount = UINT64_FROM_BUF(max_buf);

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

    TRACEVAR(max_amount);
    TRACEVAR(amount_int);

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        accept(SBUF("BIP :: Accepted :: Incoming IOU Accepted."), __LINE__);

    // Convert drops to XAH and check if below maximum
    double xah_amount = (double)otxn_drops / 1000000.0;
    if (xah_amount > max_amount)
        rollback(SBUF("BIP :: Error :: Incoming payment above maximum amount."), __LINE__);

    // Accept the payment as it is below the maximum threshold
    accept(SBUF("BIP :: Accepted :: Incoming amount below the maximum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
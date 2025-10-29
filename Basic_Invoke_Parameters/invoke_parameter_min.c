//**************************************************************
// Xahau Hook 101 Example ~ Invoke Parameter Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments when the amount is above the minimum threshold set by Invoke parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects payments below the minimum amount.
//
// Parameters:-
//   'MIN' (8 bytes): The minimum allowed amount in XAH (uint64_t) to set in state.
//
// Usage:-
//   - Invoke from hook owner with 'MIN' to set the amount.
//   - Incoming XAH payments must be above this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Invoke transactions from the hook owner with 'MIN' parameter to set the amount.
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments above the minimum threshold.
//
// Rejects:-
//   - Invoke transactions not from the hook owner.
//   - Invoke transactions without valid MIN.
//   - Incoming XAH payments below the minimum amount.
//   - Payment transactions if MIN amount not set in state.
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

        uint8_t min_buf[8];
        uint8_t min_key[3] = {'M', 'I', 'N'};
        int64_t min_len = otxn_param(SBUF(min_buf), SBUF(min_key));

        if (min_len != 8)
            rollback(SBUF("BIP :: Error :: MIN parameter not set or invalid."), __LINE__);

        // Set the min amount in state
        if (state_set(SBUF(min_buf), SBUF(min_key)) < 0)
            rollback(SBUF("BIP :: Error :: Could not set min amount in state."), __LINE__);

        accept(SBUF("BIP :: Accepted :: Min amount set in state."), __LINE__);
    }

    if (tt != ttPAYMENT) {
        accept(SBUF("BIP :: Accepted :: Non-Payment transaction."), __LINE__);
    }

    // For Payment transactions
    // Get the minimum allowed amount from state
    uint8_t min_buf[8];
    uint8_t min_key[3] = {'M', 'I', 'N'};
    if (state(SBUF(min_buf), SBUF(min_key)) != 8)
        rollback(SBUF("BIP :: Error :: Min amount not set in state."), __LINE__);

    uint64_t min_amount = UINT64_FROM_BUF(min_buf);

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

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    if (amount_len != 8)
        accept(SBUF("BIP :: Accepted :: Incoming IOU Accepted."), __LINE__);

    // Convert drops to XAH and check if above minimum
    double xah_amount = (double)otxn_drops / 1000000.0;

    // Check if the amount is below the minimum threshold
    if (xah_amount < min_amount)
        rollback(SBUF("BIP :: Error :: Incoming payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("BIP :: Accepted :: Incoming amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
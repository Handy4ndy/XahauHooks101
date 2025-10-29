//**************************************************************
// Xahau Hook 101 Example ~ Install Parameter Max Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments when the amount is below the maximum threshold set by install parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects incoming payments above the maximum amount.
//
// Parameters:-
//   'MAX' (8 bytes): The maximum allowed amount in XAH (uint64_t).
//
// Usage:-
//   - Set 'MAX' to the required XAH amount during hook installation.
//   - Incoming XAH payments must be below this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments below the maximum threshold.
//
// Rejects:-
//   - Incoming XAH payments above the maximum amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Install Parameter :: Called.");
    
    // Get the maximum allowed amount from install parameter
    uint8_t max_buf[8];
    if (hook_param(SBUF(max_buf), "MAX", 3) != 8)
        rollback(SBUF("BIP :: Error :: MAX parameter not set or invalid."), __LINE__);

    uint64_t max_amount = UINT64_FROM_BUF(max_buf);

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

    TRACEVAR(max_amount);
    TRACEVAR(amount_int);

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        accept(SBUF("BIP :: Incoming IOU Accepted."), __LINE__);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);

    // Convert drops to XAH and check if below maximum
    double xah_amount = (double)otxn_drops / 1000000.0;
    if (xah_amount > max_amount)
        rollback(SBUF("BIP :: Incoming payment above maximum amount."), __LINE__);

    // Accept the payment as it is below the maximum threshold
    accept(SBUF("BIP :: Incoming amount below the maximum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
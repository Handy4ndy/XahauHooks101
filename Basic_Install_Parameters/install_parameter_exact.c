//**************************************************************
// Xahau Hook 101 Example ~ Install Parameter Exact Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments that match the exact amount set by install parameter.
//   Accepts outgoing payments and IOU payments.
//   Rejects if the payment amount doesn't match.
//
// Parameters:-
//   'EXACT' (8 bytes): The exact amount in XAH (uint64_t).
//
// Usage:-
//   - Set 'EXACT' to the required XAH amount during hook installation.
//   - Incoming XAH payments must match exactly; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match the exact amount set by install parameter.
//
// Rejects:-
//   - Incoming XAH payments that do not match the exact amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Install Parameter :: Called.");
    
    // Get the exact amount from install parameter
    uint8_t exact_buf[8];
    if (hook_param(SBUF(exact_buf), "EXACT", 5) != 8)
        rollback(SBUF("BIP :: Error :: EXACT parameter not set or invalid."), __LINE__);

    uint64_t exact_amount = UINT64_FROM_BUF(exact_buf);

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

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

    TRACEVAR(exact_amount);
    TRACEVAR(amount_int);

    // Convert drops to XAH for comparison
    double xah_amount = (double)otxn_drops / 1000000.0;

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        accept(SBUF("BIP :: Incoming IOU Accepted."), __LINE__);

    // Check if the payment amount exactly matches the required amount
    if (xah_amount != exact_amount)
        rollback(SBUF("BIP :: Incoming payment doesn't match the amount required."), __LINE__);

    // Accept the payment as it matches the required amount
    accept(SBUF("BIP :: Incoming payment matches the amount required."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
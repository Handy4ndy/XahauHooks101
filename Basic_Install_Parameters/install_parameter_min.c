//**************************************************************
// Xahau Hook 101 Example ~ Install Parameter Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments when the amount is above the minimum threshold set by install parameter.
//   Accepts outgoing payments and incoming IOU payments.
//   Rejects payments below the minimum amount.
//
// Parameters:-
//   'MIN' (8 bytes): The minimum allowed amount in XAH (uint64_t).
//
// Usage:-
//   - Set 'MIN' to the required XAH amount during hook installation.
//   - Incoming XAH payments must be above this amount; others are accepted or rejected accordingly.
//
// Accepts:-
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments above the minimum threshold.
//
// Rejects:-
//   - Incoming XAH payments below the minimum amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Install Parameter :: Called.");
    
    // Get the minimum allowed amount from install parameter
    uint8_t min_buf[8];
    if (hook_param(SBUF(min_buf), "MIN", 3) != 8)
        rollback(SBUF("BIP :: Error :: MIN parameter not set or invalid."), __LINE__);

    uint64_t min_amount = UINT64_FROM_BUF(min_buf);

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

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    if (amount_len != 8)
        accept(SBUF("BIP :: Incoming IOU Accepted."), __LINE__);

    // Convert drops to XAH and check if above minimum
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    double xah_amount = (double)otxn_drops / 1000000.0;

    // Check if the amount is below the minimum threshold
    if (xah_amount < min_amount)
        rollback(SBUF("BIP :: Incoming payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("BIP :: Incoming amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
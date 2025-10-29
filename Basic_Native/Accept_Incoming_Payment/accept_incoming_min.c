//**************************************************************
// Xahau Hook 101 Example ~ Accept Incoming Min Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts incoming XAH payments when the amount is above the minimum threshold (1 XAH), outgoing payments, and incoming IOU payments.
//   Rejects payments below the minimum amount.
//
// Accepts:-
//   - Incoming XAH payments when the amount is above the minimum threshold (1 XAH).
//   - Outgoing payments.
//   - Incoming IOU payments.
//
// Rejects:-
//   - Payments below the minimum amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AIP :: Accept Incoming Payment :: Called.");

    // Define the minimum allowed amount in XAH
    uint64_t min_amount = 1; // 1 XAH
    uint64_t min_drops = min_amount * 1000000; // Convert to drops

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

    TRACEVAR(min_amount);
    TRACEVAR(amount_int);

    if (amount_len != 8)
        accept(SBUF("AIP :: Incoming IOU Accepted."), __LINE__);

    // Check if the amount is below the minimum threshold
    if (otxn_drops < min_drops)
        rollback(SBUF("AIP :: Incoming payment is below minimum amount."), __LINE__);

    // Accept the payment as it is above the minimum threshold
    accept(SBUF("AIP :: Incoming amount above the minimum amount threshold."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
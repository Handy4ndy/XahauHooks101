//**************************************************************
// Xahau Hook 101 Example ~ Reject Incoming Exact Hook
// Author: @handy_4ndy
//
// Description:
//   This hook rejects outgoing payments, incoming XAH payments that exactly match the specified amount (10 XAH), and incoming IOU payments.
//   Accepts incoming XAH payments that do not exactly match the specified amount.
//
// Accepts:-
//   - Incoming XAH payments that do not exactly match the specified amount.
//
// Rejects:-
//   - Outgoing payments.
//   - Incoming XAH payments that exactly match the specified amount (10 XAH).
//   - Incoming IOU payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("RIP :: Reject Incoming Payment :: Called.");

    // Define the exact required amount in XAH
    uint64_t exact_amount = 10; // 10 XAH
    uint64_t exact_drops = exact_amount * 1000000; // Convert to drops

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Reject all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("RIP Hook :: Rejected :: Outgoing payment."), __LINE__);
    }

     // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exact_amount);
    TRACEVAR(amount_int);

    // Check if the payment is in XAH (native currency), amount_len == 8 for XAH
    if (amount_len != 8)
        rollback(SBUF("RIP :: Rejected :: Incoming IOU payment."), __LINE__);

    // Check if the payment exactly matches the required amount, reject if so
    if (otxn_drops == exact_drops)
        rollback(SBUF("RIP :: Rejected :: Incoming payment matches the rejection amount."), __LINE__);

    // Accept the payment as it does not match the required amount
    accept(SBUF("RIP :: Accepted :: Incoming payment does not match the amount required."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Reject Incoming IOU Exact Hook
// Author: @Handy_4ndy
//
// Description:
//   This hook rejects outgoing payments, incoming IOU payments that exactly match the specified amount (10 IOU), and incoming XAH payments.
//   Accepts incoming IOU payments that do not exactly match the specified amount.
//
// Accepts:-
//   - Incoming IOU payments that do not exactly match the specified amount.
//
// Rejects:-
//   - Outgoing payments.
//   - Incoming IOU payments that exactly match the specified amount (10 IOU).
//   - Incoming XAH payments.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("RII :: Reject Incoming IOU :: Called.");

    // Define the exact required amount in IOU
    uint64_t exact_amount = 10; // 10 IOU
    uint64_t exact_drops = exact_amount * 1000000;

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
        rollback(SBUF("RII :: Rejected :: Outgoing payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[48];

    // Retrieve the amount field from the originating transaction
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    
    // Reject if not IOU
    if (amount_len != 48)
        rollback(SBUF("RII :: Rejected :: Incoming XAH payment."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exact_amount);
    TRACEVAR(amount_int);

    // Check if the payment exactly matches the required amount, reject if so
    if (amount_drops == exact_drops)
        rollback(SBUF("RII :: Rejected :: Incoming IOU payment matches the rejection amount."), __LINE__);

    // Accept the payment as it does not match the required amount
    accept(SBUF("RII :: Accepted :: Incoming IOU payment does not match the amount required."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
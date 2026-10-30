//**************************************************************
// Xahau Hook 101 Example ~ Reject Outgoing IOU Exact Amount
// Author: @Handy_4ndy
//
// Description:
//   Rejects incoming payments, outgoing IOU payments that exactly match 10 IOU, and outgoing XAH payments. Accepts outgoing IOU payments not equal to 10 IOU.
//
// Triggers:
//   ttPAYMENT (outgoing payments only)
//
// Parameters:
//   None (all logic is hardcoded)
//
// Usage:
//   - Deploy to a Xahau account using Xahau Hooks Builder.
//   - Set trigger for ttPAYMENT.
//   - Outgoing IOU payments not equal to 10 IOU will be accepted; others rejected.
//
// Accepts:
//   - Outgoing IOU payments not equal to 10 IOU.
//
// Rejects:
//   - Incoming payments.
//   - Outgoing IOU payments equal to 10 IOU.
//   - Outgoing XAH payments.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("ROI :: Reject Outgoing IOU :: Called.");

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

    // Reject all incoming payments
    if (!BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        rollback(SBUF("ROI :: Rejected :: Incoming payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[48];

    // Retrieve the amount field from the originating transaction
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    
    // Reject if not IOU
    if (amount_len != 48)
        rollback(SBUF("ROI :: Rejected :: Outgoing XAH payment."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exact_amount);
    TRACEVAR(amount_int);

    // Check if the payment exactly matches the required amount, reject if so
    if (amount_drops == exact_drops)
        rollback(SBUF("ROI :: Rejected :: Outgoing IOU payment matches the rejection amount."), __LINE__);

    // Accept the payment as it does not match the required amount
    accept(SBUF("ROI :: Accepted :: Outgoing IOU payment does not match the amount required."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
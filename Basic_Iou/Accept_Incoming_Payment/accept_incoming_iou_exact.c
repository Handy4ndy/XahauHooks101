//**************************************************************
// Xahau Hook 101 Example ~ Accept Incoming IOU Hook
// Author: @handy_4ndy
//
// Description:
//   This hook allows the owner to accept all incoming IOU payments.
//
// Accepts:-
//   - All incoming IOU payments.
//   - All outgoing payments.
//   - All incoming XAH payments.
//
// Rejects:-
//   - Incoming IOU payments that do not match the exact amount.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AII :: Accept Incoming IOU :: Called.");
    
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

    // Accept all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc)){
        accept(SBUF("AII :: Accepted :: Outgoing payment."), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[48];

    // Retrieve the amount field from the originating transaction
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    
    // Accept Xah payments
    if (amount_len != 48)
        accept(SBUF("AII :: Incoming XAH Accepted."), __LINE__);

    // Convert IOU amount to drops
    int64_t amount_xfl = -INT64_FROM_BUF(amount_buffer);
    int64_t amount_drops = float_int(amount_xfl, 6, 1);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exact_amount);
    TRACEVAR(amount_int);

    // Check if the payment amount exactly matches the required amount
    if (amount_drops != exact_drops)
        rollback(SBUF("AII :: Incoming payment doesn't match the amount required."), __LINE__);

    // Accept the payment as it matches the required amount
    accept(SBUF("AII :: Incoming IOU payment matches the amount required."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
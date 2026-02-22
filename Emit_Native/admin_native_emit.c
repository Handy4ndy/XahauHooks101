//**************************************************************
// Xahau Hook 101 Example ~ Admin Native Emit
// Author: @Handy_4ndy
//
// Description:
//   Allows a designated admin account to trigger native XAH payments from the hook account via invoke transactions.
//
// Triggers:
//   ttINVOKE (admin triggers emission)
//
// Parameters:
//   'ADMIN' (20 bytes): Admin account ID that can trigger emissions.
//   'AMT' (8 bytes): XAH amount to emit (provided in invoke).
//   'DEST' (20 bytes): Destination account (provided in invoke).
//
// Usage:
//   - Set 'ADMIN' during hook installation.
//   - Admin sends Invoke with 'AMT' and 'DEST' to emit XAH payment.
//   - Hook emits XAH payment from itself to DEST for AMT.
//   - Use [Hooks Services](https://hooks.services/tools) for testing.
//
// Accepts:
//   - Invoke from admin with valid 'AMT' and 'DEST'.
//   - Outgoing payments.
//
// Rejects:
//   - Invoke from non-admin.
//   - Invalid parameters.
//   - Emission failures.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved)
{
    TRACESTR("ANE :: Admin Native Emit :: Called.");

    // Hook and origin accounts
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    // Accept outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("ANE :: Accepted :: Outgoing payment"), __LINE__);

    // Handle ttINVOKE
    if (tt == 99)
    {
        // Get admin from hook param
        uint8_t admin_acc[20];
        if (hook_param(SBUF(admin_acc), "ADMIN", 5) != 20)
            rollback(SBUF("ANE :: Error: ADMIN not set"), __LINE__);

        // Check if invoker is admin
        if (!BUFFER_EQUAL_20(otxn_acc, admin_acc))
            accept(SBUF("ANE :: Accepted :: Non-admin invoke passed through"), __LINE__);

        // Get AMT and DEST from invoke params
        uint8_t amt_buf[8];
        if (otxn_param(SBUF(amt_buf), "AMT", 3) != 8)
            rollback(SBUF("ANE :: Error: AMT not provided"), __LINE__);

        uint8_t dest_acc[20];
        if (otxn_param(SBUF(dest_acc), "DEST", 4) != 20)
            rollback(SBUF("ANE :: Error: DEST not provided"), __LINE__);

        uint64_t amt_xah = UINT64_FROM_BUF(amt_buf);
        if (amt_xah == 0)
            rollback(SBUF("ANE :: Error: AMT must be positive"), __LINE__);

        uint64_t amt_drops = amt_xah * 1000000;

        // Reserve and emit
        etxn_reserve(1);
        uint8_t txn[PREPARE_PAYMENT_SIMPLE_SIZE];
        PREPARE_PAYMENT_SIMPLE(txn, amt_drops, dest_acc, 0, 0);

        uint8_t emithash[32];
        if (emit(SBUF(emithash), SBUF(txn)) != 32)
            rollback(SBUF("ANE :: Error: Failed to emit payment"), __LINE__);

        accept(SBUF("ANE :: Accepted :: Payment emitted successfully"), __LINE__);
    }

    // Pass through other transactions
    accept(SBUF("ANE :: Accepted :: Transaction passed through"), __LINE__);

    _g(1, 1);
    return 0;
}
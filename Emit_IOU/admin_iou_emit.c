//**************************************************************
// Xahau Hook 101 Example ~ Admin IOU Emit
// Author: @Handy_4ndy
//
// Description:
//   Allows a designated admin account to trigger IOU token payments from the hook account via invoke transactions.
//
// Triggers:
//   ttINVOKE (admin triggers emission)
//
// Parameters:
//   'ADMIN' (20 bytes): Admin account ID that can trigger emissions.
//   'CURRENCY' (20 bytes): IOU currency code.
//   'ISSUER' (20 bytes): IOU issuer account.
//   'AMT' (8 bytes): IOU amount to emit (provided in invoke).
//   'DEST' (20 bytes): Destination account (provided in invoke).
//
// Usage:
//   - Set 'ADMIN', 'CURRENCY', 'ISSUER' during hook installation.
//   - Admin sends Invoke with 'AMT' and 'DEST' to emit IOU payment.
//   - Hook emits IOU payment from itself to DEST for AMT.
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
    TRACESTR("AIE :: Admin IOU Emit :: Called.");

    // Hook and origin accounts
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    // Accept outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("AIE :: Accepted :: Outgoing payment"), __LINE__);

    // Handle ttINVOKE
    if (tt == 99)
    {
        // Get admin from hook param
        uint8_t admin_acc[20];
        if (hook_param(SBUF(admin_acc), "ADMIN", 5) != 20)
            rollback(SBUF("AIE :: Error: ADMIN not set"), __LINE__);

        // Get currency from hook param
        uint8_t currency[20];
        if (hook_param(SBUF(currency), "CURRENCY", 8) != 20)
            rollback(SBUF("AIE :: Error: CURRENCY not set"), __LINE__);

        // Get issuer from hook param
        uint8_t issuer[20];
        if (hook_param(SBUF(issuer), "ISSUER", 6) != 20)
            rollback(SBUF("AIE :: Error: ISSUER not set"), __LINE__);

        // Check if invoker is admin
        if (!BUFFER_EQUAL_20(otxn_acc, admin_acc))
            accept(SBUF("AIE :: Accepted :: Non-admin invoke passed through"), __LINE__);
        // Get AMT and DEST from invoke params
        uint8_t amt_buf[8];
        if (otxn_param(SBUF(amt_buf), "AMT", 3) != 8)
            rollback(SBUF("AIE :: Error: AMT not provided"), __LINE__);

        uint8_t dest_acc[20];
        if (otxn_param(SBUF(dest_acc), "DEST", 4) != 20)
            rollback(SBUF("AIE :: Error: DEST not provided"), __LINE__);
        uint64_t amt_iou = UINT64_FROM_BUF(amt_buf);
        if (amt_iou == 0)
            rollback(SBUF("AIE :: Error: AMT must be positive"), __LINE__);

        // Reserve and emit
        etxn_reserve(1);

        // Prepare IOU amount in XFL format
        int64_t amount_xfl = float_set(0, amt_iou); // Convert uint64 to XFL
        uint8_t amt_out[48];
        if (float_sto(amt_out - 1, 49, SBUF(currency), SBUF(issuer), amount_xfl, sfAmount) < 0)
            rollback(SBUF("AIE :: Error: Failed to serialize IOU amount"), __LINE__);

        uint8_t txn[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
        PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn, amt_out, dest_acc, 0, 0);

        uint8_t emithash[32];
        if (emit(SBUF(emithash), SBUF(txn)) != 32)
            rollback(SBUF("AIE :: Error: Failed to emit IOU payment"), __LINE__);

        accept(SBUF("AIE :: Accepted :: IOU payment emitted successfully"), __LINE__);
    }

    // Pass through other transactions
    accept(SBUF("AIE :: Accepted :: Transaction passed through"), __LINE__);
    _g(1, 1);
    return 0;
}
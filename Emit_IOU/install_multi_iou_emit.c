//**************************************************************
// Xahau Hook 101 Example ~ Install Multi IOU Emit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits IOU token payments to two user-defined accounts and amounts, set via install parameters.
//   Accepts outgoing payments and IOU payments.
//   Only emits when the incoming XAH payment matches the exact amount set by install parameter.
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to emit (uint64_t)
//   'F_ACC1' (20 bytes): First recipient account (AccountID)
//   'F_ACC2' (20 bytes): Second recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Usage:
//   - Set 'AMT_IN', 'AMT_OUT', 'F_ACC1', 'F_ACC2', 'CURRENCY', and 'ISSUER' during hook installation.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger IOU emits to both recipients.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match the exact amount set by install parameter.
//
// Rejects:
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved)
{
    TRACESTR("IMIE :: Install Multi IOU Emit :: Called.");

    // ACCOUNT: Hook Account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    uint8_t exact_buf[8];
    // Get the amount to receive from the hook param
    if (hook_param(SBUF(exact_buf), "AMT_IN", 6) != 8)
        rollback(SBUF("IMIE :: Error: Payment exactAmount not set"), __LINE__);

    uint64_t exactAmount = UINT64_FROM_BUF(exact_buf);
    if (exactAmount == 0)
        rollback(SBUF("IMIE :: Error: exactAmount cannot be zero"), __LINE__);

    uint8_t amountOut_buf[8];
    // Get the IOU amount to send from the hook param
    if (hook_param(SBUF(amountOut_buf), "AMT_OUT", 7) != 8)
        rollback(SBUF("IMIE :: Error: Payment amountOut not set"), __LINE__);

    uint64_t amountOut = UINT64_FROM_BUF(amountOut_buf);
    if (amountOut == 0)
        rollback(SBUF("IMIE :: Error: amountOut cannot be zero"), __LINE__);

    uint8_t ftxn_acc1[20];
    // Get the first account from the hook param
    if (hook_param(SBUF(ftxn_acc1), "F_ACC1", 6) != 20)
        rollback(SBUF("IMIE :: Error: Account F_ACC1 not set"), __LINE__);

    uint8_t ftxn_acc2[20];
    // Get the second account from the hook param
    if (hook_param(SBUF(ftxn_acc2), "F_ACC2", 6) != 20)
        rollback(SBUF("IMIE :: Error: Account F_ACC2 not set"), __LINE__);

    uint8_t currency[20];
    // Get the IOU currency from the hook param
    if (hook_param(SBUF(currency), "CURRENCY", 8) != 20)
        rollback(SBUF("IMIE :: Error: Currency not set"), __LINE__);

    uint8_t issuer[20];
    // Get the IOU issuer from the hook param
    if (hook_param(SBUF(issuer), "ISSUER", 6) != 20)
        rollback(SBUF("IMIE :: Error: Issuer not set"), __LINE__);

    // Ensure the accounts are unique
    if (BUFFER_EQUAL_20(ftxn_acc1, ftxn_acc2))
        rollback(SBUF("IMIE :: Error: Matching accounts found. Bailing.."), __LINE__);

    // To know the type of origin txn
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
    {
        accept(SBUF("IMIE :: Accepted : Outgoing payment transaction accepted"), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exactAmount);
    TRACEVAR(amount_int);

    // Ensure the payment is XAH
    if (amount_len != 8)
        rollback(SBUF("IMIE :: Error: Non-XAH payment rejected."), __LINE__);

    // Check if the payment is equal to the exact amount (drops)
    uint64_t exactAmountDrops = exactAmount * 1000000;
    if (otxn_drops != exactAmountDrops)
        rollback(SBUF("IMIE :: Error: Payment amount doesn't match the exact_amount_value."), __LINE__);

    // Reserve space for two emitted transactions
    etxn_reserve(2);

    // Prepare IOU amount in XFL format
    int64_t iou_amount_xfl = float_set(0, amountOut); // Convert uint64 to XFL
    uint8_t amt_out[48];
    if (float_sto(amt_out - 1, 49, SBUF(currency), SBUF(issuer), amount_xfl, sfAmount) < 0)
        rollback(SBUF("IMIE :: Error: Failed to serialize IOU amount"), __LINE__);

    // Prepare the first IOU payment transaction
    uint8_t txn1[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
    PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn1, amt_out, ftxn_acc1, 0, 0);

    // Prepare the second IOU payment transaction
    uint8_t txn2[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
    PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn2, amt_out, ftxn_acc2, 0, 0);

    uint8_t emithash1[32];
    uint8_t emithash2[32];

    // Emit the transactions and check if they were successful
    if (emit(SBUF(emithash1), SBUF(txn1)) != 32 ||
        emit(SBUF(emithash2), SBUF(txn2)) != 32)
    {
        rollback(SBUF("IMIE :: Error: Failed to emit IOU transactions"), __LINE__);
    }

    accept(SBUF("IMIE :: Accepted : Payment received and IOU forwarded successfully"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
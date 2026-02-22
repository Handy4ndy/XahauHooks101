//**************************************************************
// Xahau Hook 101 Example ~ Install Native Emit
// Author: @Handy_4ndy
//
// Description:
//   Emits a native XAH payment to a user-defined account and amount, set via install parameters. Only emits when the incoming XAH payment matches the exact amount set by install parameter.
//
// Triggers:
//   ttPAYMENT (incoming payment of exact amount)
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The amount to emit (XAH, uint64_t)
//   'F_ACC' (20 bytes): Recipient account (AccountID)
//
// Usage:
//   - Set 'AMT_IN', 'AMT_OUT', and 'F_ACC' during hook installation.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger the emit to 'F_ACC' for 'AMT_OUT' XAH.
//   - Use [Hooks Services](https://hooks.services/tools) for testing.
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

    TRACESTR("INE :: Install Native Emit :: Called.");

    // ACCOUNT: Hook Account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    uint8_t exact_buf[8];
    // Get the amount to receive from the hook param
    if (hook_param(SBUF(exact_buf), "AMT_IN", 6) != 8)
        rollback(SBUF("INE :: Error: Payment exactAmount not set"), __LINE__);

    uint64_t exactAmount = UINT64_FROM_BUF(exact_buf);
    if (exactAmount == 0)
        rollback(SBUF("INE :: Error: exactAmount cannot be zero"), __LINE__);

    uint8_t amountOut_buf[8];
    // Get the amount to send from the hook param (In XAH)
    if (hook_param(SBUF(amountOut_buf), "AMT_OUT", 7) != 8)
        rollback(SBUF("INE :: Error: Payment amountOut not set"), __LINE__);

    uint64_t amountOut = UINT64_FROM_BUF(amountOut_buf);
    if (amountOut == 0)
        rollback(SBUF("INE :: Error: amountOut cannot be zero"), __LINE__);

    uint8_t ftxn_acc[20];
    // Get the first account from the hook param
    if (hook_param(SBUF(ftxn_acc), "F_ACC", 5) != 20)
        rollback(SBUF("INE :: Error: Account F_ACC not set"), __LINE__);

    // Ensure the accounts are unique
    if (BUFFER_EQUAL_20(ftxn_acc, hook_acc))
        rollback(SBUF("INE :: Error: Matching accounts found. Bailing.."), __LINE__);

    // To know the type of origin txn
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
    {
        accept(SBUF("INE :: Accepted : Outgoing payment transaction accepted"), __LINE__);
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
    {
        rollback(SBUF("INE :: Error: Non-XAH payment rejected."), __LINE__);
    }

    // Check if the payment is equal to the exact amount (drops)
    uint64_t exactAmountDrops = exactAmount * 1000000;
    if (otxn_drops != exactAmountDrops)
    {
        rollback(SBUF("INE :: Error: Payment amount doesn't match the exact_amount_value."), __LINE__);
    }

    // Reserve space for the emitted transaction
    etxn_reserve(1);

    // Prepare the first payment transactions
    uint64_t amountOutDrops = amountOut * 1000000;
    uint8_t txn[PREPARE_PAYMENT_SIMPLE_SIZE];
    PREPARE_PAYMENT_SIMPLE(txn, amountOutDrops, ftxn_acc, 0, 0);

    uint8_t emithash[32];

    // Emit the transaction and check if it was successful
    if (emit(SBUF(emithash), SBUF(txn)) != 32)
    {
        rollback(SBUF("INE :: Error: Failed to emit transactions"), __LINE__);
    }

    accept(SBUF("INE :: Accepted : Payment recieved and forwarded Successfully"), __LINE__);

    _g(1, 1);
    return 0;
}

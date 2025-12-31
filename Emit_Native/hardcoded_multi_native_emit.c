//**************************************************************
// Xahau Hook 101 Example ~ Hardcoded Multi Native Emit
// Author: @Handy_4ndy
//
// Description:
//   This hook demonstrates how to emit hardcoded native events to multiple accounts on the Xahau network.
//   When triggered, it emits two predefined payment transactions to two hardcoded accounts.
//
// Usage:
//   - Replace the hardcoded accounts and amount as needed.
//   - [Hooks Services](https://hooks.services/tools)
//   - Deploy this hook to your account.
//   - Any payment of 10 XAH triggering this hook will cause it to emit the hardcoded events.
//   - Useful as a template for learning how to use emits to multiple recipients in Xahau Hooks.
//
// Accepts:
//   - Outgoing payments.
//   - Incoming XAH payments that match the exact amount.
//
// Rejects:
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

// Configure the amount to emit (In drops)
uint64_t amountOut = 5000000; // 5 XAH

// Configure Account to emit the payment to.
uint8_t ftxn_acc1[20] = {0xE8U, 0xF1U, 0x0CU, 0xA2U, 0x7BU, 0xD8U, 0xECU, 0xDBU, 0x28U, 0x9BU, 0x94U, 0x7CU, 0xB1U, 0xA7U, 0x75U, 0x48U, 0xCAU, 0x0EU, 0x80U, 0xD9U};
uint8_t ftxn_acc2[20] = {0x17U, 0xFAU, 0xCBU, 0xECU, 0xBFU, 0x53U, 0xD4U, 0x10U, 0x08U, 0x15U, 0xF0U, 0x6AU, 0x74U, 0x2FU, 0xCDU, 0x2AU, 0x15U, 0x82U, 0x34U, 0x52U};

int64_t hook(uint32_t reserved)
{

    TRACESTR("HMNE :: Hardcoded Multi Native Emit :: Called.");

    // Configure an exact amount to recieve
    uint64_t exact_amount_value = 10;                           // 10 XAH
    uint64_t exact_amount_drops = exact_amount_value * 1000000; // Convert to drops

    // ACCOUNT: Hook Account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // ACCOUNT: Origin Tx Account
    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // To know the type of origin txn
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
    {
        accept(SBUF("HMNE :: Accepted : Outgoing payment transaction accepted"), __LINE__);
    }

    // Buffer to hold the amount field from the transaction
    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    // Trace the values (in XAH)
    TRACEVAR(exact_amount_value);
    TRACEVAR(amount_int);

    // Ensure the payment is XAH
    if (amount_len != 8)
    {
        rollback(SBUF("HMNE :: Error: Non-XAH payment rejected."), __LINE__);
    }

    // Check if the payment is equal to the exact amount
    if (otxn_drops != exact_amount_drops)
    {
        rollback(SBUF("HMNE :: Error: Payment amount doesn't match the exact_amount_value."), __LINE__);
    }

    // Reserve space for two emitted transactions
    etxn_reserve(2);

    // Prepare the first payment transactions
    uint8_t txn1[PREPARE_PAYMENT_SIMPLE_SIZE];
    PREPARE_PAYMENT_SIMPLE(txn1, amountOut, ftxn_acc1, 0, 0);

    // Prepare the second payment transaction
    uint8_t txn2[PREPARE_PAYMENT_SIMPLE_SIZE];
    PREPARE_PAYMENT_SIMPLE(txn2, amountOut, ftxn_acc2, 0, 0);

    uint8_t emithash1[32];
    uint8_t emithash2[32];

    // Emit the transactions and check if they were successful
    if (emit(SBUF(emithash1), SBUF(txn1)) != 32 ||
        emit(SBUF(emithash2), SBUF(txn2)) != 32)
    {
        rollback(SBUF("HMNE :: Error: Failed to emit transactions"), __LINE__);
    }

    accept(SBUF("HMNE :: Accepted : Payment received and forwarded Successfully"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}

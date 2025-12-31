//**************************************************************
// Xahau Hook 101 Example ~ Hardcoded Multi IOU Emit
// Author: @Handy_4ndy
//
// Description:
//   This hook demonstrates how to emit hardcoded IOU token payments to multiple accounts on the Xahau network.
//   When triggered, it emits two predefined IOU payment transactions to two hardcoded accounts.
//
// Usage:
//   - Replace the hardcoded accounts, amount, currency, and issuer as needed.
//   - [Hooks Services](https://hooks.services/tools)
//   - Deploy this hook to your account.
//   - Any payment of 10 XAH triggering this hook will cause it to emit the hardcoded IOU events.
//   - Useful as a template for learning how to use IOU emits to multiple recipients in Xahau Hooks.
//
// Accepts:
//   - Outgoing payments.
//   - Incoming XAH payments that match the exact amount.
//
// Rejects:
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

// Configure the IOU amount to emit (as uint64 in smallest units)
uint64_t amountOut = 5; // 5 IOU tokens

// Configure Accounts to emit the payment to
uint8_t ftxn_acc1[20] = {0x58U, 0xBFU, 0x50U, 0x22U, 0x87U, 0x21U, 0xCFU, 0x7EU, 0x6BU, 0x3AU, 0xE4U, 0x5AU, 0xCEU, 0xFEU, 0xEBU, 0x71U, 0x97U, 0x4CU, 0xA5U, 0x71U};
uint8_t ftxn_acc2[20] = {0xE8U, 0xF1U, 0x0CU, 0xA2U, 0x7BU, 0xD8U, 0xECU, 0xDBU, 0x28U, 0x9BU, 0x94U, 0x7CU, 0xB1U, 0xA7U, 0x75U, 0x48U, 0xCAU, 0x0EU, 0x80U, 0xD9U};

// Configure IOU currency code (20 bytes, e.g., "XPN")
uint8_t currency[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'X', 'P', 'N', 0, 0, 0, 0, 0};

// Configure IOU issuer (hardcoded issuer account)
uint8_t issuer[20] = {0xB9U, 0x59U, 0xD6U, 0xACU, 0x9DU, 0x15U, 0x89U, 0x17U, 0xECU, 0x26U, 0x9EU, 0xACU, 0x05U, 0x70U, 0x19U, 0x07U, 0x4FU, 0x56U, 0xD6U, 0x39U};

int64_t hook(uint32_t reserved)
{
    TRACESTR("HMIE :: Hardcoded Multi IOU Emit :: Called.");

    // Configure an exact amount to receive
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
        accept(SBUF("HMIE :: Accepted : Outgoing payment transaction accepted"), __LINE__);
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
        rollback(SBUF("HMIE :: Error: Non-XAH payment rejected."), __LINE__);
    }

    // Check if the payment is equal to the exact amount
    if (otxn_drops != exact_amount_drops)
    {
        rollback(SBUF("HMIE :: Error: Payment amount doesn't match the exact_amount_value."), __LINE__);
    }

    // Reserve space for two emitted transactions
    etxn_reserve(2);

    // Prepare IOU amount in XFL format
    int64_t iou_amount_xfl = float_set(0, amountOut); // Convert uint64 to XFL
    uint8_t amt_out[48];
    if (float_sto(amt_out - 1, 49, SBUF(currency), SBUF(issuer), amount_xfl, sfAmount) < 0)
        rollback(SBUF("HMIE :: Error: Failed to serialize IOU amount"), __LINE__);

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
        rollback(SBUF("HMIE :: Error: Failed to emit IOU transactions"), __LINE__);
    }

    accept(SBUF("HMIE :: Accepted : Payment received and IOU forwarded successfully"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
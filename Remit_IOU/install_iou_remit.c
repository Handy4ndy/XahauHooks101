//**************************************************************
// Xahau Hook 101 Example ~ Install IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits an IOU token remit to a user-defined account and amount,
//   set via install parameters. Uses Remit transaction type for automatic trustline creation.
//   Accepts outgoing payments and IOU payments.
//   Only emits when the incoming XAH payment matches the exact amount set by install parameter.
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to remit (uint64_t)
//   'F_ACC' (20 bytes): Recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Key Benefits of Remit:
//   ✅ Automatic trustline creation (no pre-setup required)
//   ✅ Can send to accounts that don't exist yet (account creation)
//   ✅ Reserves paid by sender, not recipient
//
// Usage:
//   - Set 'AMT_IN', 'AMT_OUT', 'F_ACC', 'CURRENCY', and 'ISSUER' during hook installation.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger the IOU remit to 'F_ACC'.
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

// Field codes for Remit transaction Amounts array
#define sfAmountEntry ((14U << 16U) + 91U)  // 0xE0 0x5B
#define sfAmounts ((15U << 16U) + 92U)      // 0xF0 0x5C

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)
#define GUARD(maxiter) _g(__LINE__, (maxiter) + 1)

#define UINT64_FROM_BUF(buf) \
    (((uint64_t)(buf)[0] << 56) + ((uint64_t)(buf)[1] << 48) + \
     ((uint64_t)(buf)[2] << 40) + ((uint64_t)(buf)[3] << 32) + \
     ((uint64_t)(buf)[4] << 24) + ((uint64_t)(buf)[5] << 16) + \
     ((uint64_t)(buf)[6] << 8) + (uint64_t)(buf)[7])

// Base Remit transaction template (229 bytes)
// clang-format off
uint8_t txn[350] =
{
/* size,upto */
/*   3,   0 */   0x12U, 0x00U, 0x5FU,                                           /* ttREMIT */
/*   5,   3 */   0x22U, 0x80U, 0x00U, 0x00U, 0x00U,                            /* Flags */
/*   5,   8 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,                            /* Sequence */
/*   6,  13 */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,                     /* FirstLedgerSequence */
/*   6,  19 */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,                     /* LastLedgerSequence */
/*   9,  25 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, /* Fee */
/*  35,  34 */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* SigningPubKey */
/*  22,  69 */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* Account */
/*  22,  91 */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* Destination */
/* 116, 113 */   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* EmitDetails */
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*   0, 229 */   /* Amounts array appended here */
};
// clang-format on

#define BASE_SIZE 229U
#define HOOK_ACC (txn + 71U)
#define DEST_ACC (txn + 93U)
#define EMIT_OUT (txn + 113U)
#define AMOUNTS_OUT (txn + 229U)

int64_t hook(uint32_t reserved)
{
    TRACESTR("IIR :: Install IOU Remit :: Called.");

    // Get hook account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // Get origin account
    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Get install parameters
    uint8_t exact_buf[8];
    if (hook_param(SBUF(exact_buf), "AMT_IN", 6) != 8)
        NOPE("IIR :: Error: AMT_IN parameter not set.");

    uint64_t exactAmount = UINT64_FROM_BUF(exact_buf);
    if (exactAmount == 0)
        NOPE("IIR :: Error: AMT_IN cannot be zero.");

    uint8_t amountOut_buf[8];
    if (hook_param(SBUF(amountOut_buf), "AMT_OUT", 7) != 8)
        NOPE("IIR :: Error: AMT_OUT parameter not set.");

    uint64_t amountOut = UINT64_FROM_BUF(amountOut_buf);
    if (amountOut == 0)
        NOPE("IIR :: Error: AMT_OUT cannot be zero.");

    uint8_t ftxn_acc[20];
    if (hook_param(SBUF(ftxn_acc), "F_ACC", 5) != 20)
        NOPE("IIR :: Error: F_ACC parameter not set.");

    uint8_t currency[20];
    if (hook_param(SBUF(currency), "CURRENCY", 8) != 20)
        NOPE("IIR :: Error: CURRENCY parameter not set.");

    uint8_t issuer[20];
    if (hook_param(SBUF(issuer), "ISSUER", 6) != 20)
        NOPE("IIR :: Error: ISSUER parameter not set.");

    // Ensure the accounts are unique
    if (BUFFER_EQUAL_20(ftxn_acc, hook_acc))
        NOPE("IIR :: Error: F_ACC matches hook account.");

    // Transaction type
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        DONE("IIR :: Outgoing payment accepted.");

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
        NOPE("IIR :: Error: Non-XAH payment rejected.");

    // Check if the payment is equal to the exact amount (drops)
    uint64_t exactAmountDrops = exactAmount * 1000000;
    if (otxn_drops != exactAmountDrops)
        NOPE("IIR :: Error: Payment amount doesn't match AMT_IN.");

    // Convert IOU amount to XFL format
    int64_t iou_amount_xfl = float_set(0, amountOut);

    // Build Amounts array
    uint8_t* amounts_ptr = AMOUNTS_OUT;
    
    *amounts_ptr++ = 0xF0U;  // sfAmounts array start
    *amounts_ptr++ = 0x5CU;
    
    *amounts_ptr++ = 0xE0U;  // sfAmountEntry object start
    *amounts_ptr++ = 0x5BU;
    
    int32_t amount_len_out = float_sto(
        amounts_ptr, 49,
        currency, 20,
        issuer, 20,
        iou_amount_xfl,
        sfAmount
    );
    
    if (amount_len_out < 0)
        NOPE("IIR :: Error: Failed to serialize IOU amount.");
    
    amounts_ptr += amount_len_out;
    
    *amounts_ptr++ = 0xE1U;  // End AmountEntry
    *amounts_ptr++ = 0xF1U;  // End Amounts array
    
    int32_t amounts_len = amounts_ptr - AMOUNTS_OUT;

    // Fill transaction fields
    hook_account(HOOK_ACC, 20);
    
    for (int i = 0; GUARD(20), i < 20; ++i)
        DEST_ACC[i] = ftxn_acc[i];

    // Prepare for emission
    etxn_reserve(1);
    
    int32_t total_size = BASE_SIZE + amounts_len;
    
    etxn_details(EMIT_OUT, 116U);
    
    // Encode ledger sequences
    int64_t seq = ledger_seq() + 1;
    txn[15] = (seq >> 24U) & 0xFFU;
    txn[16] = (seq >> 16U) & 0xFFU;
    txn[17] = (seq >>  8U) & 0xFFU;
    txn[18] = seq & 0xFFU;
    
    seq += 4;
    txn[21] = (seq >> 24U) & 0xFFU;
    txn[22] = (seq >> 16U) & 0xFFU;
    txn[23] = (seq >>  8U) & 0xFFU;
    txn[24] = seq & 0xFFU;
    
    // Calculate and encode fee
    int64_t fee = etxn_fee_base(txn, total_size);
    
    if (fee < 0)
        NOPE("IIR :: Error: Fee calculation failed.");
    
    uint64_t fee_tmp = fee;
    uint8_t* fee_ptr = (uint8_t*)&fee;
    *fee_ptr++ = 0b01000000 + ((fee_tmp >> 56) & 0b00111111);
    *fee_ptr++ = (fee_tmp >> 48) & 0xFFU;
    *fee_ptr++ = (fee_tmp >> 40) & 0xFFU;
    *fee_ptr++ = (fee_tmp >> 32) & 0xFFU;
    *fee_ptr++ = (fee_tmp >> 24) & 0xFFU;
    *fee_ptr++ = (fee_tmp >> 16) & 0xFFU;
    *fee_ptr++ = (fee_tmp >>  8) & 0xFFU;
    *fee_ptr++ = (fee_tmp >>  0) & 0xFFU;
    
    *((uint64_t*)(txn + 26)) = fee;
    
    // Emit transaction
    uint8_t emithash[32];
    if (emit(SBUF(emithash), txn, total_size) < 0)
        NOPE("IIR :: Error: Failed to emit remit.");

    DONE("IIR :: Payment received and IOU remitted successfully.");
}

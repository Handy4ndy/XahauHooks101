//**************************************************************
// Xahau Hook 101 Example ~ Hardcoded Multi IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   Emits hardcoded IOU token remits to multiple accounts when an incoming payment of exact XAH amount is received. Uses the Remit transaction type with multiple AmountEntry objects.
//
// Triggers:
//   ttPAYMENT (incoming, exact amount)
//
// Parameters:
//   Hardcoded: recipient accounts, IOU amounts, currency, issuer.
//
// Usage:
//   - Replace the hardcoded accounts, amount, currency, and issuer as needed.
//   - Deploy to your account.
//   - Any payment of 10 XAH triggers the hardcoded IOU remits.
//   - Useful for learning multi-recipient remit patterns in Xahau Hooks.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Outgoing transactions.
//   - Incoming XAH payments that match the exact amount.
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

// Configure the IOU amount to remit (as uint64 in smallest units)
uint64_t amountOut = 5; // 5 IOU tokens

// Configure Accounts to remit the payment to
uint8_t ftxn_acc1[20] = {0x58U, 0xBFU, 0x50U, 0x22U, 0x87U, 0x21U, 0xCFU, 0x7EU, 0x6BU, 0x3AU, 0xE4U, 0x5AU, 0xCEU, 0xFEU, 0xEBU, 0x71U, 0x97U, 0x4CU, 0xA5U, 0x71U};
uint8_t ftxn_acc2[20] = {0xE8U, 0xF1U, 0x0CU, 0xA2U, 0x7BU, 0xD8U, 0xECU, 0xDBU, 0x28U, 0x9BU, 0x94U, 0x7CU, 0xB1U, 0xA7U, 0x75U, 0x48U, 0xCAU, 0x0EU, 0x80U, 0xD9U};

// Configure IOU currency code (20 bytes, e.g., "XPN")
uint8_t currency[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'X', 'P', 'N', 0, 0, 0, 0, 0};

// Configure IOU issuer (hardcoded issuer account)
uint8_t issuer[20] = {0xB9U, 0x59U, 0xD6U, 0xACU, 0x9DU, 0x15U, 0x89U, 0x17U, 0xECU, 0x26U, 0x9EU, 0xACU, 0x05U, 0x70U, 0x19U, 0x07U, 0x4FU, 0x56U, 0xD6U, 0x39U};

// clang-format off
// Transaction template - used for both remit transactions
const uint8_t txn_template[350] =
{
/*   3,   0 */   0x12U, 0x00U, 0x5FU,
/*   5,   3 */   0x22U, 0x80U, 0x00U, 0x00U, 0x00U,
/*   5,   8 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,
/*   6,  13 */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,
/*   6,  19 */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,
/*   9,  25 */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U,
/*  35,  34 */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*  22,  69 */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*  22,  91 */   0x83U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/* 116, 113 */   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
// clang-format on

#define BASE_SIZE 229U

int64_t hook(uint32_t reserved)
{
    TRACESTR("HMIR :: Hardcoded Multi IOU Remit :: Called.");

    // Configure an exact amount to receive
    uint64_t exact_amount_value = 10;                           // 10 XAH
    uint64_t exact_amount_drops = exact_amount_value * 1000000; // Convert to drops

    // Get hook account
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    // Get origin account
    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    // If the transaction is outgoing from the hook account, accept it
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        DONE("HMIR :: Outgoing payment accepted.");

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
        NOPE("HMIR :: Error: Non-XAH payment rejected.");

    // Check if the payment is equal to the exact amount
    if (otxn_drops != exact_amount_drops)
        NOPE("HMIR :: Error: Payment amount doesn't match exact_amount_value.");

    // Reserve space for two emitted transactions
    etxn_reserve(2);

    // Convert IOU amount to XFL format
    int64_t iou_amount_xfl = float_set(0, amountOut);

    // Initialize both transaction buffers from template
    uint8_t txn1[350];
    uint8_t txn2[350];
    for (int i = 0; GUARD(350), i < 350; ++i)
    {
        txn1[i] = txn_template[i];
        txn2[i] = txn_template[i];
    }

    // Build first remit transaction
    uint8_t* amounts_ptr1 = txn1 + 229U;
    
    *amounts_ptr1++ = 0xF0U;  // sfAmounts array start
    *amounts_ptr1++ = 0x5CU;
    
    *amounts_ptr1++ = 0xE0U;  // sfAmountEntry object start
    *amounts_ptr1++ = 0x5BU;
    
    int32_t amount_len1 = float_sto(
        amounts_ptr1, 49,
        currency, 20,
        issuer, 20,
        iou_amount_xfl,
        sfAmount
    );
    
    if (amount_len1 < 0)
        NOPE("HMIR :: Error: Failed to serialize IOU amount 1.");
    
    amounts_ptr1 += amount_len1;
    
    *amounts_ptr1++ = 0xE1U;  // End AmountEntry
    *amounts_ptr1++ = 0xF1U;  // End Amounts array
    
    int32_t amounts_len1 = amounts_ptr1 - (txn1 + 229U);

    // Fill first transaction fields
    hook_account(txn1 + 71U, 20);
    for (int i = 0; GUARD(20), i < 20; ++i)
        txn1[93U + i] = ftxn_acc1[i];

    int32_t total_size1 = BASE_SIZE + amounts_len1;
    etxn_details(txn1 + 113U, 116U);
    
    // Encode ledger sequences for txn1
    int64_t seq = ledger_seq() + 1;
    txn1[15] = (seq >> 24U) & 0xFFU;
    txn1[16] = (seq >> 16U) & 0xFFU;
    txn1[17] = (seq >>  8U) & 0xFFU;
    txn1[18] = seq & 0xFFU;
    
    seq += 4;
    txn1[21] = (seq >> 24U) & 0xFFU;
    txn1[22] = (seq >> 16U) & 0xFFU;
    txn1[23] = (seq >>  8U) & 0xFFU;
    txn1[24] = seq & 0xFFU;
    
    // Calculate and encode fee for txn1
    int64_t fee1 = etxn_fee_base(txn1, total_size1);
    if (fee1 < 0)
        NOPE("HMIR :: Error: Fee calculation failed for txn1.");
    
    uint64_t fee_tmp1 = fee1;
    uint8_t* fee_ptr1 = (uint8_t*)&fee1;
    *fee_ptr1++ = 0b01000000 + ((fee_tmp1 >> 56) & 0b00111111);
    *fee_ptr1++ = (fee_tmp1 >> 48) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 40) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 32) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 24) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 16) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >>  8) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >>  0) & 0xFFU;
    *((uint64_t*)(txn1 + 26)) = fee1;

    // Build second remit transaction
    uint8_t* amounts_ptr2 = txn2 + 229U;
    
    *amounts_ptr2++ = 0xF0U;
    *amounts_ptr2++ = 0x5CU;
    *amounts_ptr2++ = 0xE0U;
    *amounts_ptr2++ = 0x5BU;
    
    int32_t amount_len2 = float_sto(
        amounts_ptr2, 49,
        currency, 20,
        issuer, 20,
        iou_amount_xfl,
        sfAmount
    );
    
    if (amount_len2 < 0)
        NOPE("HMIR :: Error: Failed to serialize IOU amount 2.");
    
    amounts_ptr2 += amount_len2;
    *amounts_ptr2++ = 0xE1U;
    *amounts_ptr2++ = 0xF1U;
    
    int32_t amounts_len2 = amounts_ptr2 - (txn2 + 229U);

    // Fill second transaction fields
    hook_account(txn2 + 71U, 20);
    for (int i = 0; GUARD(20), i < 20; ++i)
        txn2[93U + i] = ftxn_acc2[i];

    int32_t total_size2 = BASE_SIZE + amounts_len2;
    etxn_details(txn2 + 113U, 116U);
    
    // Encode ledger sequences for txn2
    seq = ledger_seq() + 1;
    txn2[15] = (seq >> 24U) & 0xFFU;
    txn2[16] = (seq >> 16U) & 0xFFU;
    txn2[17] = (seq >>  8U) & 0xFFU;
    txn2[18] = seq & 0xFFU;
    
    seq += 4;
    txn2[21] = (seq >> 24U) & 0xFFU;
    txn2[22] = (seq >> 16U) & 0xFFU;
    txn2[23] = (seq >>  8U) & 0xFFU;
    txn2[24] = seq & 0xFFU;
    
    // Calculate and encode fee for txn2
    int64_t fee2 = etxn_fee_base(txn2, total_size2);
    if (fee2 < 0)
        NOPE("HMIR :: Error: Fee calculation failed for txn2.");
    
    uint64_t fee_tmp2 = fee2;
    uint8_t* fee_ptr2 = (uint8_t*)&fee2;
    *fee_ptr2++ = 0b01000000 + ((fee_tmp2 >> 56) & 0b00111111);
    *fee_ptr2++ = (fee_tmp2 >> 48) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 40) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 32) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 24) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 16) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >>  8) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >>  0) & 0xFFU;
    *((uint64_t*)(txn2 + 26)) = fee2;

    // Emit both transactions
    uint8_t emithash1[32];
    uint8_t emithash2[32];

    if (emit(SBUF(emithash1), txn1, total_size1) < 0 ||
        emit(SBUF(emithash2), txn2, total_size2) < 0)
        NOPE("HMIR :: Error: Failed to emit remit transactions.");

    DONE("HMIR :: Payment received and IOU remitted to multiple accounts successfully.");
}

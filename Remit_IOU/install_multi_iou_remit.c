//**************************************************************
// Xahau Hook 101 Example ~ Install Multi IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits IOU token remits to two user-defined accounts and amounts,
//   set via install parameters. Uses Remit for automatic trustline creation.
//   Accepts outgoing payments and IOU payments.
//   Only emits when the incoming XAH payment matches the exact amount.
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to remit (uint64_t)
//   'F_ACC1' (20 bytes): First recipient account (AccountID)
//   'F_ACC2' (20 bytes): Second recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Key Benefits of Remit:
//   ✅ Automatic trustline creation (no pre-setup required)
//   ✅ Can send to accounts that don't exist yet
//
// Usage:
//   - Set all parameters during hook installation.
//   - Incoming XAH payments matching 'AMT_IN' trigger IOU remits to both recipients.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match AMT_IN exactly.
//
// Rejects:
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

#define sfAmountEntry ((14U << 16U) + 91U)
#define sfAmounts ((15U << 16U) + 92U)

#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)
#define GUARD(maxiter) _g(__LINE__, (maxiter) + 1)

#define UINT64_FROM_BUF(buf) \
    (((uint64_t)(buf)[0] << 56) + ((uint64_t)(buf)[1] << 48) + \
     ((uint64_t)(buf)[2] << 40) + ((uint64_t)(buf)[3] << 32) + \
     ((uint64_t)(buf)[4] << 24) + ((uint64_t)(buf)[5] << 16) + \
     ((uint64_t)(buf)[6] << 8) + (uint64_t)(buf)[7])

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
    TRACESTR("IMIR :: Install Multi IOU Remit :: Called.");

    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Get install parameters
    uint8_t exact_buf[8];
    if (hook_param(SBUF(exact_buf), "AMT_IN", 6) != 8)
        NOPE("IMIR :: Error: AMT_IN not set.");

    uint64_t exactAmount = UINT64_FROM_BUF(exact_buf);
    if (exactAmount == 0)
        NOPE("IMIR :: Error: AMT_IN cannot be zero.");

    uint8_t amountOut_buf[8];
    if (hook_param(SBUF(amountOut_buf), "AMT_OUT", 7) != 8)
        NOPE("IMIR :: Error: AMT_OUT not set.");

    uint64_t amountOut = UINT64_FROM_BUF(amountOut_buf);
    if (amountOut == 0)
        NOPE("IMIR :: Error: AMT_OUT cannot be zero.");

    uint8_t ftxn_acc1[20];
    if (hook_param(SBUF(ftxn_acc1), "F_ACC1", 6) != 20)
        NOPE("IMIR :: Error: F_ACC1 not set.");

    uint8_t ftxn_acc2[20];
    if (hook_param(SBUF(ftxn_acc2), "F_ACC2", 6) != 20)
        NOPE("IMIR :: Error: F_ACC2 not set.");

    uint8_t currency[20];
    if (hook_param(SBUF(currency), "CURRENCY", 8) != 20)
        NOPE("IMIR :: Error: CURRENCY not set.");

    uint8_t issuer[20];
    if (hook_param(SBUF(issuer), "ISSUER", 6) != 20)
        NOPE("IMIR :: Error: ISSUER not set.");

    if (BUFFER_EQUAL_20(ftxn_acc1, ftxn_acc2))
        NOPE("IMIR :: Error: Matching accounts found.");

    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        DONE("IMIR :: Outgoing payment accepted.");

    uint8_t amount_buffer[8];
    int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
    int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
    int64_t amount_xfl = float_set(-6, otxn_drops);
    int64_t amount_int = float_int(amount_xfl, 0, 1);

    TRACEVAR(exactAmount);
    TRACEVAR(amount_int);

    if (amount_len != 8)
        NOPE("IMIR :: Error: Non-XAH payment rejected.");

    uint64_t exactAmountDrops = exactAmount * 1000000;
    if (otxn_drops != exactAmountDrops)
        NOPE("IMIR :: Error: Payment amount doesn't match AMT_IN.");

    etxn_reserve(2);

    int64_t iou_amount_xfl = float_set(0, amountOut);

    // Initialize both transaction buffers from template
    uint8_t txn1[350];
    uint8_t txn2[350];
    for (int i = 0; GUARD(350), i < 350; ++i)
    {
        txn1[i] = txn_template[i];
        txn2[i] = txn_template[i];
    }

    // Build first transaction
    uint8_t* amounts_ptr1 = txn1 + 229U;
    *amounts_ptr1++ = 0xF0U;
    *amounts_ptr1++ = 0x5CU;
    *amounts_ptr1++ = 0xE0U;
    *amounts_ptr1++ = 0x5BU;
    
    int32_t amount_len1 = float_sto(amounts_ptr1, 49, currency, 20, issuer, 20, iou_amount_xfl, sfAmount);
    if (amount_len1 < 0)
        NOPE("IMIR :: Error: Failed to serialize amount 1.");
    
    amounts_ptr1 += amount_len1;
    *amounts_ptr1++ = 0xE1U;
    *amounts_ptr1++ = 0xF1U;
    
    int32_t amounts_len1 = amounts_ptr1 - (txn1 + 229U);

    hook_account(txn1 + 71U, 20);
    for (int i = 0; GUARD(20), i < 20; ++i)
        txn1[93U + i] = ftxn_acc1[i];

    int32_t total_size1 = BASE_SIZE + amounts_len1;
    etxn_details(txn1 + 113U, 116U);
    
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
    
    int64_t fee1 = etxn_fee_base(txn1, total_size1);
    if (fee1 < 0)
        NOPE("IMIR :: Error: Fee calculation failed for txn1.");
    
    uint64_t fee_tmp1 = fee1;
    uint8_t* fee_ptr1 = (uint8_t*)&fee1;
    *fee_ptr1++ = 0b01000000 + ((fee_tmp1 >> 56) & 0b00111111);
    *fee_ptr1++ = (fee_tmp1 >> 48) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 40) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 32) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 24) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >> 16) & 0xFFU;
    *fee_ptr1++ = (fee_tmp1 >>  8) & 0xFFU;
    *fee_ptr1++ = fee_tmp1 & 0xFFU;
    *((uint64_t*)(txn1 + 26)) = fee1;

    // Build second transaction
    uint8_t* amounts_ptr2 = txn2 + 229U;
    *amounts_ptr2++ = 0xF0U;
    *amounts_ptr2++ = 0x5CU;
    *amounts_ptr2++ = 0xE0U;
    *amounts_ptr2++ = 0x5BU;
    
    int32_t amount_len2 = float_sto(amounts_ptr2, 49, currency, 20, issuer, 20, iou_amount_xfl, sfAmount);
    if (amount_len2 < 0)
        NOPE("IMIR :: Error: Failed to serialize amount 2.");
    
    amounts_ptr2 += amount_len2;
    *amounts_ptr2++ = 0xE1U;
    *amounts_ptr2++ = 0xF1U;
    
    int32_t amounts_len2 = amounts_ptr2 - (txn2 + 229U);

    hook_account(txn2 + 71U, 20);
    for (int i = 0; GUARD(20), i < 20; ++i)
        txn2[93U + i] = ftxn_acc2[i];

    int32_t total_size2 = BASE_SIZE + amounts_len2;
    etxn_details(txn2 + 113U, 116U);
    
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
    
    int64_t fee2 = etxn_fee_base(txn2, total_size2);
    if (fee2 < 0)
        NOPE("IMIR :: Error: Fee calculation failed for txn2.");
    
    uint64_t fee_tmp2 = fee2;
    uint8_t* fee_ptr2 = (uint8_t*)&fee2;
    *fee_ptr2++ = 0b01000000 + ((fee_tmp2 >> 56) & 0b00111111);
    *fee_ptr2++ = (fee_tmp2 >> 48) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 40) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 32) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 24) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >> 16) & 0xFFU;
    *fee_ptr2++ = (fee_tmp2 >>  8) & 0xFFU;
    *fee_ptr2++ = fee_tmp2 & 0xFFU;
    *((uint64_t*)(txn2 + 26)) = fee2;

    uint8_t emithash1[32];
    uint8_t emithash2[32];

    if (emit(SBUF(emithash1), txn1, total_size1) < 0 ||
        emit(SBUF(emithash2), txn2, total_size2) < 0)
        NOPE("IMIR :: Error: Failed to emit remit transactions.");

    DONE("IMIR :: Payment received and IOU remitted to multiple accounts successfully.");
}

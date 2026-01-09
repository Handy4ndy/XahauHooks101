//**************************************************************
// Xahau Hook 101 Example ~ Admin IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   This hook allows a designated admin account to trigger IOU token remit
//   transactions from the hook account via invoke transactions.
//   Uses the Remit transaction type for automatic trustline creation.
//
// Parameters:
//   'ADMIN' (20 bytes): Admin account ID that can trigger remits.
//   'CURRENCY' (20 bytes): IOU currency code.
//   'ISSUER' (20 bytes): IOU issuer account.
//
// Usage:
//   - Set 'ADMIN', 'CURRENCY', 'ISSUER' during hook installation.
//   - Admin sends Invoke with 'AMT' (8 bytes, IOU amount) and 'DEST' (20 bytes, destination account).
//   - Hook emits Remit transaction from itself to DEST for AMT.
//   - [Hooks Services](https://hooks.services/tools)
//
// Key Benefits of Remit:
//   ✅ Automatic trustline creation (no pre-setup required)
//   ✅ Can send to accounts that don't exist yet (account creation)
//   ✅ Reserves paid by sender, not recipient
//
// Accepts:
//   - Invoke from admin with valid AMT and DEST.
//   - Outgoing transactions.
//
// Rejects:
//   - Invoke from non-admin.
//   - Invalid parameters.
//   - Emission failures.
//**************************************************************

#include "hookapi.h"

// Field codes for Remit transaction Amounts array
#define sfAmountEntry ((14U << 16U) + 91U)  // 0xE0 0x5B
#define sfAmounts ((15U << 16U) + 92U)      // 0xF0 0x5C

// Utility macros
#define DONE(x) accept(SBUF(x), __LINE__)
#define NOPE(x) rollback(SBUF(x), __LINE__)
#define GUARD(maxiter) _g(__LINE__, (maxiter) + 1)

// Convert 8-byte buffer to uint64 (big-endian)
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
#define FLS_OUT (txn + 15U)
#define LLS_OUT (txn + 21U)
#define FEE_OUT (txn + 26U)
#define HOOK_ACC (txn + 71U)
#define DEST_ACC (txn + 93U)
#define EMIT_OUT (txn + 113U)
#define AMOUNTS_OUT (txn + 229U)

int64_t hook(uint32_t reserved)
{
    TRACESTR("AIR :: Admin IOU Remit :: Called.");

    // Only process Invoke transactions
    if (otxn_type() != 99)
        DONE("AIR :: Non-INVOKE transaction passed through.");

    // Get hook account
    uint8_t hook_acc[20];
    if (hook_account(SBUF(hook_acc)) != 20)
        NOPE("AIR :: Failed to get hook account.");

    // Get originating account
    uint8_t otxn_acc[20];
    if (otxn_field(SBUF(otxn_acc), sfAccount) != 20)
        NOPE("AIR :: Failed to get origin account.");

    // Pass through outgoing transactions
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc))
        DONE("AIR :: Outgoing transaction passed through.");

    // Get admin account from hook param
    uint8_t admin_acc[20];
    if (hook_param(SBUF(admin_acc), "ADMIN", 5) != 20)
        NOPE("AIR :: Error: ADMIN parameter not set.");

    // Check if invoker is admin
    if (!BUFFER_EQUAL_20(otxn_acc, admin_acc))
        DONE("AIR :: Non-admin invoke passed through.");

    // Get currency from hook param
    uint8_t currency[20];
    if (hook_param(SBUF(currency), "CURRENCY", 8) != 20)
        NOPE("AIR :: Error: CURRENCY parameter not set.");

    // Get issuer from hook param
    uint8_t issuer[20];
    if (hook_param(SBUF(issuer), "ISSUER", 6) != 20)
        NOPE("AIR :: Error: ISSUER parameter not set.");

    // Get AMT from invoke params
    uint8_t amt_buf[8];
    if (otxn_param(SBUF(amt_buf), "AMT", 3) != 8)
        NOPE("AIR :: Error: AMT parameter not provided.");

    // Get DEST from invoke params
    uint8_t dest_acc[20];
    if (otxn_param(SBUF(dest_acc), "DEST", 4) != 20)
        NOPE("AIR :: Error: DEST parameter not provided.");

    uint64_t amt_iou = UINT64_FROM_BUF(amt_buf);
    if (amt_iou == 0)
        NOPE("AIR :: Error: AMT must be positive.");

    // Convert to XFL format
    int64_t amount_xfl = float_set(0, amt_iou);

    // Build Amounts array
    uint8_t* amounts_ptr = AMOUNTS_OUT;
    
    *amounts_ptr++ = 0xF0U;  // sfAmounts array start
    *amounts_ptr++ = 0x5CU;
    
    *amounts_ptr++ = 0xE0U;  // sfAmountEntry object start
    *amounts_ptr++ = 0x5BU;
    
    int32_t amount_len = float_sto(
        amounts_ptr, 49,
        currency, 20,
        issuer, 20,
        amount_xfl,
        sfAmount
    );
    
    if (amount_len < 0)
        NOPE("AIR :: Error: Failed to serialize amount.");
    
    amounts_ptr += amount_len;
    
    *amounts_ptr++ = 0xE1U;  // End AmountEntry
    *amounts_ptr++ = 0xF1U;  // End Amounts array
    
    int32_t amounts_len = amounts_ptr - AMOUNTS_OUT;

    // Fill transaction fields
    hook_account(HOOK_ACC, 20);
    
    for (int i = 0; GUARD(20), i < 20; ++i)
        DEST_ACC[i] = dest_acc[i];

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
        NOPE("AIR :: Error: Fee calculation failed.");
    
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
    int64_t emit_result = emit(SBUF(emithash), txn, total_size);
    
    if (emit_result < 0)
        NOPE("AIR :: Error: Emit failed.");

    DONE("AIR :: IOU remitted successfully.");
}

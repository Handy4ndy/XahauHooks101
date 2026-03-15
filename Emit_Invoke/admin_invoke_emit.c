//**************************************************************
// Xahau Hook 101 Example ~ Admin Invoke Emit
// Author: @Handy_4ndy
//
// Description:
//   Allows a designated admin account (set at install-time) to trigger
//   an emitted Invoke transaction from the hook account by sending an
//   Invoke transaction with a DEST parameter.
//   Demonstrates canonical admin-controlled emission for Hooks 101.
//
// Triggers:
//   ttINVOKE (admin triggers emission)
//   ttPAYMENT (outgoing payments are accepted)
//
// Parameters:
//   'ADMIN' (20 bytes, install-time): Admin account ID allowed to emit.
//   'DEST' (20 bytes, runtime/invoke): Destination account for emission.
//
// Usage:
//   - Set 'ADMIN' during hook installation.
//   - Admin sends Invoke with 'DEST' to emit an Invoke transaction.
//   - Only the admin can trigger emission; others are ignored.
//
// Accepts:
//   - Invoke from admin with valid DEST.
//   - Outgoing payments.
//
// Rejects:
//   - Invoke from non-admin (passes through).
//   - Missing or invalid parameters.
//   - Emission failures.
//**************************************************************

#include "hookapi.h"

// clang-format off
uint8_t txn[234] =
{
/* size, upto, field name               */
/*    3,    0, tt = Invoke              */   0x12U, 0x00U, 0x63U,
/*    5,    3, flags                    */   0x22U, 0x00U, 0x00U, 0x00U, 0x00U,
/*    5,    8, sequence                 */   0x24U, 0x00U, 0x00U, 0x00U, 0x00U,
/*    5,   13, destinationtag           */   0x2EU, 0x00U, 0x00U, 0x00U, 0x00U,
/*    6,   18, firstledgersequence      */   0x20U, 0x1AU, 0x00U, 0x00U, 0x00U, 0x00U,
/*    6,   24, lastledgersequence       */   0x20U, 0x1BU, 0x00U, 0x00U, 0x00U, 0x00U,
/*    9,   30, fee                      */   0x68U, 0x40U, 0x00U, 0x00U, 0x00U, 0x00U, 0x00U, 0x01U, 0xF4U,
/*   35,   39, signingpubkey            */   0x73U, 0x21U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*   22,   74, account                  */   0x81U, 0x14U, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
/*   22,   96, destination              */   0x83U, 0x14U, 0x53U, 0x2EU, 0xD3U, 0xB0U, 0x7BU, 0x2FU, 0xB4U, 0x3EU, 0xA9U, 0x7AU, 0x71U, 0x58U, 0x1CU, 0x76U, 0xE5U, 0x65U, 0xEBU, 0xCCU, 0x15U, 0x14U,
/*  116,  118, emit details             */ 
/*    0,  234,                          */ 
};
// clang-format on

// TX BUILDER
#define FLAGS_OUT (txn + 4U)
#define DTAG_OUT (txn + 14U)
#define FLS_OUT (txn + 20U)
#define LLS_OUT (txn + 26U)
#define FEE_OUT (txn + 31U)
#define ACCOUNT_OUT (txn + 76U)
#define DEST_OUT (txn + 98U)
#define EMIT_OUT (txn + 118U)

#define FLIP_ENDIAN_32(value)                                       \
    (uint32_t)(((value & 0xFFU) << 24) | ((value & 0xFF00U) << 8) | \
               ((value & 0xFF0000U) >> 8) | ((value & 0xFF000000U) >> 24))

#define SET_UINT32(ptr, value) *((uint32_t *)(ptr)) = FLIP_ENDIAN_32(value);

#define SET_NATIVE_AMOUNT(ptr, amount)                     \
    do                                                     \
    {                                                      \
        uint8_t *b = (ptr);                                \
        *b++ = 0b01000000 + ((amount >> 56) & 0b00111111); \
        *b++ = (amount >> 48) & 0xFFU;                     \
        *b++ = (amount >> 40) & 0xFFU;                     \
        *b++ = (amount >> 32) & 0xFFU;                     \
        *b++ = (amount >> 24) & 0xFFU;                     \
        *b++ = (amount >> 16) & 0xFFU;                     \
        *b++ = (amount >> 8) & 0xFFU;                      \
        *b++ = (amount >> 0) & 0xFFU;                      \
    } while (0)

#define SET_ACCOUNT(ptr_to, ptr_from)                              \
    {                                                              \
        unsigned char *buf_to = (unsigned char *)ptr_to;           \
        unsigned char *buf_from = (unsigned char *)ptr_from;       \
        *(uint64_t *)(buf_to + 0) = *(uint64_t *)(buf_from + 0);   \
        *(uint64_t *)(buf_to + 8) = *(uint64_t *)(buf_from + 8);   \
        *(uint32_t *)(buf_to + 16) = *(uint32_t *)(buf_from + 16); \
    }

#define PREPARE_TXN()                              \
    do                                             \
    {                                              \
        etxn_reserve(1);                           \
        uint32_t fls = (uint32_t)ledger_seq() + 1; \
        SET_UINT32(FLS_OUT, fls);                  \
        SET_UINT32(LLS_OUT, fls + 4);              \
        hook_account(ACCOUNT_OUT, 20);             \
        etxn_details(EMIT_OUT, 116U);              \
        int64_t fee = etxn_fee_base(SBUF(txn));    \
        SET_NATIVE_AMOUNT(FEE_OUT, fee);           \
        TRACEHEX(txn);                             \
    } while (0)

int64_t hook(uint32_t reserved)
{
    TRACESTR("AIE :: Admin Invoke Emit :: Called.");

    // Get the hook account (this account)
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    // Get the originating account (sender of invoke/payment)
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    // Accept outgoing payments from the hook account
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("AIE :: Accepted :: Outgoing payment"), __LINE__);

    // Handle admin-controlled emission via ttINVOKE
    if (tt == 99)
    {
        // Fetch admin from install-time parameter
        uint8_t admin_acc[20];
        if (hook_param(SBUF(admin_acc), "ADMIN", 5) != 20)
            rollback(SBUF("AIE :: Error: ADMIN not set"), __LINE__);

        // Only allow admin to emit
        if (!BUFFER_EQUAL_20(otxn_acc, admin_acc))
            accept(SBUF("AIE :: Accepted :: Non-admin invoke passed through"), __LINE__);

        // Fetch DEST from invoke parameter
        uint8_t dest_acc[20];
        if (otxn_param(SBUF(dest_acc), "DEST", 4) != 20)
            rollback(SBUF("AIE :: Error: DEST not provided"), __LINE__);

        // Build and emit the Invoke transaction to DEST
        SET_ACCOUNT(ACCOUNT_OUT, hook_acc);
        SET_ACCOUNT(DEST_OUT, dest_acc);
        SET_UINT32(DTAG_OUT, 0);
        PREPARE_TXN();
        uint8_t emithash[32];
        int64_t emit_result = emit(SBUF(emithash), SBUF(txn));
        if (emit_result != 32)
            rollback(SBUF("AIE :: Error: Emit failed."), __LINE__);
        TRACEHEX(emithash);
        accept(SBUF("AIE :: Success :: Invoke emitted by admin."), __LINE__);
    }

    // Pass through all other transactions
    accept(SBUF("AIE :: Accepted :: Transaction passed through"), __LINE__);
    _g(1, 1);
    return 0;
}
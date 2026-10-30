//**************************************************************
// Xahau Hook 101 Example ~ Hardcoded Invoke Emit
// Author: @Handy_4ndy
//
// Description:
//   Emits a minimal, hardcoded Invoke transaction whenever an incoming
//   Payment transaction is detected. Demonstrates the simplest possible
//   emission pattern for Hooks 101.
//
// Triggers:
//   ttPAYMENT (incoming payments only)
//
// Parameters:
//   None (all values hardcoded for demonstration)
//
// Usage:
//   - Deploy the hook to your account.
//   - Any incoming XAH payment will trigger an emitted Invoke transaction.
//
// Accepts:
//   - All incoming XAH payments
//
// Emits:
//   - A hardcoded Invoke transaction to a fixed destination (for demo only)
//
// Notes:
//   - This file is for educational/demo purposes and is not parameterized.
//   - See other examples for install-time, runtime, or admin parameterization.
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
    // Log entry for debugging
    TRACESTR("HIE :: Hardcoded Invoke Emit :: Called");

    // Only trigger on Payment transactions
    int64_t tt = otxn_type();
    if (tt != 0) // 0 = ttPAYMENT
        accept(SBUF("HIE :: Success :: Not a payment, skipping."), __LINE__);

    // Get the Hook account (this account)
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));
    TRACEHEX(hook_acc);

    // Get the originating account (sender of payment)
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);
    TRACEHEX(otxn_acc);

    // NOTE: Destination is hardcoded in txn[] for demonstration only.
    // For parameterized or dynamic destinations, see other examples.

    // Set account to the hook account
    SET_ACCOUNT(ACCOUNT_OUT, hook_acc);

    // Set destination tag to 0
    SET_UINT32(DTAG_OUT, 0);

    // Prepare transaction (set FLS, LLS, fee, etc.)
    PREPARE_TXN();

    // Emit the invoke transaction
    uint8_t emithash[32];
    int64_t emit_result = emit(SBUF(emithash), SBUF(txn));
    if (emit_result != 32)
        rollback(SBUF("HIE :: Error :: Emit failed."), __LINE__);
    TRACEHEX(emithash);
    accept(SBUF("HIE :: Success :: Payment received, invoke emitted."), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
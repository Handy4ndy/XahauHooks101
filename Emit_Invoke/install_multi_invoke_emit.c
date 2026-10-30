//**************************************************************
// Xahau Hook 101 Example ~ IMIE :: Install-time Multi-Invoke Emit
// Author: @Handy_4ndy
//
// Description:
//   Emits three Invoke transactions, each to a destination specified by an install-time
//   hook parameter (DST1, DST2, DST3), whenever an incoming Payment transaction is detected.
//   This demonstrates canonical install-time multi-destination parameterization for Hooks 101.
//
// Triggers:
//   ttPAYMENT (incoming payments only)
//
// Parameters:
//   DST1 (20 bytes, required): First destination account
//   DST2 (20 bytes, required): Second destination account
//   DST3 (20 bytes, required): Third destination account
//     - All set at install time using the Hook Parameter interface.
//
// Usage:
//   - Deploy the hook to your account, specifying DST1, DST2, and DST3 parameters.
//   - Any incoming XAH payment will trigger three emitted Invoke transactions, one to each destination.
//
// Accepts:
//   - All incoming XAH payments
//
// Emits:
//   - Three Invoke transactions, one to each install-time DST parameter
//   - Fails if any DST parameter is not set or invalid
//
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
        etxn_reserve(3);                           \
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
    TRACESTR("IMIE :: Install-time Multi-Invoke Emit :: Called");

    // Only trigger on Payment transactions
    int64_t tt = otxn_type();
    if (tt != 0) // 0 = ttPAYMENT
        accept(SBUF("IMIE :: Success :: Not a payment, skipping."), __LINE__);

    // Get the Hook account (this account)
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));
    TRACEHEX(hook_acc);

    // Get the originating account (sender of payment)
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);
    TRACEHEX(otxn_acc);

    // Fetch DST1 parameter from install-time hook parameters
    uint8_t dst1_buf[20];
    if (hook_param(SBUF(dst1_buf), "DST1", 4) != 20)
        rollback(SBUF("IMIE :: Error :: DST1 parameter not set or invalid."), __LINE__);

    // Fetch DST2 parameter from install-time hook parameters
    uint8_t dst2_buf[20];
    if (hook_param(SBUF(dst2_buf), "DST2", 4) != 20)
        rollback(SBUF("IMIE :: Error :: DST2 parameter not set or invalid."), __LINE__);

    // Fetch DST3 parameter from install-time hook parameters
    uint8_t dst3_buf[20];
    if (hook_param(SBUF(dst3_buf), "DST3", 4) != 20)
        rollback(SBUF("IMIE :: Error :: DST3 parameter not set or invalid."), __LINE__);

    // Set account to the hook account
    SET_ACCOUNT(ACCOUNT_OUT, hook_acc);
    // Set destination tag to 0
    SET_UINT32(DTAG_OUT, 0);

    // Emit to DST1
    SET_ACCOUNT(DEST_OUT, dst1_buf);
    PREPARE_TXN();
    uint8_t emithash1[32];
    int64_t emit_result1 = emit(SBUF(emithash1), SBUF(txn));
    if (emit_result1 != 32)
        rollback(SBUF("IMIE :: Error :: Emit 1 failed."), __LINE__);
    TRACEHEX(emithash1);

    // Emit to DST2
    SET_ACCOUNT(DEST_OUT, dst2_buf);
    PREPARE_TXN();
    uint8_t emithash2[32];
    int64_t emit_result2 = emit(SBUF(emithash2), SBUF(txn));
    if (emit_result2 != 32)
        rollback(SBUF("IMIE :: Error :: Emit 2 failed."), __LINE__);
    TRACEHEX(emithash2);

    // Emit to DST3
    SET_ACCOUNT(DEST_OUT, dst3_buf);
    PREPARE_TXN();
    uint8_t emithash3[32];
    int64_t emit_result3 = emit(SBUF(emithash3), SBUF(txn));
    if (emit_result3 != 32)
        rollback(SBUF("IMIE :: Error :: Emit 3 failed."), __LINE__);
    TRACEHEX(emithash3);

    accept(SBUF("IMIE :: Success :: Payment received, 3 invokes emitted."), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
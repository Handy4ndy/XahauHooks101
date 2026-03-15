//**************************************************************
// Xahau Hook 101 Example ~ IMIE :: Invoke Multi Invoke Emit
// Author: @Handy_4ndy
//
// Description:
//   Emits up to three Invoke transactions, each to a destination specified at runtime
//   by the hook owner, using the DST1, DST2, and DST3 parameters. The hook owner can set or clear
//   DST1/2/3, or reset all, by sending an Invoke transaction with the appropriate parameters. Any
//   incoming Payment transaction will then emit to all currently set destinations. Demonstrates canonical
//   runtime/invoke multi-destination parameterization for Hooks 101.
//
// Triggers:
//   ttPAYMENT (incoming payments only)
//   ttINVOKE (for setting/clearing DST1/2/3 or resetting all by hook owner)
//
// Parameters:
//   DST1 (20 bytes, optional): First destination account
//   DST2 (20 bytes, optional): Second destination account
//   DST3 (20 bytes, optional): Third destination account
//   RSET (any, optional): If present, clears all destinations
//     - All set/cleared at runtime by the hook owner via Invoke transaction with parameters.
//
// Usage:
//   - Deploy the hook to your account.
//   - To set/clear DST1/2/3 or reset all, the hook owner sends an Invoke transaction with the appropriate parameters.
//   - Any incoming XAH payment will trigger up to three emitted Invoke transactions, one to each currently set destination.
//
// Accepts:
//   - All incoming XAH payments
//   - Only the hook owner can set/clear DST1/2/3 or reset via Invoke
//
// Emits:
//   - Up to three Invoke transactions, one to each runtime DST parameter
//   - Fails if no destinations are set
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
    TRACESTR("IMIE :: Invoke-time Multi-Invoke Emit :: Called");

    int64_t tt = otxn_type();

    // Get the Hook account (this account)
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));
    TRACEHEX(hook_acc);

    // Get the originating account (sender of payment or invoke)
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);
    TRACEHEX(otxn_acc);

    // If this is an Invoke transaction, allow the hook owner to set/clear DST1/2/3 or reset all
    if (tt == 99)
    { // 99 = ttINVOKE
        if (!BUFFER_EQUAL_20(hook_acc, otxn_acc))
        {
            rollback(SBUF("IMIE :: Error :: Only hook owner can set DST1/2/3 or reset."), __LINE__);
        }
        uint8_t rset_key[4] = {'R', 'S', 'E', 'T'};
        uint8_t dst1_key[4] = {'D', 'S', 'T', '1'};
        uint8_t dst2_key[4] = {'D', 'S', 'T', '2'};
        uint8_t dst3_key[4] = {'D', 'S', 'T', '3'};
        uint8_t buf[20];
        int64_t rset_len = otxn_param(SBUF(buf), SBUF(rset_key));
        if (rset_len >= 0)
        {
            // RSET present: clear all destinations
            for (int i = 0;GUARD(20), i < 20; ++i)
                buf[i] = 0;
            state_set(SBUF(buf), SBUF(dst1_key));
            state_set(SBUF(buf), SBUF(dst2_key));
            state_set(SBUF(buf), SBUF(dst3_key));
            accept(SBUF("IMIE :: Success :: All destinations reset."), __LINE__);
        }
        int set_any = 0;
        int64_t len;
        // DST1
        len = otxn_param(SBUF(buf), SBUF(dst1_key));
        if (len == 20)
        {
            state_set(SBUF(buf), SBUF(dst1_key));
            set_any = 1;
        }
        else if (len == 0)
        {
            // Clear DST1 if explicitly set to empty
            state_set(SBUF(buf), SBUF(dst1_key));
        }
        // DST2
        len = otxn_param(SBUF(buf), SBUF(dst2_key));
        if (len == 20)
        {
            state_set(SBUF(buf), SBUF(dst2_key));
            set_any = 1;
        }
        else if (len == 0)
        {
            state_set(SBUF(buf), SBUF(dst2_key));
        }
        // DST3
        len = otxn_param(SBUF(buf), SBUF(dst3_key));
        if (len == 20)
        {
            state_set(SBUF(buf), SBUF(dst3_key));
            set_any = 1;
        }
        else if (len == 0)
        {
            state_set(SBUF(buf), SBUF(dst3_key));
        }
        if (!set_any)
            rollback(SBUF("IMIE :: Error :: No DST set. At least one required."), __LINE__);
        accept(SBUF("IMIE :: Success :: DST1/2/3 updated in state."), __LINE__);
    }

    // Only process Payment transactions
    if (tt != 0) // 0 = ttPAYMENT
        accept(SBUF("IMIE :: Success :: Not a payment, skipping."), __LINE__);

    // Fetch DST1/2/3 from state, emit to each if set (not all zero)
    uint8_t dst1_buf[20], dst2_buf[20], dst3_buf[20];
    uint8_t dst1_key[4] = {'D', 'S', 'T', '1'};
    uint8_t dst2_key[4] = {'D', 'S', 'T', '2'};
    uint8_t dst3_key[4] = {'D', 'S', 'T', '3'};
    int emits = 0;
    // Set account to the hook account
    SET_ACCOUNT(ACCOUNT_OUT, hook_acc);
    // Set destination tag to 0
    SET_UINT32(DTAG_OUT, 0);

    if (state(SBUF(dst1_buf), SBUF(dst1_key)) == 20)
    {
        int is_zero = 1;
        for (int i = 0;GUARD(20), i < 20; ++i)
            if (dst1_buf[i] != 0)
            {
                is_zero = 0;
                break;
            }
        if (!is_zero)
        {
            SET_ACCOUNT(DEST_OUT, dst1_buf);
            PREPARE_TXN();
            uint8_t emithash1[32];
            int64_t emit_result1 = emit(SBUF(emithash1), SBUF(txn));
            if (emit_result1 != 32)
                rollback(SBUF("IMIE :: Error :: Emit 1 failed."), __LINE__);
            TRACEHEX(emithash1);
            emits++;
        }
    }
    if (state(SBUF(dst2_buf), SBUF(dst2_key)) == 20)
    {
        int is_zero = 1;
        for (int i = 0;GUARD(20), i < 20; ++i)
            if (dst2_buf[i] != 0)
            {
                is_zero = 0;
                break;
            }
        if (!is_zero)
        {
            SET_ACCOUNT(DEST_OUT, dst2_buf);
            PREPARE_TXN();
            uint8_t emithash2[32];
            int64_t emit_result2 = emit(SBUF(emithash2), SBUF(txn));
            if (emit_result2 != 32)
                rollback(SBUF("IMIE :: Error :: Emit 2 failed."), __LINE__);
            TRACEHEX(emithash2);
            emits++;
        }
    }
    if (state(SBUF(dst3_buf), SBUF(dst3_key)) == 20)
    {
        int is_zero = 1;
        for (int i = 0;GUARD(20), i < 20; ++i)
            if (dst3_buf[i] != 0)
            {
                is_zero = 0;
                break;
            }
        if (!is_zero)
        {
            SET_ACCOUNT(DEST_OUT, dst3_buf);
            PREPARE_TXN();
            uint8_t emithash3[32];
            int64_t emit_result3 = emit(SBUF(emithash3), SBUF(txn));
            if (emit_result3 != 32)
                rollback(SBUF("IMIE :: Error :: Emit 3 failed."), __LINE__);
            TRACEHEX(emithash3);
            emits++;
        }
    }
    if (emits == 0)
        rollback(SBUF("IMIE :: Error :: No destinations set."), __LINE__);
    accept(SBUF("IMIE :: Success :: Payment received, invokes emitted."), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Invoke IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits an IOU token remit to a user-defined account and amount,
//   set via invoke transaction state. Uses Remit for automatic trustline creation.
//   Hook owner uses Invoke to set parameters in state.
//   Incoming payments trigger remit based on state configuration.
//
// Parameters (set via Invoke):
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to remit (uint64_t)
//   'F_ACC' (20 bytes): Recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Key Benefits of Remit:
//   ✅ Automatic trustline creation (no pre-setup required)
//   ✅ Can send to accounts that don't exist yet
//   ✅ Dynamic configuration via Invoke
//
// Usage:
//   - Owner sends Invoke with desired parameters to set state.
//   - Incoming XAH payments matching 'AMT_IN' trigger IOU remit to 'F_ACC'.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Invoke from owner to set parameters.
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match AMT_IN.
//
// Rejects:
//   - Invoke from non-owner.
//   - Incoming XAH payments that don't match AMT_IN.
//   - Missing state configuration.
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
uint8_t txn[350] =
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
#define HOOK_ACC (txn + 71U)
#define DEST_ACC (txn + 93U)
#define EMIT_OUT (txn + 113U)
#define AMOUNTS_OUT (txn + 229U)

int64_t hook(uint32_t reserved)
{
    TRACESTR("IVOIR :: Invoke IOU Remit :: Called.");

    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        DONE("IVOIR :: Outgoing payment accepted.");

    // State keys
    uint8_t am_key[6] = {'A', 'M', 'T', '_', 'I', 'N'};
    uint8_t ao_key[7] = {'A', 'M', 'T', '_', 'O', 'U', 'T'};
    uint8_t ac_key[5] = {'F', '_', 'A', 'C', 'C'};
    uint8_t cur_key[8] = {'C', 'U', 'R', 'R', 'E', 'N', 'C', 'Y'};
    uint8_t iss_key[6] = {'I', 'S', 'S', 'U', 'E', 'R'};

    // Handle ttINVOKE for setting parameters
    if (tt == 99)
    {
        int equal = 0;
        BUFFER_EQUAL(equal, otxn_acc, hook_acc, 20);
        if (!equal)
            NOPE("IVOIR :: Error: Only hook owner can change settings.");

        uint8_t am_param[8];
        int8_t is_am = otxn_param(SBUF(am_param), SBUF(am_key));
        if (is_am > 0)
        {
            uint64_t am_value = UINT64_FROM_BUF(am_param);
            if (am_value <= 0)
                NOPE("IVOIR :: Error: AMT_IN must be positive.");
            if (state_set(SBUF(am_param), SBUF(am_key)) < 0)
                NOPE("IVOIR :: Error: Failed to set AMT_IN state.");
            TRACEVAR(am_value);
            DONE("IVOIR :: AMT_IN state set successfully.");
        }

        uint8_t ao_param[8];
        int8_t is_ao = otxn_param(SBUF(ao_param), SBUF(ao_key));
        if (is_ao > 0)
        {
            uint64_t ao_value = UINT64_FROM_BUF(ao_param);
            if (ao_value <= 0)
                NOPE("IVOIR :: Error: AMT_OUT must be positive.");
            if (state_set(SBUF(ao_param), SBUF(ao_key)) < 0)
                NOPE("IVOIR :: Error: Failed to set AMT_OUT state.");
            TRACEVAR(ao_value);
            DONE("IVOIR :: AMT_OUT state set successfully.");
        }

        uint8_t ac_param[20];
        int8_t is_ac = otxn_param(SBUF(ac_param), SBUF(ac_key));
        if (is_ac > 0)
        {
            if (BUFFER_EQUAL_20(ac_param, hook_acc))
                NOPE("IVOIR :: Error: F_ACC cannot match hook account.");
            if (state_set(SBUF(ac_param), SBUF(ac_key)) < 0)
                NOPE("IVOIR :: Error: Failed to set F_ACC state.");
            TRACEHEX(ac_param);
            DONE("IVOIR :: F_ACC state set successfully.");
        }

        uint8_t cur_param[20];
        int8_t is_cur = otxn_param(SBUF(cur_param), SBUF(cur_key));
        if (is_cur > 0)
        {
            if (state_set(SBUF(cur_param), SBUF(cur_key)) < 0)
                NOPE("IVOIR :: Error: Failed to set CURRENCY state.");
            DONE("IVOIR :: CURRENCY state set successfully.");
        }

        uint8_t iss_param[20];
        int8_t is_iss = otxn_param(SBUF(iss_param), SBUF(iss_key));
        if (is_iss > 0)
        {
            if (state_set(SBUF(iss_param), SBUF(iss_key)) < 0)
                NOPE("IVOIR :: Error: Failed to set ISSUER state.");
            DONE("IVOIR :: ISSUER state set successfully.");
        }

        NOPE("IVOIR :: Error: No valid parameters provided for invoke.");
    }

    // Handle ttPAYMENT
    if (tt == ttPAYMENT)
    {
        // Load state
        uint8_t am_state[8];
        if (state(SBUF(am_state), SBUF(am_key)) < 0)
            NOPE("IVOIR :: Error: AMT_IN state not set.");
        
        uint64_t exact_amount = UINT64_FROM_BUF(am_state);
        if (exact_amount <= 0)
            NOPE("IVOIR :: Error: AMT_IN must be positive.");

        uint8_t ao_state[8];
        if (state(SBUF(ao_state), SBUF(ao_key)) < 0)
            NOPE("IVOIR :: Error: AMT_OUT state not set.");
        
        uint64_t amount_out = UINT64_FROM_BUF(ao_state);
        if (amount_out <= 0)
            NOPE("IVOIR :: Error: AMT_OUT must be positive.");

        uint8_t ftxn_acc[20];
        if (state(SBUF(ftxn_acc), SBUF(ac_key)) < 0)
            NOPE("IVOIR :: Error: F_ACC state not set.");

        uint8_t currency[20];
        if (state(SBUF(currency), SBUF(cur_key)) < 0)
            NOPE("IVOIR :: Error: CURRENCY state not set.");

        uint8_t issuer[20];
        if (state(SBUF(issuer), SBUF(iss_key)) < 0)
            NOPE("IVOIR :: Error: ISSUER state not set.");

        if (BUFFER_EQUAL_20(ftxn_acc, hook_acc))
            NOPE("IVOIR :: Error: F_ACC cannot match hook account.");

        // Validate incoming payment
        uint8_t amount_buffer[8];
        int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
        int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
        int64_t amount_xfl = float_set(-6, otxn_drops);
        int64_t amount_int = float_int(amount_xfl, 0, 1);

        TRACEVAR(exact_amount);
        TRACEVAR(amount_int);

        if (amount_len != 8)
            NOPE("IVOIR :: Error: Non-XAH payment rejected.");

        uint64_t exact_amount_drops = exact_amount * 1000000;
        if (otxn_drops != exact_amount_drops)
            NOPE("IVOIR :: Error: Payment amount doesn't match AMT_IN.");

        // Convert IOU amount to XFL
        int64_t iou_amount_xfl = float_set(0, amount_out);

        // Build Amounts array
        uint8_t* amounts_ptr = AMOUNTS_OUT;
        
        *amounts_ptr++ = 0xF0U;
        *amounts_ptr++ = 0x5CU;
        *amounts_ptr++ = 0xE0U;
        *amounts_ptr++ = 0x5BU;
        
        int32_t amount_len_out = float_sto(
            amounts_ptr, 49,
            currency, 20,
            issuer, 20,
            iou_amount_xfl,
            sfAmount
        );
        
        if (amount_len_out < 0)
            NOPE("IVOIR :: Error: Failed to serialize IOU amount.");
        
        amounts_ptr += amount_len_out;
        *amounts_ptr++ = 0xE1U;
        *amounts_ptr++ = 0xF1U;
        
        int32_t amounts_len = amounts_ptr - AMOUNTS_OUT;

        // Fill transaction fields
        hook_account(HOOK_ACC, 20);
        for (int i = 0; GUARD(20), i < 20; ++i)
            DEST_ACC[i] = ftxn_acc[i];

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
            NOPE("IVOIR :: Error: Fee calculation failed.");
        
        uint64_t fee_tmp = fee;
        uint8_t* fee_ptr = (uint8_t*)&fee;
        *fee_ptr++ = 0b01000000 + ((fee_tmp >> 56) & 0b00111111);
        *fee_ptr++ = (fee_tmp >> 48) & 0xFFU;
        *fee_ptr++ = (fee_tmp >> 40) & 0xFFU;
        *fee_ptr++ = (fee_tmp >> 32) & 0xFFU;
        *fee_ptr++ = (fee_tmp >> 24) & 0xFFU;
        *fee_ptr++ = (fee_tmp >> 16) & 0xFFU;
        *fee_ptr++ = (fee_tmp >>  8) & 0xFFU;
        *fee_ptr++ = fee_tmp & 0xFFU;
        *((uint64_t*)(txn + 26)) = fee;
        
        uint8_t emithash[32];
        if (emit(SBUF(emithash), txn, total_size) < 0)
            NOPE("IVOIR :: Error: Failed to emit remit.");

        DONE("IVOIR :: Payment received and IOU remitted successfully.");
    }

    NOPE("IVOIR :: Error: Transaction type not supported.");
}

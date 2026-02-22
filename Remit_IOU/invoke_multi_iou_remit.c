//**************************************************************
// Xahau Hook 101 Example ~ Invoke Multi IOU Remit
// Author: @Handy_4ndy
//
// Description:
//   Emits IOU token remits to multiple accounts with amounts set via invoke transaction state. Uses Remit for automatic trustline creation. Incoming payments trigger remits to both recipients based on state configuration.
//
// Triggers:
//   ttINVOKE (owner sets state)
//   ttPAYMENT (incoming, exact amount and outgoing only)
//
// Parameters (set via Invoke):
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to remit (uint64_t)
//   'F_ACC1' (20 bytes): First recipient account (AccountID)
//   'F_ACC2' (20 bytes): Second recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Usage:
//   - Owner sends Invoke with desired parameters to set state.
//   - Incoming XAH payments matching 'AMT_IN' trigger IOU remits to both accounts.
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
    TRACESTR("IVMIR :: Invoke Multi IOU Remit :: Called.");

    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        DONE("IVMIR :: Outgoing payment accepted.");

    // State keys
    uint8_t am_key[6] = {'A', 'M', 'T', '_', 'I', 'N'};
    uint8_t ao_key[7] = {'A', 'M', 'T', '_', 'O', 'U', 'T'};
    uint8_t ac1_key[6] = {'F', '_', 'A', 'C', 'C', '1'};
    uint8_t ac2_key[6] = {'F', '_', 'A', 'C', 'C', '2'};
    uint8_t cur_key[8] = {'C', 'U', 'R', 'R', 'E', 'N', 'C', 'Y'};
    uint8_t iss_key[6] = {'I', 'S', 'S', 'U', 'E', 'R'};

    // Handle ttINVOKE
    if (tt == 99)
    {
        int equal = 0;
        BUFFER_EQUAL(equal, otxn_acc, hook_acc, 20);
        if (!equal)
            NOPE("IVMIR :: Error: Only hook owner can change settings.");

        uint8_t param[20];
        int8_t is_set;

        // Set AMT_IN
        is_set = otxn_param(SBUF(param), SBUF(am_key));
        if (is_set == 8)
        {
            uint64_t val = UINT64_FROM_BUF(param);
            if (val <= 0)
                NOPE("IVMIR :: Error: AMT_IN must be positive.");
            if (state_set(param, 8, SBUF(am_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set AMT_IN.");
            DONE("IVMIR :: AMT_IN set successfully.");
        }

        // Set AMT_OUT
        is_set = otxn_param(SBUF(param), SBUF(ao_key));
        if (is_set == 8)
        {
            uint64_t val = UINT64_FROM_BUF(param);
            if (val <= 0)
                NOPE("IVMIR :: Error: AMT_OUT must be positive.");
            if (state_set(param, 8, SBUF(ao_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set AMT_OUT.");
            DONE("IVMIR :: AMT_OUT set successfully.");
        }

        // Set F_ACC1
        is_set = otxn_param(SBUF(param), SBUF(ac1_key));
        if (is_set == 20)
        {
            if (BUFFER_EQUAL_20(param, hook_acc))
                NOPE("IVMIR :: Error: F_ACC1 cannot match hook account.");
            if (state_set(SBUF(param), SBUF(ac1_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set F_ACC1.");
            DONE("IVMIR :: F_ACC1 set successfully.");
        }

        // Set F_ACC2
        is_set = otxn_param(SBUF(param), SBUF(ac2_key));
        if (is_set == 20)
        {
            if (BUFFER_EQUAL_20(param, hook_acc))
                NOPE("IVMIR :: Error: F_ACC2 cannot match hook account.");
            if (state_set(SBUF(param), SBUF(ac2_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set F_ACC2.");
            DONE("IVMIR :: F_ACC2 set successfully.");
        }

        // Set CURRENCY
        is_set = otxn_param(SBUF(param), SBUF(cur_key));
        if (is_set == 20)
        {
            if (state_set(SBUF(param), SBUF(cur_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set CURRENCY.");
            DONE("IVMIR :: CURRENCY set successfully.");
        }

        // Set ISSUER
        is_set = otxn_param(SBUF(param), SBUF(iss_key));
        if (is_set == 20)
        {
            if (state_set(SBUF(param), SBUF(iss_key)) < 0)
                NOPE("IVMIR :: Error: Failed to set ISSUER.");
            DONE("IVMIR :: ISSUER set successfully.");
        }

        NOPE("IVMIR :: Error: No valid parameters provided.");
    }

    // Handle ttPAYMENT
    if (tt == ttPAYMENT)
    {
        // Load all state
        uint8_t am_state[8];
        if (state(SBUF(am_state), SBUF(am_key)) < 0)
            NOPE("IVMIR :: Error: AMT_IN not set.");
        uint64_t exact_amount = UINT64_FROM_BUF(am_state);

        uint8_t ao_state[8];
        if (state(SBUF(ao_state), SBUF(ao_key)) < 0)
            NOPE("IVMIR :: Error: AMT_OUT not set.");
        uint64_t amount_out = UINT64_FROM_BUF(ao_state);

        uint8_t ftxn_acc1[20];
        if (state(SBUF(ftxn_acc1), SBUF(ac1_key)) < 0)
            NOPE("IVMIR :: Error: F_ACC1 not set.");

        uint8_t ftxn_acc2[20];
        if (state(SBUF(ftxn_acc2), SBUF(ac2_key)) < 0)
            NOPE("IVMIR :: Error: F_ACC2 not set.");

        uint8_t currency[20];
        if (state(SBUF(currency), SBUF(cur_key)) < 0)
            NOPE("IVMIR :: Error: CURRENCY not set.");

        uint8_t issuer[20];
        if (state(SBUF(issuer), SBUF(iss_key)) < 0)
            NOPE("IVMIR :: Error: ISSUER not set.");

        if (BUFFER_EQUAL_20(ftxn_acc1, ftxn_acc2))
            NOPE("IVMIR :: Error: F_ACC1 and F_ACC2 cannot match.");

        // Validate payment
        uint8_t amount_buffer[8];
        int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
        int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);

        if (amount_len != 8)
            NOPE("IVMIR :: Error: Non-XAH payment rejected.");

        uint64_t exact_amount_drops = exact_amount * 1000000;
        if (otxn_drops != exact_amount_drops)
            NOPE("IVMIR :: Error: Payment amount doesn't match AMT_IN.");

        etxn_reserve(2);

        int64_t iou_amount_xfl = float_set(0, amount_out);

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
        int32_t amt_len1 = float_sto(amounts_ptr1, 49, currency, 20, issuer, 20, iou_amount_xfl, sfAmount);
        if (amt_len1 < 0)
            NOPE("IVMIR :: Error: Failed to serialize amount 1.");
        amounts_ptr1 += amt_len1;
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
            NOPE("IVMIR :: Error: Fee calc failed for txn1.");
        uint64_t ft1 = fee1;
        uint8_t* fp1 = (uint8_t*)&fee1;
        *fp1++ = 0b01000000 + ((ft1 >> 56) & 0b00111111);
        *fp1++ = (ft1 >> 48) & 0xFFU;
        *fp1++ = (ft1 >> 40) & 0xFFU;
        *fp1++ = (ft1 >> 32) & 0xFFU;
        *fp1++ = (ft1 >> 24) & 0xFFU;
        *fp1++ = (ft1 >> 16) & 0xFFU;
        *fp1++ = (ft1 >>  8) & 0xFFU;
        *fp1++ = ft1 & 0xFFU;
        *((uint64_t*)(txn1 + 26)) = fee1;

        // Build second transaction
        uint8_t* amounts_ptr2 = txn2 + 229U;
        *amounts_ptr2++ = 0xF0U;
        *amounts_ptr2++ = 0x5CU;
        *amounts_ptr2++ = 0xE0U;
        *amounts_ptr2++ = 0x5BU;
        int32_t amt_len2 = float_sto(amounts_ptr2, 49, currency, 20, issuer, 20, iou_amount_xfl, sfAmount);
        if (amt_len2 < 0)
            NOPE("IVMIR :: Error: Failed to serialize amount 2.");
        amounts_ptr2 += amt_len2;
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
            NOPE("IVMIR :: Error: Fee calc failed for txn2.");
        uint64_t ft2 = fee2;
        uint8_t* fp2 = (uint8_t*)&fee2;
        *fp2++ = 0b01000000 + ((ft2 >> 56) & 0b00111111);
        *fp2++ = (ft2 >> 48) & 0xFFU;
        *fp2++ = (ft2 >> 40) & 0xFFU;
        *fp2++ = (ft2 >> 32) & 0xFFU;
        *fp2++ = (ft2 >> 24) & 0xFFU;
        *fp2++ = (ft2 >> 16) & 0xFFU;
        *fp2++ = (ft2 >>  8) & 0xFFU;
        *fp2++ = ft2 & 0xFFU;
        *((uint64_t*)(txn2 + 26)) = fee2;

        uint8_t emithash1[32];
        uint8_t emithash2[32];

        if (emit(SBUF(emithash1), txn1, total_size1) < 0 ||
            emit(SBUF(emithash2), txn2, total_size2) < 0)
            NOPE("IVMIR :: Error: Failed to emit remit transactions.");

        DONE("IVMIR :: Payment received and IOU remitted to multiple accounts successfully.");
    }

    NOPE("IVMIR :: Error: Transaction type not supported.");
}

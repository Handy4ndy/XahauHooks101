//**************************************************************
// Xahau Hook 101 Example ~ Invoke Multi Native Emit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits native payments to up to 3 user-defined accounts and amounts, set via invoke parameters.
//   Accepts outgoing payments and IOU payments.
//   Only emits when the incoming XAH payment matches the exact amount set by invoke parameter.
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The amount to emit (XAH, uint64_t)
//   'F_ACC1' (20 bytes): First recipient account (AccountID)
//   'F_ACC2' (20 bytes): Second recipient account (AccountID)
//   'F_ACC3' (20 bytes): Third recipient account (AccountID)
//
// Usage:
//   - Invoke from hook owner to set 'AMT_IN', 'AMT_OUT', 'F_ACC1', 'F_ACC2', 'F_ACC3'.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger emits to configured recipients for 'AMT_OUT' XAH each.
//   - [Hooks Services](https://hooks.services/tools)
//
// Accepts:
//   - Invoke transactions from hook owner to set parameters.
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match the exact amount set by invoke parameter.
//
// Rejects:
//   - Invoke transactions not from the hook owner.
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

// Macro to convert buffer to uint64_t
#define UINT64_FROM_BUF(buf)                                   \
    (((uint64_t)(buf)[0] << 56) + ((uint64_t)(buf)[1] << 48) + \
     ((uint64_t)(buf)[2] << 40) + ((uint64_t)(buf)[3] << 32) + \
     ((uint64_t)(buf)[4] << 24) + ((uint64_t)(buf)[5] << 16) + \
     ((uint64_t)(buf)[6] << 8) + (uint64_t)(buf)[7])

// Macro to convert uint64_t to buffer
#define UINT64_TO_BUF(buf, i)         \
    {                                 \
        (buf)[0] = (i >> 56) & 0xFFU; \
        (buf)[1] = (i >> 48) & 0xFFU; \
        (buf)[2] = (i >> 40) & 0xFFU; \
        (buf)[3] = (i >> 32) & 0xFFU; \
        (buf)[4] = (i >> 24) & 0xFFU; \
        (buf)[5] = (i >> 16) & 0xFFU; \
        (buf)[6] = (i >> 8) & 0xFFU;  \
        (buf)[7] = i & 0xFFU;         \
    }

// State keys for F_ACC
#define F_ACC1_KEY 0x465F414343310000ULL // "F_ACC1"
#define F_ACC2_KEY 0x465F414343320000ULL // "F_ACC2"
#define F_ACC3_KEY 0x465F414343330000ULL // "F_ACC3"

int64_t hook(uint32_t reserved)
{
    TRACESTR("IMNE :: Invoke Multi Native Emit :: Called.");

    // Hook and origin accounts
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("IMNE :: Accepted :: Outgoing payment accepted"), __LINE__);

    // State keys for AMT_IN and AMT_OUT
    uint8_t amt_in_key[6] = {'A', 'M', 'T', '_', 'I', 'N'};
    uint8_t amt_out_key[7] = {'A', 'M', 'T', '_', 'O', 'U', 'T'};

    // Parameters for invoke transactions
    uint8_t amt_in_param[8];
    int8_t is_amt_in = otxn_param(SBUF(amt_in_param), SBUF(amt_in_key));

    uint8_t amt_out_param[8];
    int8_t is_amt_out = otxn_param(SBUF(amt_out_param), SBUF(amt_out_key));

    // Handle ttINVOKE (type 99) for setting parameters
    if (tt == 99)
    {
        // Verify hook owner
        int equal = 0;
        BUFFER_EQUAL(equal, otxn_acc, hook_acc, 20);
        if (!equal)
            rollback(SBUF("IMNE :: Error: Only hook owner can change settings"), __LINE__);

        int params_set = 0;

        // Set AMT_IN state
        if (is_amt_in > 0)
        {
            uint64_t amt_in_value = UINT64_FROM_BUF(amt_in_param);
            if (amt_in_value <= 0)
                rollback(SBUF("IMNE :: Error: AMT_IN must be positive"), __LINE__);
            if (state_set(SBUF(amt_in_param), SBUF(amt_in_key)) < 0)
                rollback(SBUF("IMNE :: Error: Failed to set AMT_IN state"), __LINE__);
            TRACEVAR(amt_in_value);
            params_set = 1;
            accept(SBUF("IMNE :: Accepted :: AMT_IN state set successfully"), __LINE__);
        }

        // Set AMT_OUT state
        if (is_amt_out > 0)
        {
            uint64_t amt_out_value = UINT64_FROM_BUF(amt_out_param);
            if (amt_out_value <= 0)
                rollback(SBUF("IMNE :: Error: AMT_OUT must be positive"), __LINE__);
            if (state_set(SBUF(amt_out_param), SBUF(amt_out_key)) < 0)
                rollback(SBUF("IMNE :: Error: Failed to set AMT_OUT state"), __LINE__);
            TRACEVAR(amt_out_value);
            params_set = 1;
            accept(SBUF("IMNE :: Accepted :: AMT_OUT state set successfully"), __LINE__);
        }

        // Set F_ACC1 state
        uint8_t facc_param[20];
        if (otxn_param(SBUF(facc_param), "F_ACC1", 6) == 20)
        {
            uint8_t key_buf[8];
            UINT64_TO_BUF(key_buf, F_ACC1_KEY);
            if (state_set(SBUF(facc_param), SBUF(key_buf)) < 0)
                rollback(SBUF("IMNE :: Error: Failed to set F_ACC1 state"), __LINE__);
            TRACEHEX(facc_param);
            params_set = 1;
        }

        // Set F_ACC2 state
        if (otxn_param(SBUF(facc_param), "F_ACC2", 6) == 20)
        {
            uint8_t key_buf[8];
            UINT64_TO_BUF(key_buf, F_ACC2_KEY);
            if (state_set(SBUF(facc_param), SBUF(key_buf)) < 0)
                rollback(SBUF("IMNE :: Error: Failed to set F_ACC2 state"), __LINE__);
            TRACEHEX(facc_param);
            params_set = 1;
        }

        // Set F_ACC3 state
        if (otxn_param(SBUF(facc_param), "F_ACC3", 6) == 20)
        {
            uint8_t key_buf[8];
            UINT64_TO_BUF(key_buf, F_ACC3_KEY);
            if (state_set(SBUF(facc_param), SBUF(key_buf)) < 0)
                rollback(SBUF("IMNE :: Error: Failed to set F_ACC3 state"), __LINE__);
            TRACEHEX(facc_param);
            params_set = 1;
        }

        if (params_set)
            accept(SBUF("IMNE :: Accepted :: Parameters updated successfully"), __LINE__);
        else
            rollback(SBUF("IMNE :: Error: No valid parameters provided for invoke"), __LINE__);
    }

    // Handle ttPAYMENT (type 0)
    if (tt == ttPAYMENT)
    {
        // Load parameters from state
        uint8_t amt_in_state[8];
        int64_t exact_amount;
        if (state(SBUF(amt_in_state), SBUF(amt_in_key)) < 0)
            rollback(SBUF("IMNE :: Error: AMT_IN state not set"), __LINE__);
        exact_amount = UINT64_FROM_BUF(amt_in_state);
        if (exact_amount <= 0)
            rollback(SBUF("IMNE :: Error: AMT_IN must be positive"), __LINE__);

        uint8_t amt_out_state[8];
        int64_t amount_out;
        if (state(SBUF(amt_out_state), SBUF(amt_out_key)) < 0)
            rollback(SBUF("IMNE :: Error: AMT_OUT state not set"), __LINE__);
        amount_out = UINT64_FROM_BUF(amt_out_state);
        if (amount_out <= 0)
            rollback(SBUF("IMNE :: Error: AMT_OUT must be positive"), __LINE__);

        // Check incoming payment amount
        unsigned char amount_buffer[48];
        int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
        int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
        if (amount_len != 8)
            rollback(SBUF("IMNE :: Error: Non-XAH payment rejected"), __LINE__);
        int64_t exact_amount_drops = exact_amount * 1000000;
        if (otxn_drops != exact_amount_drops)
            rollback(SBUF("IMNE :: Error: Payment amount doesn't match AMT_IN"), __LINE__);

        // Load F_ACC states
        uint8_t f_acc[3][20];
        int configured_accounts = 0;

        uint8_t key_buf[8];
        UINT64_TO_BUF(key_buf, F_ACC1_KEY);
        if (state(SBUF(f_acc[0]), SBUF(key_buf)) == 20)
            configured_accounts++;

        UINT64_TO_BUF(key_buf, F_ACC2_KEY);
        if (state(SBUF(f_acc[1]), SBUF(key_buf)) == 20)
            configured_accounts++;

        UINT64_TO_BUF(key_buf, F_ACC3_KEY);
        if (state(SBUF(f_acc[2]), SBUF(key_buf)) == 20)
            configured_accounts++;

        if (configured_accounts == 0)
            rollback(SBUF("IMNE :: Error: No valid destination accounts set"), __LINE__);

        etxn_reserve(configured_accounts);

        for (int i = 0; GUARD(60), i < configured_accounts; ++i)
        {
            // Prepare payment transaction
            uint8_t txn[PREPARE_PAYMENT_SIMPLE_SIZE];
            int64_t amount_out_drops = amount_out * 1000000;
            PREPARE_PAYMENT_SIMPLE(txn, amount_out_drops, f_acc[i], 0, 0);
            // Emit the transaction
            uint8_t emithash[32];
            if (emit(SBUF(emithash), SBUF(txn)) != 32)
                rollback(SBUF("IMNE :: Error: Failed to emit transaction"), __LINE__);
        }
        accept(SBUF("IMNE :: Accepted :: Payment received and forwarded to all accounts successfully"), __LINE__);
    }

    rollback(SBUF("IMNE :: Error: Transaction type not supported"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}

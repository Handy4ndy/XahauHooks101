//**************************************************************
// Xahau Hook 101 Example ~ Invoke Multi IOU Emit
// Author: @Handy_4ndy
//
// Description:
//   This hook emits IOU token payments to up to 3 user-defined accounts and amounts, set via invoke parameters.
//   Accepts outgoing payments and IOU payments.
//   Only emits when the incoming XAH payment matches the exact amount set by invoke parameter.
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to emit (uint64_t)
//   'F_ACC1' (20 bytes): First recipient account (AccountID)
//   'F_ACC2' (20 bytes): Second recipient account (AccountID)
//   'F_ACC3' (20 bytes): Third recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Usage:
//   - Invoke from hook owner to set 'AMT_IN', 'AMT_OUT', 'F_ACC1', 'F_ACC2', 'F_ACC3', 'CURRENCY', 'ISSUER'.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger IOU emits to configured recipients.
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

int64_t hook(uint32_t reserved)
{
    TRACESTR("IMIE :: Invoke Multi IOU Emit :: Called.");

    // Hook and origin accounts
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("IMIE :: Accepted :: Outgoing payment accepted"), __LINE__);

    // Parameters for invoke transactions
    uint8_t amt_in_param[8];
    uint8_t amt_in_key[6] = {'A', 'M', 'T', '_', 'I', 'N'};
    int8_t is_amt_in = otxn_param(SBUF(amt_in_param), SBUF(amt_in_key));

    uint8_t amt_out_param[8];
    uint8_t amt_out_key[7] = {'A', 'M', 'T', '_', 'O', 'U', 'T'};
    int8_t is_amt_out = otxn_param(SBUF(amt_out_param), SBUF(amt_out_key));

    uint8_t facc1_param[20];
    uint8_t facc1_key[6] = {'F', '_', 'A', 'C', 'C', '1'};
    int8_t is_facc1 = otxn_param(SBUF(facc1_param), SBUF(facc1_key));

    uint8_t facc2_param[20];
    uint8_t facc2_key[6] = {'F', '_', 'A', 'C', 'C', '2'};
    int8_t is_facc2 = otxn_param(SBUF(facc2_param), SBUF(facc2_key));

    uint8_t facc3_param[20];
    uint8_t facc3_key[6] = {'F', '_', 'A', 'C', 'C', '3'};
    int8_t is_facc3 = otxn_param(SBUF(facc3_param), SBUF(facc3_key));

    uint8_t cur_param[20];
    uint8_t cur_key[8] = {'C', 'U', 'R', 'R', 'E', 'N', 'C', 'Y'};
    int8_t is_cur = otxn_param(SBUF(cur_param), SBUF(cur_key));

    uint8_t iss_param[20];
    uint8_t iss_key[6] = {'I', 'S', 'S', 'U', 'E', 'R'};
    int8_t is_iss = otxn_param(SBUF(iss_param), SBUF(iss_key));

    // Handle ttINVOKE (type 99) for setting parameters
    if (tt == 99)
    {
        // Verify hook owner
        int equal = 0;
        BUFFER_EQUAL(equal, otxn_acc, hook_acc, 20);
        if (!equal)
            rollback(SBUF("IMIE :: Error: Only hook owner can change settings"), __LINE__);

        // Set AMT_IN state
        if (is_amt_in > 0)
        {
            uint64_t amt_in_value = UINT64_FROM_BUF(amt_in_param);
            if (amt_in_value <= 0)
                rollback(SBUF("IMIE :: Error: AMT_IN must be positive"), __LINE__);
            if (state_set(SBUF(amt_in_param), SBUF(amt_in_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set AMT_IN state"), __LINE__);
            TRACEVAR(amt_in_value);
            accept(SBUF("IMIE :: Accepted :: AMT_IN state set successfully"), __LINE__);
        }

        // Set AMT_OUT state
        if (is_amt_out > 0)
        {
            uint64_t amt_out_value = UINT64_FROM_BUF(amt_out_param);
            if (amt_out_value <= 0)
                rollback(SBUF("IMIE :: Error: AMT_OUT must be positive"), __LINE__);
            if (state_set(SBUF(amt_out_param), SBUF(amt_out_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set AMT_OUT state"), __LINE__);
            TRACEVAR(amt_out_value);
            accept(SBUF("IMIE :: Accepted :: AMT_OUT state set successfully"), __LINE__);
        }

        // Set F_ACC1 state
        if (is_facc1 > 0)
        {
            if (state_set(SBUF(facc1_param), SBUF(facc1_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set F_ACC1 state"), __LINE__);
            TRACEHEX(facc1_param);
            accept(SBUF("IMIE :: Accepted :: F_ACC1 state set successfully"), __LINE__);
        }

        // Set F_ACC2 state
        if (is_facc2 > 0)
        {
            if (state_set(SBUF(facc2_param), SBUF(facc2_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set F_ACC2 state"), __LINE__);
            TRACEHEX(facc2_param);
            accept(SBUF("IMIE :: Accepted :: F_ACC2 state set successfully"), __LINE__);
        }

        // Set F_ACC3 state
        if (is_facc3 > 0)
        {
            if (state_set(SBUF(facc3_param), SBUF(facc3_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set F_ACC3 state"), __LINE__);
            TRACEHEX(facc3_param);
            accept(SBUF("IMIE :: Accepted :: F_ACC3 state set successfully"), __LINE__);
        }

        // Set CURRENCY state
        if (is_cur > 0)
        {
            if (state_set(SBUF(cur_param), SBUF(cur_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set CURRENCY state"), __LINE__);
            accept(SBUF("IMIE :: Accepted :: CURRENCY state set successfully"), __LINE__);
        }

        // Set ISSUER state
        if (is_iss > 0)
        {
            if (state_set(SBUF(iss_param), SBUF(iss_key)) < 0)
                rollback(SBUF("IMIE :: Error: Failed to set ISSUER state"), __LINE__);
            accept(SBUF("IMIE :: Accepted :: ISSUER state set successfully"), __LINE__);
        }

        rollback(SBUF("IMIE :: Error: No valid parameters provided for invoke"), __LINE__);
    }

    // Handle ttPAYMENT (type 0)
    if (tt == ttPAYMENT)
    {
        // Load parameters from state
        uint8_t amt_in_state[8];
        int64_t exact_amount;
        if (state(SBUF(amt_in_state), SBUF(amt_in_key)) < 0)
            rollback(SBUF("IMIE :: Error: AMT_IN state not set"), __LINE__);
        exact_amount = UINT64_FROM_BUF(amt_in_state);
        if (exact_amount <= 0)
            rollback(SBUF("IMIE :: Error: AMT_IN must be positive"), __LINE__);

        uint8_t amt_out_state[8];
        int64_t amount_out;
        if (state(SBUF(amt_out_state), SBUF(amt_out_key)) < 0)
            rollback(SBUF("IMIE :: Error: AMT_OUT state not set"), __LINE__);
        amount_out = UINT64_FROM_BUF(amt_out_state);
        if (amount_out <= 0)
            rollback(SBUF("IMIE :: Error: AMT_OUT must be positive"), __LINE__);

        uint8_t currency[20];
        if (state(SBUF(currency), SBUF(cur_key)) < 0)
            rollback(SBUF("IMIE :: Error: CURRENCY state not set"), __LINE__);

        uint8_t issuer[20];
        if (state(SBUF(issuer), SBUF(iss_key)) < 0)
            rollback(SBUF("IMIE :: Error: ISSUER state not set"), __LINE__);

        // Count configured accounts
        int configured_accounts = 0;
        uint8_t facc1[20] = {0};
        uint8_t facc2[20] = {0};
        uint8_t facc3[20] = {0};

        if (state(SBUF(facc1), SBUF(facc1_key)) == 20)
            configured_accounts++;
        if (state(SBUF(facc2), SBUF(facc2_key)) == 20)
            configured_accounts++;
        if (state(SBUF(facc3), SBUF(facc3_key)) == 20)
            configured_accounts++;

        if (configured_accounts == 0)
            rollback(SBUF("IMIE :: Error: No recipient accounts configured"), __LINE__);

        // Validate incoming payment
        uint8_t amount_buffer[8];
        int64_t amount_len = otxn_field(SBUF(amount_buffer), sfAmount);
        int64_t otxn_drops = AMOUNT_TO_DROPS(amount_buffer);
        int64_t amount_xfl = float_set(-6, otxn_drops);
        int64_t amount_int = float_int(amount_xfl, 0, 1);

        TRACEVAR(exact_amount);
        TRACEVAR(amount_int);

        // Ensure the payment is XAH
        if (amount_len != 8)
            rollback(SBUF("IMIE :: Error: Non-XAH payment rejected"), __LINE__);

        uint64_t exact_amount_drops = exact_amount * 1000000;

        // Check if payment matches exact_amount (drops)
        if (otxn_drops != exact_amount_drops)
            rollback(SBUF("IMIE :: Error: Payment amount doesn't match AMT_IN"), __LINE__);

        // Reserve space for emitted transactions
        etxn_reserve(configured_accounts);

        // Prepare IOU amount in XFL format
        int64_t amount_xfl = float_set(0, amount_out); // Convert uint64 to XFL
        uint8_t amt_out[48];
        if (float_sto(amt_out - 1, 49, SBUF(currency), SBUF(issuer), amount_xfl, sfAmount) < 0)
            rollback(SBUF("IMIE :: Error: Failed to serialize IOU amount"), __LINE__);

        // Emit to configured accounts
        int emitted = 0;
        uint8_t emithash[32];

        if (BUFFER_EQUAL_20(facc1, hook_acc) == 0 && facc1[0] != 0)
        {
            uint8_t txn1[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
            PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn1, amt_out, facc1, 0, 0);
            if (emit(SBUF(emithash), SBUF(txn1)) == 32)
                emitted++;
        }

        if (BUFFER_EQUAL_20(facc2, hook_acc) == 0 && facc2[0] != 0)
        {
            uint8_t txn2[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
            PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn2, amt_out, facc2, 0, 0);
            if (emit(SBUF(emithash), SBUF(txn2)) == 32)
                emitted++;
        }

        if (BUFFER_EQUAL_20(facc3, hook_acc) == 0 && facc3[0] != 0)
        {
            uint8_t txn3[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
            PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn3, amt_out, facc3, 0, 0);
            if (emit(SBUF(emithash), SBUF(txn3)) == 32)
                emitted++;
        }

        if (emitted == 0)
            rollback(SBUF("IMIE :: Error: Failed to emit any IOU transactions"), __LINE__);

        accept(SBUF("IMIE :: Accepted : Payment received and IOU forwarded successfully"), __LINE__);
    }

    rollback(SBUF("IMIE :: Error: Transaction type not supported"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
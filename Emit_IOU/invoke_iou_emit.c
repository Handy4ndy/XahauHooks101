//**************************************************************
// Xahau Hook 101 Example ~ Invoke IOU Emit
// Author: @Handy_4ndy
//
// Description:
//   Emits an IOU token payment to a user-defined account and amount, set via invoke parameters (state). Only emits when the incoming XAH payment matches the exact amount set by invoke parameter.
//
// Triggers:
//   ttPAYMENT (incoming payment of exact amount)
//   ttINVOKE (to set parameters)
//
// Parameters:
//   'AMT_IN' (8 bytes): The exact amount to receive (XAH, uint64_t)
//   'AMT_OUT' (8 bytes): The IOU amount to emit (uint64_t)
//   'F_ACC' (20 bytes): Recipient account (AccountID)
//   'CURRENCY' (20 bytes): IOU currency code
//   'ISSUER' (20 bytes): IOU issuer account
//
// Usage:
//   - Set 'AMT_IN', 'AMT_OUT', 'F_ACC', 'CURRENCY', and 'ISSUER' using an invoke transaction.
//   - Incoming XAH payments must match 'AMT_IN' exactly to trigger the IOU emit to 'F_ACC'.
//   - Use [Hooks Services](https://hooks.services/tools) for testing.
//
// Accepts:
//   - Outgoing payments.
//   - Incoming IOU payments.
//   - Incoming XAH payments that match the exact amount set by invoke parameter.
//
// Rejects:
//   - Incoming XAH payments that do not match the exact amount.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved)
{
    TRACESTR("IIE :: Invoke IOU Emit :: Called.");

    // Hook and origin accounts
    uint8_t hook_acc[20];
    hook_account(hook_acc, 20);

    uint8_t otxn_acc[20];
    otxn_field(otxn_acc, 20, sfAccount);

    // Transaction type
    int64_t tt = otxn_type();

    if (BUFFER_EQUAL_20(hook_acc, otxn_acc) && tt == ttPAYMENT)
        accept(SBUF("IIE :: Accepted : Outgoing payment transaction accepted"), __LINE__);

    // Parameters for invoke transactions
    uint8_t am_param[8];
    uint8_t am_key[6] = {'A', 'M', 'T', '_', 'I', 'N'};
    int8_t is_am = otxn_param(SBUF(am_param), SBUF(am_key));

    uint8_t ao_param[8];
    uint8_t ao_key[7] = {'A', 'M', 'T', '_', 'O', 'U', 'T'};
    int8_t is_ao = otxn_param(SBUF(ao_param), SBUF(ao_key));

    uint8_t ac_param[20];
    uint8_t ac_key[5] = {'F', '_', 'A', 'C', 'C'};
    int8_t is_ac = otxn_param(SBUF(ac_param), SBUF(ac_key));

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
            rollback(SBUF("IIE :: Error: Only hook owner can change settings"), __LINE__);

        // Set AMT_IN state
        if (is_am > 0)
        {
            uint64_t am_value = UINT64_FROM_BUF(am_param);
            if (am_value <= 0)
                rollback(SBUF("IIE :: Error: AMT_IN must be positive"), __LINE__);
            if (state_set(SBUF(am_param), SBUF(am_key)) < 0)
                rollback(SBUF("IIE :: Error: Failed to set AMT_IN state"), __LINE__);
            TRACEVAR(am_value);
            accept(SBUF("IIE :: Accepted : AMT_IN state set successfully"), __LINE__);
        }

        // Set AMT_OUT state
        if (is_ao > 0)
        {
            uint64_t ao_value = UINT64_FROM_BUF(ao_param);
            if (ao_value <= 0)
                rollback(SBUF("IIE :: Error: AMT_OUT must be positive"), __LINE__);
            if (state_set(SBUF(ao_param), SBUF(ao_key)) < 0)
                rollback(SBUF("IIE :: Error: Failed to set AMT_OUT state"), __LINE__);
            TRACEVAR(ao_value);
            accept(SBUF("IIE :: Accepted : AMT_OUT state set successfully"), __LINE__);
        }

        // Set F_ACC state
        if (is_ac > 0)
        {
            if (BUFFER_EQUAL_20(ac_param, hook_acc))
                rollback(SBUF("IIE :: Error: F_ACC cannot match hook account"), __LINE__);
            if (state_set(SBUF(ac_param), SBUF(ac_key)) < 0)
                rollback(SBUF("IIE :: Error: Failed to set F_ACC state"), __LINE__);
            TRACEHEX(ac_param);
            accept(SBUF("IIE :: Accepted : F_ACC state set successfully"), __LINE__);
        }

        // Set CURRENCY state
        if (is_cur > 0)
        {
            if (state_set(SBUF(cur_param), SBUF(cur_key)) < 0)
                rollback(SBUF("IIE :: Error: Failed to set CURRENCY state"), __LINE__);
            accept(SBUF("IIE :: Accepted : CURRENCY state set successfully"), __LINE__);
        }

        // Set ISSUER state
        if (is_iss > 0)
        {
            if (state_set(SBUF(iss_param), SBUF(iss_key)) < 0)
                rollback(SBUF("IIE :: Error: Failed to set ISSUER state"), __LINE__);
            accept(SBUF("IIE :: Accepted : ISSUER state set successfully"), __LINE__);
        }

        rollback(SBUF("IIE :: Error: No valid parameters provided for invoke"), __LINE__);
    }

    // Handle ttPAYMENT (type 0)
    if (tt == ttPAYMENT)
    {
        // Load parameters from state
        uint8_t am_state[8];
        int64_t exact_amount;
        if (state(SBUF(am_state), SBUF(am_key)) < 0)
            rollback(SBUF("IIE :: Error: AMT_IN state not set"), __LINE__);
        exact_amount = UINT64_FROM_BUF(am_state);
        if (exact_amount <= 0)
            rollback(SBUF("IIE :: Error: AMT_IN must be positive"), __LINE__);

        uint8_t ao_state[8];
        int64_t amount_out;
        if (state(SBUF(ao_state), SBUF(ao_key)) < 0)
            rollback(SBUF("IIE :: Error: AMT_OUT state not set"), __LINE__);

        amount_out = UINT64_FROM_BUF(ao_state);
        if (amount_out <= 0)
            rollback(SBUF("IIE :: Error: AMT_OUT must be positive"), __LINE__);
        uint8_t ftxn_acc[20];
        if (state(SBUF(ftxn_acc), SBUF(ac_key)) < 0)
            rollback(SBUF("IIE :: Error: F_ACC state not set"), __LINE__);

        uint8_t currency[20];
        if (state(SBUF(currency), SBUF(cur_key)) < 0)
            rollback(SBUF("IIE :: Error: CURRENCY state not set"), __LINE__);
        uint8_t issuer[20];
        if (state(SBUF(issuer), SBUF(iss_key)) < 0)
            rollback(SBUF("IIE :: Error: ISSUER state not set"), __LINE__);

        if (BUFFER_EQUAL_20(ftxn_acc, hook_acc))
            rollback(SBUF("IIE :: Error: F_ACC cannot match hook account"), __LINE__);

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
            rollback(SBUF("IIE :: Error: Non-XAH payment rejected"), __LINE__);

        uint64_t exact_amount_drops = exact_amount * 1000000;

        // Check if payment matches exact_amount (drops)
        if (otxn_drops != exact_amount_drops)
            rollback(SBUF("IIE :: Error: Payment amount doesn't match AMT_IN"), __LINE__);

        // Reserve space for the emitted transaction
        etxn_reserve(1);

        // Prepare IOU amount in XFL format
        int64_t iou_amount_xfl = float_set(0, amount_out); // Convert uint64 to XFL
        uint8_t amt_out[48];
        if (float_sto(amt_out - 1, 49, SBUF(currency), SBUF(issuer), amount_xfl, sfAmount) < 0)
            rollback(SBUF("IIE :: Error: Failed to serialize IOU amount"), __LINE__);

        // Prepare the IOU payment transaction
        uint8_t txn[PREPARE_PAYMENT_SIMPLE_TRUSTLINE_SIZE];
        PREPARE_PAYMENT_SIMPLE_TRUSTLINE(txn, amt_out, ftxn_acc, 0, 0);

        uint8_t emithash[32];

        // Emit the transaction and check if it was successful
        if (emit(SBUF(emithash), SBUF(txn)) != 32)
        {
            rollback(SBUF("IIE :: Error: Failed to emit IOU transaction"), __LINE__);
        }

        accept(SBUF("IIE :: Accepted : Payment received and IOU forwarded successfully"), __LINE__);
    }

    rollback(SBUF("IIE :: Error: Transaction type not supported"), __LINE__);

    _g(1, 1); // Guard
    return 0;
}
//**************************************************************
// Xahau Hook 101 Example ~ Invoke Parameter Admin Hook
// Author: @handy_4ndy
//
// Description:
//   This hook allows only admin accounts to perform Invoke transactions on the hook account.
//   The hook owner can set the admin account via Invoke.
//   Other transactions pass through unchanged.
//
// Parameters:-
//   'A_ACC' (20 byte ACCOUNT_ID): Admin account ID to set in state.
//   Convert raddress to ACCOUNT_ID (https://hooks.services/tools/raddress-to-accountid)
//
// Usage:-
//   - Invoke from hook owner with 'A_ACC' to set the admin account.
//   - Admin account sends Invoke transaction; hook accepts.
//   - Non-admin or non-Invoke transactions pass through.
//
// Accepts:-
//   - Invoke transactions from the hook owner with 'A_ACC' parameter to set the account.
//   - Invoke transactions from the admin account.
//   - Non-Invoke transactions.
//
// Rejects:-
//   - Invoke transactions not from the hook owner or admin account.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Invoke Parameters :: called");

    // Get hook account
    uint8_t hook_acc[20];
    if (hook_account(SBUF(hook_acc)) != 20)
        rollback(SBUF("BIP :: Error :: Failed to get hook account."), __LINE__);

    // Get origin transaction account
    uint8_t otxn_acc[20];
    if (otxn_field(SBUF(otxn_acc), sfAccount) != 20)
        rollback(SBUF("BIP :: Error :: Failed to get origin account."), __LINE__);

    // Get transaction type
    int64_t tt = otxn_type();

    if (tt == 99) { // Invoke

        // Check if from hook owner and setting A_ACC
        if (BUFFER_EQUAL_20(hook_acc, otxn_acc)) {
            uint8_t a_acc_buf[20];
            uint8_t a_acc_key[5] = {'A', '_', 'A', 'C', 'C'};
            int64_t a_acc_len = otxn_param(SBUF(a_acc_buf), SBUF(a_acc_key));

            // Set the admin account in state
            if (a_acc_len == 20) {
                if (state_set(a_acc_buf, 20, a_acc_key, 5) < 0)
                    rollback(SBUF("BIP :: Error :: Could not set admin account in state."), __LINE__);

                // Successfully set admin account
                accept(SBUF("BIP :: Success :: Admin account set."), __LINE__);

            } else {
                // A_ACC parameter not set or invalid
                rollback(SBUF("BIP :: Error :: A_ACC parameter not set or invalid."), __LINE__);
            }
        } else {
            // Check if from admin account
            uint8_t admin_acc[20];
            uint8_t a_acc_key[5] = {'A', '_', 'A', 'C', 'C'};

            // Get the admin account from state
            if (state(SBUF(admin_acc), SBUF(a_acc_key)) != 20)
                rollback(SBUF("BIP :: Error :: Misconfigured. Admin account not set in state."), __LINE__);

            // Reject if not from admin account
            if (!BUFFER_EQUAL_20(otxn_acc, admin_acc))
                rollback(SBUF("BIP :: Error :: Invoke from non-admin account rejected."), __LINE__);

            accept(SBUF("BIP :: Success :: Invoke from admin account accepted."), __LINE__);
        }
    } else {
        accept(SBUF("BIP :: Success :: Non-INVOKE transaction passed through."), __LINE__);
    }

    _g(1,1); //Guard
    return 0;
}
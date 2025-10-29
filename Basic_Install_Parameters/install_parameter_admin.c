//**************************************************************
// Xahau Hook 101 Example ~ Install Parameter Admin Hook
// Author: @handy_4ndy
//
// Description:
//   This hook allows only the hook account and admin accounts to perform Invoke transactions on the hook account.
//   Other transactions pass through unchanged.
//
// Parameters:-
//   'A_ACC' (20 byte ACCOUNT_ID): Admin account ID that can trigger Invoke transactions.
//   Convert raddress to ACCOUNT_ID (https://hooks.services/tools/raddress-to-accountid)
//
// Usage:-
//   - Hook account or admin account sends Invoke transaction; hook accepts.
//   - Non-admin or non-Invoke transactions pass through.
//
// Accepts:-
//   - Invoke transactions from the hook account or admin account.
//   - Non-Invoke transactions.
//
// Rejects:-
//   - Invoke transactions from non-hook account and non-admin accounts.
//
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("BIP :: Basic Install Parameter :: called");

    // Get hook account
    uint8_t hook_acc[20];
    if (hook_account(SBUF(hook_acc)) != 20)
        rollback(SBUF("BIP :: Error :: Failed to get hook account."), __LINE__);

    // Get origin transaction account
    uint8_t otxn_acc[20];
    if (otxn_field(SBUF(otxn_acc), sfAccount) != 20)
        rollback(SBUF("BIP :: Error :: Failed to get origin account."), __LINE__);

    // Check if the transaction is of type INVOKE
    if (otxn_type() != 99)
        accept(SBUF("BIP :: Accepted :: Non-INVOKE transaction passed through."), __LINE__);

    // Get admin account from hook parameter
    uint8_t admin_acc[20];
    if (hook_param(SBUF(admin_acc), "A_ACC", 5) != 20)
        rollback(SBUF("BIP :: Error :: Misconfigured. Admin account not set as Hook Parameter."), __LINE__);

    // Check if the origin account is either the hook account or the admin account
    if (!BUFFER_EQUAL_20(otxn_acc, hook_acc) && !BUFFER_EQUAL_20(otxn_acc, admin_acc))
        rollback(SBUF("BIP :: Error :: Invoke from non-authorized account rejected."), __LINE__);

    TRACEVAR(admin_acc);

    // The Invoke transaction is from an authorized account
    accept(SBUF("BIP :: Accepted :: Invoke from authorized account accepted."), __LINE__);

    _g(1,1) //Guard
    return 0;
}
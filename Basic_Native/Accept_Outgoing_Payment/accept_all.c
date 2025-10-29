//**************************************************************
// Xahau Hook 101 Example ~ Accept All Payments Hook
// Author: @handy_4ndy
//
// Description:
//   This hook accepts all incoming payments and all outgoing payments.
//
// Accepts:-
//   - All incoming payments.
//   - All outgoing payments.
//
//**************************************************************


#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    TRACESTR("AIP :: Accept Incoming Payment :: Called.");

    // Get the Hook account
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    TRACEHEX(hook_acc);

    // Get the Originating account of the transaction
    uint8_t otxn_acc[20];
    otxn_field(SBUF(otxn_acc), sfAccount);

    TRACEHEX(otxn_acc);

    // Accept all outgoing payments
    if (BUFFER_EQUAL_20(hook_acc, otxn_acc))
        accept(SBUF("AIP :: Outgoing payment accepted."), __LINE__);

    // Accept all Incoming payments
    accept(SBUF("AIP :: Incoming payment accepted."), __LINE__);

    _g(1,1); // Guard
    return 0;
}
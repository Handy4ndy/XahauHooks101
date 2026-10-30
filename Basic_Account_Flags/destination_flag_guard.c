//**************************************************************
// Xahau Hook 101 Example ~ Destination Flag Guard
// Author: @Handy_4ndy
//
// Description:
//   Install on an account that sends, for example a hot wallet.
//   On an outgoing transaction that has a Destination, reads the AccountRoot flags of that Destination.
//   If the Destination has any of the 17 Xahau account flags set, the transaction is rolled back and the first flag found is named.
//   Each flag check is a separate block with its own #define, test and message. Copy one block to reuse that check.
//
// Triggers:
//   ttPAYMENT and ttINVOKE (outgoing transactions with a Destination are checked)
//   Any other transaction type can be added to HookOn
//   Incoming transactions pass unchecked
//
// Parameters:
//   None (the 17 flag values are hardcoded)
//
// Usage:
//   - Deploy the hook to the sending account.
//   - Set triggers for at least ttPAYMENT and ttINVOKE.
//   - Remove the flag blocks you do not want to block (PasswordSpent and URITokenIssuer are set on many normal accounts).
//
// Accepts:
//   - Incoming transactions.
//   - Outgoing transactions with no Destination field.
//   - Outgoing transactions to a Destination that does not exist yet.
//   - Outgoing transactions to a Destination with none of the 17 flags set.
//
// Rejects:
//   - Outgoing transactions to a Destination with any of the 17 flags set (first match is named, lowest bit first, rollback code 1 to 17).
//   - Outgoing transactions where the sender or Destination flags cannot be read.
//**************************************************************

#include "hookapi.h"

int64_t hook(uint32_t reserved) {

    // Incoming tx (not sent by the hook account) passes
    uint8_t hook_acc[20];
    hook_account(SBUF(hook_acc));

    uint8_t sender[20];
    if (otxn_field(SBUF(sender), sfAccount) != 20)
        rollback(SBUF("Could not read sender"), __LINE__);

    if (!BUFFER_EQUAL_20(hook_acc, sender))
        accept(SBUF("Incoming tx passed"), __LINE__);

    // Outgoing tx with no Destination field passes
    uint8_t dest[20];
    if (otxn_field(SBUF(dest), sfDestination) != 20)
        accept(SBUF("No destination"), __LINE__);

    // Load the Destination AccountRoot into slot 1, sfFlags into slot 2
    uint8_t kl[34];
    if (util_keylet(SBUF(kl), KEYLET_ACCOUNT, SBUF(dest), 0, 0, 0, 0) != 34)
        rollback(SBUF("Could not build destination keylet"), __LINE__);

    // No AccountRoot yet, so no flags to check
    if (slot_set(SBUF(kl), 1) != 1)
        accept(SBUF("Destination not activated"), __LINE__);

    if (slot_subfield(1, sfFlags, 2) != 2)
        rollback(SBUF("Could not read destination flags"), __LINE__);

    int64_t flags = slot(0, 0, 2);
    if (flags < 0)
        rollback(SBUF("Could not read destination flags"), __LINE__);

    // 1. AllowTrustLineClawback (Xahau value, not the XRPL one)
    #define lsfAllowTrustLineClawback 0x00001000LL
    if (flags & lsfAllowTrustLineClawback)
        rollback(SBUF("Destination has AllowTrustLineClawback set"), 1);

    // 2. PasswordSpent (free SetRegularKey used)
    #define lsfPasswordSpent 0x00010000LL
    if (flags & lsfPasswordSpent)
        rollback(SBUF("Destination has PasswordSpent set"), 2);

    // 3. RequireDestTag
    #define lsfRequireDestTag 0x00020000LL
    if (flags & lsfRequireDestTag)
        rollback(SBUF("Destination has RequireDestTag set"), 3);

    // 4. RequireAuth
    #define lsfRequireAuth 0x00040000LL
    if (flags & lsfRequireAuth)
        rollback(SBUF("Destination has RequireAuth set"), 4);

    // 5. DisallowXRP
    #define lsfDisallowXRP 0x00080000LL
    if (flags & lsfDisallowXRP)
        rollback(SBUF("Destination has DisallowXRP set"), 5);

    // 6. DisableMaster
    #define lsfDisableMaster 0x00100000LL
    if (flags & lsfDisableMaster)
        rollback(SBUF("Destination has DisableMaster set"), 6);

    // 7. NoFreeze
    #define lsfNoFreeze 0x00200000LL
    if (flags & lsfNoFreeze)
        rollback(SBUF("Destination has NoFreeze set"), 7);

    // 8. GlobalFreeze
    #define lsfGlobalFreeze 0x00400000LL
    if (flags & lsfGlobalFreeze)
        rollback(SBUF("Destination has GlobalFreeze set"), 8);

    // 9. DefaultRipple
    #define lsfDefaultRipple 0x00800000LL
    if (flags & lsfDefaultRipple)
        rollback(SBUF("Destination has DefaultRipple set"), 9);

    // 10. DepositAuth
    #define lsfDepositAuth 0x01000000LL
    if (flags & lsfDepositAuth)
        rollback(SBUF("Destination has DepositAuth set"), 10);

    // 11. TshCollect
    #define lsfTshCollect 0x02000000LL
    if (flags & lsfTshCollect)
        rollback(SBUF("Destination has TshCollect set"), 11);

    // 12. DisallowIncomingNFTokenOffer
    #define lsfDisallowIncomingNFTokenOffer 0x04000000LL
    if (flags & lsfDisallowIncomingNFTokenOffer)
        rollback(SBUF("Destination has DisallowIncomingNFTokenOffer set"), 12);

    // 13. DisallowIncomingCheck
    #define lsfDisallowIncomingCheck 0x08000000LL
    if (flags & lsfDisallowIncomingCheck)
        rollback(SBUF("Destination has DisallowIncomingCheck set"), 13);

    // 14. DisallowIncomingPayChan
    #define lsfDisallowIncomingPayChan 0x10000000LL
    if (flags & lsfDisallowIncomingPayChan)
        rollback(SBUF("Destination has DisallowIncomingPayChan set"), 14);

    // 15. DisallowIncomingTrustline
    #define lsfDisallowIncomingTrustline 0x20000000LL
    if (flags & lsfDisallowIncomingTrustline)
        rollback(SBUF("Destination has DisallowIncomingTrustline set"), 15);

    // 16. URITokenIssuer (destination has minted a URIToken)
    #define lsfURITokenIssuer 0x40000000LL
    if (flags & lsfURITokenIssuer)
        rollback(SBUF("Destination has URITokenIssuer set"), 16);

    // 17. DisallowIncomingRemit (Xahau only)
    #define lsfDisallowIncomingRemit 0x80000000LL
    if (flags & lsfDisallowIncomingRemit)
        rollback(SBUF("Destination has DisallowIncomingRemit set"), 17);

    accept(SBUF("Destination flags clear"), __LINE__);

    _g(1,1); // Guard
    return 0;
}

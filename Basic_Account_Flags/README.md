
# Xahau Hooks 101 - Basic Account Flags Hooks Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. The hooks here show how to read the AccountRoot of another account and test its `sfFlags` field, one flag at a time. There are two paths: the sender path checks the account that sent a transaction to you, and the destination path checks the account you are sending to. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for more details.

## Overview

Hooks in this collection demonstrate:
- Reading another account's ledger object with `util_keylet(KEYLET_ACCOUNT)`, `slot_set`, `slot_subfield` and `slot`
- Testing the 17 Xahau AccountRoot flags, each in its own copyable block
- Two paths with the same flag list: the **Sender path** (`sender_flag_guard.c`) and the **Destination path** (`destination_flag_guard.c`)
- Rolling back with a message that names the first flag found, or accepting when none are set
- No parameters and no state: the flag list is hardcoded

**Teaching example:** blocking every flag is strict. PasswordSpent is set on many normal accounts that used their free SetRegularKey, and URITokenIssuer is set by any URITokenMint and never cleared, so those accounts are rejected by both hooks. Remove the blocks you do not want before using either hook on a real account.

### Sender path

- File: `sender_flag_guard.c`
- Install on the **receiving** account
- On an incoming transaction, reads the AccountRoot flags of the sender (`sfAccount`)
- Rolls back with `Sender has <FlagName> set` for the first flag found
- Outgoing transactions from the hook account pass unchecked (`Outgoing tx passed`)

### Destination path

- File: `destination_flag_guard.c`
- Install on a **sending** account, for example a hot wallet
- On an outgoing transaction that has a Destination, reads the AccountRoot flags of that Destination
- Rolls back with `Destination has <FlagName> set` for the first flag found
- A Destination that does not exist yet passes (`Destination not activated`)
- Outgoing transactions with no Destination field pass (`No destination`)
- Incoming transactions pass unchecked (`Incoming tx passed`)

### When to use which

- Use the **Sender path** when you want to control who can send to your account, for example to refuse payments from accounts with DisableMaster or GlobalFreeze set.
- Use the **Destination path** when you want to stop your own account from sending to accounts with certain flags, for example to avoid paying an account that has DisallowXRP or RequireDestTag set.
- Both can be installed on the same account (in different hook positions) if you want both checks.

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions to or from the hook account)
  - `ttINVOKE` (Invoke transactions to or from the hook account)
  - Any other transaction type can be added. Testing used an all zero HookOn (every type except SetHook)
  - Sender path: only incoming transactions are checked. Destination path: only outgoing transactions with `sfDestination` are checked
  - The hooks run as strong hooks (hook account is sender or destination). Weak runs need TshCollect on the hook account and were not tested

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| sender_flag_guard.c         | Sender path. Install on the receiving account. Reads the sender AccountRoot flags; rolls back with `Sender has <FlagName> set` for the first of 17 flags found, accepts with `Sender flags clear` otherwise. Outgoing tx from the hook account pass. |
| destination_flag_guard.c    | Destination path. Install on a sending account. Reads the Destination AccountRoot flags of outgoing tx; rolls back with `Destination has <FlagName> set` for the first of 17 flags found, accepts with `Destination flags clear` otherwise. Unfunded destinations, tx with no Destination and incoming tx pass. |

For detailed explanations, see comments in each `.c` file.

### Flags and check order

Both hooks check the same 17 flags in the same fixed order (lowest bit first). Only the first match is reported.

| Order | Flag | Hex | How it gets set |
|---|---|---|---|
| 1 | AllowTrustLineClawback | 0x00001000 | AccountSet asf 17 (needs empty owner dir and no NoFreeze) |
| 2 | PasswordSpent | 0x00010000 | Protocol: free SetRegularKey or Import. Cleared by an incoming XAH payment |
| 3 | RequireDestTag | 0x00020000 | AccountSet asf 1 |
| 4 | RequireAuth | 0x00040000 | AccountSet asf 2 |
| 5 | DisallowXRP | 0x00080000 | AccountSet asf 3 |
| 6 | DisableMaster | 0x00100000 | AccountSet asf 4 (needs regular key or signer list) |
| 7 | NoFreeze | 0x00200000 | AccountSet asf 6 |
| 8 | GlobalFreeze | 0x00400000 | AccountSet asf 7 |
| 9 | DefaultRipple | 0x00800000 | AccountSet asf 8 |
| 10 | DepositAuth | 0x01000000 | AccountSet asf 9 |
| 11 | TshCollect | 0x02000000 | AccountSet asf 11 |
| 12 | DisallowIncomingNFTokenOffer | 0x04000000 | AccountSet asf 12 |
| 13 | DisallowIncomingCheck | 0x08000000 | AccountSet asf 13 |
| 14 | DisallowIncomingPayChan | 0x10000000 | AccountSet asf 14 |
| 15 | DisallowIncomingTrustline | 0x20000000 | AccountSet asf 15 |
| 16 | URITokenIssuer | 0x40000000 | Protocol: set on URITokenMint, never cleared |
| 17 | DisallowIncomingRemit | 0x80000000 | AccountSet asf 16 |

Two values differ from XRPL: AllowTrustLineClawback is `0x00001000` on Xahau, and `0x80000000` is DisallowIncomingRemit.

### Messages

Sender path (`sender_flag_guard.c`):

| Case | Outcome | Message |
|---|---|---|
| Tx sent by the hook account | accept | `Outgoing tx passed` |
| Sender has a listed flag | rollback (tecHOOK_REJECTED) | `Sender has <FlagName> set` |
| Sender has none of the flags | accept | `Sender flags clear` |
| sfAccount missing | rollback | `Could not read sender` |
| Keylet build failed | rollback | `Could not build sender keylet` |
| Sender AccountRoot not found | rollback | `Could not load sender account` |
| sfFlags not readable | rollback | `Could not read sender flags` |

Destination path (`destination_flag_guard.c`):

| Case | Outcome | Message |
|---|---|---|
| Tx not sent by the hook account (incoming) | accept | `Incoming tx passed` |
| Outgoing tx with no sfDestination | accept | `No destination` |
| Destination AccountRoot not found (unfunded) | accept | `Destination not activated` |
| Destination has a listed flag | rollback (tecHOOK_REJECTED) | `Destination has <FlagName> set` |
| Destination has none of the flags | accept | `Destination flags clear` |
| sfAccount missing | rollback | `Could not read sender` |
| Keylet build failed | rollback | `Could not build destination keylet` |
| sfFlags not readable | rollback | `Could not read destination flags` |

The read failure cases should not happen on a live ledger. They stop the hooks from running on bad data.

## Technical Patterns

- Foreign ledger read: build the keylet of the other account with `util_keylet(KEYLET_ACCOUNT)`, load it into slot 1 with `slot_set`, pick `sfFlags` into slot 2 with `slot_subfield`, read it as an int64 with `slot(0, 0, 2)`
- Direction check: compare `hook_account` with `sfAccount`. The sender path accepts early when they match (outgoing), the destination path accepts early when they do not match (incoming)
- Destination path: reads `sfDestination` with `otxn_field`; a missing field means the tx has no destination and passes. A failed `slot_set` means the Destination account does not exist yet and passes
- Copy and paste layout: each flag check is a separate block with its own `#define`, test and rollback message. There is no loop or lookup table, so one block can be copied into another hook (together with the flag read above it). The blocks are the same in both files, only the word Sender or Destination in the message changes
- Rollback codes 1 to 17 match the check order, so the return code also tells you which flag matched

## Tools & Resources

- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging

## Testing and Deployment

1. **Set Up Testnet Account(s) in Hooks Builder**
	- Create funded accounts in the "Deploy" section: https://builder.xahau.network/deploy
	- Or in the "Test" section: https://builder.xahau.network/test

2. **Prepare the Code**
	- Go to the "Developer" section: https://builder.xahau.network/develop
	- Copy `sender_flag_guard.c` or `destination_flag_guard.c` into the Xahau Hooks Builder starter template (delete the template content)

3. **Compile**
	- Click "Compile to WASM"

4. **Deploy and Set Triggers**
	- Go to the "Deploy" section and deploy the hook to an account by clicking "Set Hook"
	- Sender path: deploy on the account that receives. Destination path: deploy on the account that sends
	- **Important:** Configure hook triggers for at least `ttPAYMENT` and `ttINVOKE`. If you don't set this, the hook will NOT be triggered!
	- No install parameters are needed. Any 32 byte HookNamespace works, the hooks have no state

5. **Test Transactions**
	- Sender path: from a fresh account, send a Payment to the hook account: expect `Sender flags clear`. Set one flag on that account with AccountSet (for example asf 3, DisallowXRP) and pay again: expect tecHOOK_REJECTED with `Sender has DisallowXRP set`
	- Destination path: from the hook account, send a Payment to a fresh account: expect `Destination flags clear`. Set one flag on that account with AccountSet (for example asf 3, DisallowXRP) and pay it again: expect tecHOOK_REJECTED with `Destination has DisallowXRP set`

6. **Verify**
	- Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

### Testnet results

Both hooks were run on 30 Oct 2026 on Xahau Testnet (`wss://xahau-test.net`, NetworkID 21338). Nothing was sent to Mainnet. SetHook settings for both: HookOn all zero, Flags 1 (hsfOverride), HookApiVersion 0, no parameters.

Sender path (`sender_flag_guard.c`):

| Item | Value |
|---|---|
| Hook account | `rrsVjvQmGmUWbJz3wog68mUgYZuU9yom4p` |
| SetHook tx | `6AD65E69BB4C70072B9623DD73D461EE89CE47DD8C1E4A81971D9441A74787B1` (tesSUCCESS) |
| HookHash | `974B46DA106FC0B9B9D4C4CF1104FCCCB1931471E3E115567F5D12D99CEAFF88` |
| Wasm used | 1608 bytes, sha256 `bd88256643f2085921c1db18ea64d2299b3b542e6d32acf026673d44629fa321` |
| Result | 21 of 21 cases passed |

Cases: one fresh sender per flag with exactly that flag set (17 cases, all rejected with the matching message), a clean sender (accepted), an Invoke from the clean sender (accepted), an Invoke from a RequireDestTag sender (rejected), and an outgoing Payment from the hook account (accepted with `Outgoing tx passed`).

Destination path (`destination_flag_guard.c`):

| Item | Value |
|---|---|
| Hook account | `rK37gMAW2SDbT2nF6bsLscSyBe2b5xin6B` |
| SetHook tx | `0262054024837944998D48F4FFC5B33451DACA49554679A9F9B4EEC2D1A846F5` (tesSUCCESS) |
| HookHash | `05D18E025E3E8B85A5EA5466CC7FF64E6C5E96047557F8DA669CD980994F8A37` |
| Wasm used | 1753 bytes, sha256 `50c982dcaa64016524e408a52dbb43d97ffa244f21e5143b4148985d9c310fcb` |
| Result | 23 of 23 cases passed |

Cases: one fresh destination per flag with exactly that flag set (17 Payments from the hook account, all rejected with the matching message), a clean destination (accepted), an unfunded destination (accepted with `Destination not activated`), an incoming Payment to the hook account (accepted with `Incoming tx passed`), an outgoing AccountSet with no Destination (accepted with `No destination`), an Invoke to the clean destination (accepted), and an Invoke to a RequireDestTag destination (rejected). The test Payments carried DestinationTag 1 so the RequireDestTag destination reached the hook.

Not covered for either hook: accounts with more than one flag set (check order is from code review only), weak TSH runs, transaction types other than Payment, Invoke and AccountSet, and Mainnet.

Setup notes for the harder flags:
- DisableMaster: the first SetRegularKey signed by the master key is free and sets PasswordSpent. Clear it with an incoming XAH payment, then set asf 4. For the sender path, sign the test payment with the regular key. For the destination path, send the clearing payment from another account, because the hook would block a payment from the hook account to a PasswordSpent destination
- PasswordSpent: SetRegularKey with Fee 0, signed by the master key
- URITokenIssuer: one URITokenMint

### Caveats

- **PasswordSpent and URITokenIssuer are strict.** Many normal accounts have one of these set. With all 17 blocks in place, both hooks reject them. Remove those blocks if that is not what you want
- **RequireDestTag (destination path):** a Payment with no DestinationTag to a RequireDestTag account is refused by the ledger with tecDST_TAG_NEEDED before the hook runs. The hook only sees, and rejects, tagged payments to that account
- **DepositAuth (destination path):** for a Payment to a DepositAuth account with no preauth, the hook ran first and gave tecHOOK_REJECTED with `Destination has DepositAuth set`, not the ledger DepositAuth error
- **Local build:** the wasm files above were built locally with the Hooks Builder toolchain (clang 15, wasm-opt, hook-cleaner, guard_checker pass). A compile in Hooks Builder may give different bytes and so a different HookHash. The hook logic is the same. Only the `.c` files are kept in this folder

## Testing & Debugging

- Use TRACESTR and TRACEHEX for execution tracing
- Check transaction logs and explorer for results. The hook return string is in the tx metadata
- **Common pitfalls:**
  - Missing triggers (hook not firing)
  - Installing on the wrong account: the sender path goes on the receiver, the destination path goes on the sender
  - Testing with an account that already has PasswordSpent or URITokenIssuer set, so it is rejected before the flag you meant to test
  - Expecting a weak (TSH) run to fire without TshCollect on the hook account
  - Using XRPL flag values: AllowTrustLineClawback and DisallowIncomingRemit differ on Xahau

## Code Structure & Best Practices

- One entry point, an early accept for the direction that is not checked, one flag read, then 17 independent checks and a final accept
- Each rollback names the flag, so the result is clear in the explorer
- Read failures roll back instead of accepting, so the hooks never pass a tx they could not check. The one planned exception is the unfunded destination, which has no flags to check
- Decide which flags you really want to block before deploying. Remove the rest

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

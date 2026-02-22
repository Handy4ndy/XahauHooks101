
# Xahau Hooks 101 – Emit_IOU Hooks Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. These hooks demonstrate how to emit IOU token payments from the hook account using hardcoded values, install-time parameters, or runtime state set via Invoke transactions. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Overview

Hooks in this collection demonstrate:
- IOU token emission from the hook account
- Hardcoded, install-time, and invoke-set parameterization
- Multi-account IOU emission and admin-controlled emission
- State usage for dynamic configuration
- Parameter validation, safe state read/write, and emission logic

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (incoming payment triggers IOU emission)
  - `ttINVOKE` (invoke transactions for admin or state configuration)
  - Both triggers should be set for full functionality

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| hardcoded_iou_emit.c        | Hardcoded IOU emit: emits IOU tokens to a fixed account on incoming payments. |
| hardcoded_multi_iou_emit.c  | Hardcoded multi-IOU emit: emits to up to 2 fixed accounts on incoming payments. |
| install_iou_emit.c          | Install param IOU emit: emits amount and account set at install time. |
| install_multi_iou_emit.c    | Install param multi-IOU emit: emits to multiple accounts configured at install. |
| invoke_iou_emit.c           | Invoke-set IOU emit: emits amount/account set via Invoke transaction state. |
| invoke_multi_iou_emit.c     | Invoke-set multi-IOU emit: emits to multiple accounts via Invoke-set state. |
| admin_iou_emit.c            | Admin-triggered IOU emit: emits IOU tokens from hook account when invoked by admin. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Hardcoded, install-time, and invoke-set parameterization for IOU emission
- State usage for dynamic configuration (invoke-set hooks)
- Admin/owner controls for privileged emission (admin_iou_emit.c)
- Multi-account emission with etxn_reserve for multiple transactions
- Emission logic using `PREPARE_PAYMENT_SIMPLE_TRUSTLINE` and XFL-encoded amounts
- Parameter validation, safe state handling, and clear rollback messages

## Tools & Resources

- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging

## Testing and Deployment

1. **Set Up Testnet Account(s) in Hooks Builder**
   - Create funded accounts in the “Deploy” section: https://builder.xahau.network/deploy
   - Or in the “Test” section: https://builder.xahau.network/test

2. **Prepare the Code**
   - Go to the “Developer” section: https://builder.xahau.network/develop
   - Copy the desired `.c` hook code into the Xahau Hooks Builder starter template (delete the template content)

3. **Compile**
   - Click “Compile to WASM”

4. **Deploy and Set Triggers**
   - Go to the “Deploy” section and deploy the hook to an account by clicking “Set Hook”
   - **Important:** Configure hook triggers for both `ttPAYMENT` and `ttINVOKE`. If you don’t set this, the hook will NOT be triggered!
   - For install-param hooks: provide install parameters (amounts as 8-byte uint64, accounts as 20-byte ACCOUNT_ID, currency/issuer as 20-byte)
   - For invoke-set hooks: deploy without params, then use Invoke to set state
   - For hardcoded/admin hooks: deploy as-is

5. **Test Transactions**
   - Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior

6. **Verify**
   - Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

Example: install `install_iou_emit.c` with `AMT_IN`: `000000000000000A` (10 XAH), `AMT_OUT`: `00000000000003E8` (1000 IOU), `F_ACC`: account ID, `CURRENCY`: "XPN" padded, `ISSUER`: issuer account.

## Testing & Debugging

- Use TRACESTR, TRACEHEX, and TRACEVAR for runtime debugging
- Check transaction logs and explorer for results
- **Common pitfalls:**
  - Parameter length/type mismatches
  - Missing triggers (hook not firing)
  - Invalid or missing state access
  - Permissions/admin/owner errors
  - Insufficient IOU token balance for emits
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook has a clear entry point, parameter validation, and accept/rollback logic
- Uses safe state handling and clear rollback messages
- Restricts privileged emission to admin/owner where required
- Uses correct byte sizes: 8 bytes for uint64 amounts, 20 bytes for ACCOUNT_ID and currency/issuer
- Multi-emit hooks require etxn_reserve(N) for N emissions
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.
<parameter name="filePath">/home/handy/Documents/devwork/XahauHooks101/IOU_Emit/README.md
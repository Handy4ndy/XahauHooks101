
# Xahau Hooks 101 – Basic Install Parameters Hooks

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. These hooks demonstrate how to provide configuration at install time (install parameters) and enforce those settings for subsequent Payment or Invoke flows. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Overview

Hooks in this collection demonstrate:
- Payment validation and enforcement of install-time parameters (EXACT, MIN, MAX, MULTI, ADMIN)
- Static configuration: parameters are set at install time and remain until the hook is re-installed or updated
- Admin/owner controls for certain hooks (e.g., ADMIN)
- State is not modified at runtime except for admin hooks
- Typical behaviors: outgoing payments are accepted, IOU payments are accepted unless explicitly rejected, missing/invalid parameters cause rollback with clear error

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - `ttINVOKE` (Invoke transactions, for admin hooks)
  - Both triggers must be set for full functionality

## Hook List

| File                      | Purpose/Description |
|---------------------------|---------------------|
| install_parameter_exact.c | Install param `'EXACT'` (8 bytes) — accepts incoming XAH payments that exactly match this amount. |
| install_parameter_min.c   | Install param `'MIN'` (8 bytes) — accepts incoming XAH payments ≥ MIN. |
| install_parameter_max.c   | Install param `'MAX'` (8 bytes) — accepts incoming XAH payments ≤ MAX. |
| install_parameter_multi.c | Install param `'MULTI'` (8 bytes) — accepts incoming XAH payments that are multiples of MULTI. |
| install_parameter_admin.c | Install param `'A_ACC'` (20 bytes) — allows Invoke from the hook account and configured admin account. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Install-time parameterization: parameters are provided at deployment and read using `hook_param`
- Parameters are static until the hook is re-installed/updated
- Admin/owner controls for admin hooks (20-byte ACCOUNT_ID)
- Outgoing payments and IOU payments are generally accepted unless explicitly rejected
- Rollback with clear error messages if parameters are missing or invalid

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
  - **Important:** Configure hook triggers for both `ttPAYMENT` and `ttINVOKE` (if needed). If you don’t set this, the hook will NOT be triggered!
  - Add install parameters as required:
    - 8-byte uint64 values for `EXACT`, `MIN`, `MAX`, `MULTI`
    - 20-byte ACCOUNT_ID for `A_ACC`

5. **Test Transactions**
  - Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior

6. **Verify**
  - Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

Example: install with `MULTI`: `000000000000000A` (8-byte uint64) — incoming XAH payments must be multiples of 10 XAH

## Testing & Debugging

- Use TRACESTR and TRACEHEX for execution tracing
- Check transaction logs and explorer for results
- **Common pitfalls:**
  - Parameter length/type mismatches
  - Missing triggers (hook not firing)
  - Invalid or missing state access
  - Permissions/admin/owner errors
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook reads install parameter(s) with `hook_param` and validates size
- Retrieves hook account and originating account
- Handles outgoing/IOU early-accepts
- For `ttPAYMENT`, reads `sfAmount`, converts to drops, and enforces install parameter rules
- Calls `accept()` or `rollback()` with clear messages
- Use correct byte sizes: 8 bytes for uint64 amounts, 20 bytes for ACCOUNT_ID
- Remove or reduce verbose tracing in production

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

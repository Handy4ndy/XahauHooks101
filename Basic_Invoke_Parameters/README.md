
# Xahau Hooks 101 – Basic Invoke Parameters Hooks

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. These hooks demonstrate how to use Invoke transactions to set runtime parameters in Hook state, and then enforce those parameters for subsequent Payment transactions. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Overview

Hooks in this collection demonstrate:
- Setting and updating parameters at runtime via Invoke transactions (ttINVOKE)
- Payment validation based on state values (EXACT, MIN, MAX, MULTI, ADMIN)
- Owner/admin controls for parameter setting (admin hooks allow both owner and admin)
- State usage: parameters are stored in Hook state and persist until changed by another Invoke
- Outgoing payments and IOU payments are generally accepted unless explicitly rejected
- Parameter/state validation and rollback messages for misconfiguration

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - `ttINVOKE` (Invoke transactions for setting parameters)
  - Both triggers must be set for full functionality

## Hook List

| File                      | Purpose/Description |
|---------------------------|---------------------|
| invoke_parameter_exact.c  | Owner Invoke sets 'EXACT' (8 bytes). Payments accepted only if XAH amount exactly matches state value. |
| invoke_parameter_min.c    | Owner Invoke sets 'MIN' (8 bytes). Payments accepted only if XAH amount ≥ MIN. |
| invoke_parameter_max.c    | Owner Invoke sets 'MAX' (8 bytes). Payments accepted only if XAH amount ≤ MAX. |
| invoke_parameter_multi.c  | Owner Invoke sets 'MULTI' (8 bytes). Payments accepted only if XAH amount is a multiple of MULTI. |
| invoke_parameter_admin.c  | Owner Invoke sets admin account ('A_ACC', 20 bytes) in state. Both hook owner and admin may invoke admin operations. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Parameters are set at runtime via Invoke (ttINVOKE) using `otxn_param` and `state_set`
- Payment path reads state and enforces rules (ttPAYMENT)
- Owner/admin controls for parameter setting (admin hooks)
- State validation and rollback messages for misconfiguration
- Outgoing payments and IOU payments are generally accepted unless explicitly rejected

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

5. **Test Transactions**
   - Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior

6. **Verify**
   - Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

Example: To set MIN, send an Invoke from the hook owner with parameter key 'MIN': `0000000000000001` (8 bytes uint64) → saved to state. Later payments read 'MIN' from state and are compared.

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

- Each hook has a clear entry point, parameter validation, and accept/rollback logic
- Uses safe state handling and clear rollback messages
- Detects `ttINVOKE` for parameter setting, validates origin account, and calls `state_set`
- For `ttPAYMENT`, reads state param(s), inspects `sfAmount`, computes drops, and enforces logic
- Calls `accept()` or `rollback()` with clear messages
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

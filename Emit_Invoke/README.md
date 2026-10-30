# Xahau Hooks 101 – Emit_Invoke Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains canonical, beginner-friendly examples demonstrating how to emit Invoke transactions from a Hook on the Xahau blockchain. Each example is focused on a different parameterization pattern (hardcoded, install-time, runtime/invoke, admin) and multi-destination logic. All examples are written in C, ready for compilation to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop).

## Overview

Hooks in this collection demonstrate:
- Emitting Invoke transactions from a Hook in response to incoming Payment or Invoke transactions
- Parameterization patterns: hardcoded, install-time (hook_param), runtime/invoke (otxn_param, state_set), and admin-controlled
- Multi-destination emission and reset logic
- Canonical error/status reporting and safe buffer/state handling

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (incoming payments)
  - `ttINVOKE` (for admin or owner parameter setting)
  - Some examples accept only payments, others allow admin/owner to set parameters via Invoke

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| admin_invoke_emit.c         | Admin-controlled emission: only a designated admin can trigger emission by sending an Invoke with DEST |
| hardcoded_invoke_emit.c     | Minimal hardcoded emission: emits to a fixed destination on any incoming payment |
| hardcoded_multi_invoke_emit.c | Emits to three hardcoded destinations on any incoming payment |
| install_invoke_emit.c       | Emits to a destination set at install-time via hook parameter DEST |
| install_multi_invoke_emit.c | Emits to three destinations set at install-time via DST1, DST2, DST3 |
| invoke_invoke_emit.c        | Emits to a destination set at runtime by the hook owner via Invoke with DEST |
| invoke_multi_invoke_emit.c  | Emits to up to three destinations set/cleared at runtime by the hook owner via Invoke with DST1/2/3 or RSET |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Demonstrates all canonical parameterization patterns for emission:
  - Hardcoded (no parameters)
  - Install-time (hook_param)
  - Runtime/invoke (otxn_param, state_set)
  - Admin-controlled (install-time admin, runtime invoke)
- Multi-destination logic and reset (RSET) feature
- Safe state usage, buffer handling, and error reporting


## Tools & Resources


- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks.
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments.
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging.
- **[Xahau Hook Tx Builder](https://tx-builder.xahau.tools/)**: Paste a JSON Xahau transaction to convert it to a C hook binary transaction with macros for use in your hook code. Made by @tequdev.

## Testing and Deployment

1. **Set Up Testnet Account(s) in Hooks Builder**
   - Create funded accounts in the “Deploy” section: https://builder.xahau.network/deploy
   - Or in the “Test” section: https://builder.xahau.network/test
2. **Prepare the Code**
   - Go to the “Developer” section: https://builder.xahau.network/develop
   - Copy the desired `.c` hook code into the Xahau Hooks Builder starter template (delete the template content).
3. **Compile**
   - Click “Compile to WASM”.
4. **Deploy and Set Triggers**
   - Go to the “Deploy” section and deploy the hook to an account by clicking “Set Hook”.
   - **Important:** Configure hook triggers (e.g., “for Payments”). If you don’t set this, the hook will NOT be triggered!
   - Add any install parameters if needed.
5. **Test Transactions**
   - Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior.
6. **Verify**
   - Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer.

## Testing & Debugging

- Use TRACESTR and TRACEHEX for execution tracing.
- Check transaction logs and explorer for results.
- **Common pitfalls:**
  - Parameter length/type mismatches
  - Missing triggers (hook not firing)
  - Invalid or missing state access
  - Permissions/admin/owner errors
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook has a clear entry point, parameter validation, and accept/rollback logic.
- Use safe state handling and clear rollback messages.
- Remove debug traces before production deployment.

## Dependencies

- Requires `hookapi.h` and standard C libraries.

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

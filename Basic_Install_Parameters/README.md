# Basic Install Parameters Hooks Collection

## About Xahau Hooks 101 — Install Parameters

**Xahau Hooks 101** is a collection of concise, beginner-friendly Xahau Hook examples written in C. This subdirectory demonstrates how to provide configuration to hooks at install time (install parameters) and how hooks enforce those settings for subsequent Payment or Invoke flows. Each hook is compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template and is intended for Testnet experimentation before Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Hooks ~ Trigger on ttPAYMENT && ttINVOKE

| File | Description |
|------|-------------|
| install_parameter_exact.c | Install param `'EXACT'` (8 bytes) — accepts incoming XAH payments that exactly match this amount. |
| install_parameter_min.c | Install param `'MIN'` (8 bytes) — accepts incoming XAH payments ≥ MIN. |
| install_parameter_max.c | Install param `'MAX'` (8 bytes) — accepts incoming XAH payments ≤ MAX. |
| install_parameter_multi.c | Install param `'MULTI'` (8 bytes) — accepts incoming XAH payments that are multiples of MULTI. |
| install_parameter_admin.c | Install param `'A_ACC'` (20 bytes) — allows Invoke from the hook account and configured admin account. |


## Overview

These hooks illustrate a common pattern:
- Provide parameters at install time (Deploy) using the Deploy UI or API.
- Hooks read install parameters with hook_param and enforce rules on payments or invokes.
- Install parameters are static until the hook is re-installed or updated — use for fixed policy settings.

Typical behaviors:
- Outgoing payments (from the hook account) are accepted.
- IOU payments are accepted unless hook explicitly rejects IOUs.
- Missing or invalid install parameters cause a rollback with a clear error message.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation & Usage

1. Copy one of the .c hooks into the Hooks Builder starter template and compile to WASM.
2. Deploy the hook to a Testnet account and supply install parameters:
   - 8-byte uint64 values for `EXACT`, `MIN`, `MAX`, `MULTI`.
   - 20-byte ACCOUNT_ID for `A_ACC`.
3. After installation, payments and invokes behave according to the installed parameters.
4. To change parameters re-install/update the hook with new install parameters.

Use [Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/) or  [Hooks.Services](https://hooks.services/tools) for conversion.

Example: install with `MULTI`: `000000000000000A`(8-byte uint64) — incoming XAH payments must be multiples of 10 XAH.

## Testing

- Use Hooks Builder Test to send payments and invokes, and view TRACESTR/TRACEVAR output.
- Verify cases:
  - Outgoing payments (from hook account) — accepted.
  - Incoming IOU payments — accepted unless explicitly rejected.
  - Incoming XAH payments — accepted/rejected per installed rule.
- Re-install with different parameters to test alternate policies.

## Debugging Tips

- Verify Hook triggers have been set correctly (ttPAYMENT & ttINVOKE).
- Check hook_param return length — ensure install parameter exists and is the expected size (8 or 20 bytes).
- Use TRACESTR, TRACEHEX, TRACEVAR to log values during testing.
- Use AMOUNT_TO_DROPS and integer math; avoid floating-point equality for monetary checks.
- If a parameter is missing, hooks typically rollback with an explanatory message — verify parameters on deploy.

## Code Structure

Each hook typically:
1. Reads install parameter(s) with hook_param and validates size.
2. Retrieves hook account (hook_account) and originating account (otxn_field sfAccount).
3. Handles outgoing/IOU early-accepts.
4. For ttPAYMENT, reads sfAmount, converts to drops, and enforces install parameter rules.
5. Calls accept() or rollback() with clear messages.

## Important Notes

- Install parameters are set at deployment; changing them requires re-installing the hook.
- Use correct byte sizes: 8 bytes for uint64 amounts, 20 bytes for ACCOUNT_ID.
- Test thoroughly on Testnet before deploying to Mainnet.
- Remove or reduce verbose tracing in production to minimize output and gas usage.

## Getting Started

1. Choose a hook (EXACT, MIN, MAX, MULTI, ADMIN).
2. Compile and install with appropriate install parameters.
3. Test with Payments (and Invokes for ADMIN-style hooks) using Hooks Builder Test or XRPLWin tools.
4. Inspect logs and explorer entries to confirm behavior.

## Related Collections

- See `Basic_Invoke_Parameters` for invoke→state patterns where parameters are set via Invoke.
- See other collections in the repo (Basic_State, Basic_Native, Basic_Iou) for additional examples.

---

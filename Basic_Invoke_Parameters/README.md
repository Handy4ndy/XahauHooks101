# Basic Invoke Parameters Hooks Collection

## About Xahau Hooks 101 — Basic Invoke Parameters

**Xahau Hooks 101** is a collection of short, beginner-friendly Xahau Hook examples in C. These hooks demonstrate how to use Invoke transactions (tt=99) to set runtime parameters into Hook state, and then enforce those parameters for subsequent Payment transactions. Each hook is compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template and intended for Testnet experimentation before Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for the full project context.

This directory focuses on "invoke → set state" patterns where the hook owner (or a configured admin) uses Invoke to store values like MIN, MAX, MULTI, EXACT, or admin account IDs. Payment handling logic reads those state values to accept or reject payments.

## Hooks ~ Trigger on ttPAYMENT && ttINVOKE

| File | Description |
|------|-------------|
| invoke_parameter_exact.c | Owner Invoke sets 'EXACT' (8 bytes). Payments accepted only if XAH amount exactly matches state value. |
| invoke_parameter_min.c | Owner Invoke sets 'MIN' (8 bytes). Payments accepted only if XAH amount ≥ MIN. |
| invoke_parameter_max.c | Owner Invoke sets 'MAX' (8 bytes). Payments accepted only if XAH amount ≤ MAX. |
| invoke_parameter_multi.c | Owner Invoke sets 'MULTI' (8 bytes). Payments accepted only if XAH amount is a multiple of MULTI. |
| invoke_parameter_admin.c | Owner Invoke sets admin account ('A_ACC', 20 bytes) in state. Both hook owner and admin may invoke admin operations. |

## Overview

Patterns demonstrated:
- Owner sets parameters via Invoke (tt=99) using otxn_param and state_set.
- Payment path reads state and enforces rules (ttPAYMENT).
- Hooks accept outgoing payments (originating from hook account) and IOU payments unless rules specify otherwise.
- Parameter/state validation and rollback messages for misconfiguration.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation & Usage

1. Copy one of the .c hooks into the Hooks Builder starter template and compile to WASM.
2. Deploy the compiled hook to a Testnet account.
3. Set parameters by sending an Invoke (tt=99) from the hook owner with the appropriate parameter:
   - EXACT, MIN, MAX, MULTI (8-byte uint64) or A_ACC (20-byte ACCOUNT_ID).
4. Send Payment transactions to test enforcement logic. If state parameter missing, payment path typically rollbacks with an explanatory message.

Use [Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/) and [Hooks.Services](https://hooks.services/tools) for conversion.

Example Invoke to set MIN:
- Invoke from hook owner with parameter key 'MIN' : '0000000000000001' (8 bytes uint64) → saved to state.
- Later payments read 'MIN' from state and are compared.

## Testing

- Use Hooks Builder Test to send Invokes and Payments and view TRACESTR/TRACEVAR output.
- Confirm state entries via tooling or transaction logs.
- Test both valid and invalid payments (IOU, XAH below/above/exact/multiples).

## Debugging Tips

- Check Hook triggers have been set correctly (ttPAYMENT & ttINVOKE).
- Verify otxn_param/otxn_field returned lengths — parameter lengths must match expected sizes (8 for uint64, 20 for account id).
- Use TRACESTR, TRACEHEX, TRACEVAR to emit helpful runtime diagnostics.
- Use AMOUNT_TO_DROPS and integer math; avoid float equality for monetary checks.
- Ensure Invoke is called from the hook owner (unless hook explicitly allows admin invokes).

## Code Structure

Typical hook flow:
1. Header with description and parameters.
2. hook() entry point with initial TRACESTR/guards.
3. Detect tt==99 (Invoke): validate origin account and call state_set for provided parameter.
4. Early accept for non-payment transactions after handling Invoke.
5. For ttPAYMENT: read state param(s), inspect otxn_field(sfAmount), compute drops, and enforce logic.
6. accept() or rollback() with clear messages.

## Important Notes

- Parameters set via Invoke persist in Hook state and affect subsequent transactions.
- Always validate parameter lengths and state read/write return values.
- Test thoroughly on Testnet before deploying to Mainnet.
- Remove or reduce verbose tracing in production.

## Getting Started

1. Open one of the invoke_parameter_*.c files.
2. Compile in Hooks Builder and deploy to a Testnet account.
3. Invoke as the hook owner to set parameters.
4. Send Payments and watch enforcement behavior via logs and explorer.

## Related Collections

- See other collections in the repo for IOU and Native examples (Basic_Iou, Basic_Native, etc.).
- Parent collection: [`Xahau-Hooks-101`](../README.md).

---

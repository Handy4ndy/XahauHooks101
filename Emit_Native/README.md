# Emit_Native Hooks Collection

## About Xahau Hooks 101 — Native Emit

**Xahau Hooks 101** is a collection of concise, beginner-friendly Xahau Hook examples written in C. This subdirectory demonstrates how to emit native XAH payments from the hook account using various configuration methods: hardcoded values, install-time parameters, or runtime state set via Invoke transactions. Each hook is compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template and is intended for Testnet experimentation before Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Hooks ~ Trigger on ttPAYMENT && ttINVOKE

| File | Description |
|------|-------------|
| hardcoded_native_emit.c | Hardcoded emit: emits 1 XAH to a fixed account on incoming payments. |
| hardcoded_multi_native_emit.c | Hardcoded multi-emit: emits to up to 2 fixed accounts on incoming payments. |
| install_native_emit.c | Install param emit: emits amount and account set at install time. |
| install_multi_native_emit.c | Install param multi-emit: emits to multiple accounts configured at install. |
| invoke_native_emit.c | Invoke-set emit: emits amount/account set via Invoke transaction state. |
| invoke_multi_native_emit.c | Invoke-set multi-emit: emits to multiple accounts via Invoke-set state. |
| admin_native_emit.c | Admin-triggered emit: emits from hook account when invoked by admin. |

## Overview

These hooks illustrate native payment emission patterns:
- Hardcoded emits for fixed, predictable behavior.
- Install-time parameters for static configuration.
- Invoke-set state for dynamic runtime configuration.
- Multi-account emits for distributing payments.
- Admin-controlled emits for privileged operations.

Typical behaviors:
- Accepts incoming XAH payments that match configured amounts (for payment-triggered emits).
- Accepts outgoing payments and IOU payments.
- Emits native XAH from the hook account using PREPARE_PAYMENT_SIMPLE and emit().
- Validates parameters and rollbacks on errors with clear messages.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation & Usage

1. Copy one of the .c hooks into the Hooks Builder starter template and compile to WASM.
2. Deploy the hook to a Testnet account:
   - For install-param hooks: provide install parameters (amounts as 8-byte uint64, accounts as 20-byte ACCOUNT_ID).
   - For invoke-set hooks: deploy without params, then use Invoke to set state.
   - For hardcoded/admin hooks: deploy as-is.
3. For payment-triggered hooks: send XAH payments matching configured amounts to trigger emits.
4. For invoke-triggered hooks: send Invoke transactions to set parameters or trigger emits.
5. Monitor emitted transactions in the explorer.

Use [Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/) or [Hooks.Services](https://hooks.services/tools) for conversion.

Example: install install_native_emit.c with `AMT`: `00000000000F4240` (1 XAH in drops) and `ACC`: account ID.

## Testing

- Use Hooks Builder Test to send payments and invokes, and view TRACESTR/TRACEVAR output.
- Verify emitted transactions appear in the explorer.
- Test edge cases: insufficient balance, invalid accounts, mismatched amounts.
- For multi-emit hooks, confirm all configured accounts receive payments.

## Debugging Tips

- Verify Hook triggers have been set correctly (ttPAYMENT & ttINVOKE).
- Check hook_param/state return lengths — ensure parameters exist and match expected sizes.
- Use TRACESTR, TRACEHEX, TRACEVAR to log emit preparation and execution.
- Ensure hook account has sufficient XAH balance for emits.
- Use etxn_reserve() for multi-emit hooks to reserve transaction slots.
- If emits fail, check PREPARE_PAYMENT_SIMPLE parameters and account validity.

## Code Structure

Each hook typically:
1. Reads configuration (hardcoded, install params, or state).
2. Handles early accepts for outgoing/IOU payments.
3. For ttPAYMENT: validates incoming amount and triggers emit.
4. For ttINVOKE: sets state parameters or triggers admin emits.
5. Prepares payment with PREPARE_PAYMENT_SIMPLE, reserves with etxn_reserve().
6. Calls emit() and accept() with success messages, or rollback() on errors.

## Important Notes

- Emits consume XAH from the hook account — ensure sufficient balance.
- Use correct byte sizes: 8 bytes for uint64 amounts, 20 bytes for ACCOUNT_ID.
- Multi-emit hooks require etxn_reserve(N) for N emissions.
- Test thoroughly on Testnet before deploying to Mainnet.
- Remove or reduce verbose tracing in production to minimize output and gas usage.

## Getting Started

1. Choose a hook (hardcoded, install, invoke, multi, admin).
2. Compile and install with appropriate parameters.
3. Test with Payments (or Invokes for admin/state-set hooks) using Hooks Builder Test or XRPLWin tools.
4. Inspect logs and explorer entries to confirm emitted payments.

## Related Collections

- See `Basic_Install_Parameters` for install-time configuration patterns.
- See `Basic_Invoke_Parameters` for invoke→state patterns.
- See other collections in the repo (Basic_State, Basic_Iou, Basic_Native) for additional examples.

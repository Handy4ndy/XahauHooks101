# Basic State Hooks Collection

## About Xahau Hooks 101 — Basic State

**Xahau Hooks 101** is a collection of concise, beginner-friendly Xahau Hook examples written in C. Each Hook is a small, focused snippet compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template. These examples are intended for learning, testing on Xahau Testnet, and reference when deploying to Mainnet.

For more details, see the parent [`Xahau-Hooks-101`](../README.md).

This directory contains examples demonstrating common on‑chain state patterns: adding, updating, deleting state entries, counters, execution limits, and owner-only management using Invoke transactions and Payment triggers.

## Hooks ~ Triggers ttPAYMENT & ttInvoke

| File | Description |
|------|-------------|
| basic_state_add.c | Owner-only Invoke: add arbitrary state key/value pairs ('KEY', 'VAL'). |
| basic_state_delete.c | Owner-only Invoke: add or delete notes; tracks count under 'CNT'. |
| basic_state_manager.c | Owner-only Invoke: add, update (UPD), or delete state entries; maintains entry counter. |
| basic_state_counter.c | Payment counter: increments 'CNT' on payments; owner can set 'CNT' via Invoke. |
| basic_state_count_increase.c | Increments counter on Payment; rejects when counter >= 100. |
| basic_state_count_decrease.c | Decrements counter on Payment; starts at 100, rejects when <= 0. |
| basic_state_execution_limit.c | Execution limit example: starts at 5, decrements on Payment, rejects when 0. |

## Overview

These hooks show practical patterns for persisting and managing data in Hook state:
- Owner-only state management via Invoke (tt=99).
- Automatic state updates on Payment transactions (ttPAYMENT).
- Simple counters, limits, and CRUD-style state operations.
- Parameter validation and safe state read/write patterns.

## Common Parameters & State Keys

- 'VAL' (bytes) — value to store (basic_state_add, basic_state_delete, manager).
- 'KEY' (bytes) — custom key to store under (basic_state_add).
- 'DEL' (8 bytes) — numeric key to delete (basic_state_delete / manager).
- 'UPD' (8 bytes) — key to update with new 'VAL' (basic_state_manager).
- 'CNT' (8 bytes) — counter parameter for manual set (basic_state_counter).
- State key {'C','N','T'} — used by multiple hooks to store the counter.

Lengths matter: check otxn_param/state return lengths (8 for uint64, 20 for account ids, etc.).

## Hook Triggers

- Invoke (tt=99): used for owner/admin commands to add/update/delete state.
- Payment (ttPAYMENT): used for automatic counters, limits, and payment-based rules.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation & Usage

1. Copy a .c file into the Hooks Builder starter template and compile to WASM.
2. Deploy to a Testnet account and configure as needed.
3. For Invoke operations, send Invoke transactions from the hook owner with required parameters.
4. For payment-triggered hooks, send Payment transactions to the hooked account.

Use [Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/) and [Hooks.Services](https://hooks.services/tools) for conversion.

Example Invoke to Delete a value:
- Invoke from hook owner with parameter key 'DEL' : '0000000000000001' (8 bytes uint64).
- This Key : Value pair will be removed from the hook namespace.


## Testing

- Use Hooks Builder Test to run transactions and view TRACESTR/TRACEHEX output.
- Confirm state changes via explorer or tooling that shows hook state.
- Validate parameter lengths and that you invoke from the hook owner for owner-only operations.

## Debugging Tips

- Use TRACESTR, TRACEHEX, TRACEVAR for runtime debugging.
- Validate return values from otxn_param, otxn_field, state, and state_set.
- Ensure consistent state key bytes when reading/writing (e.g. {'C','N','T'}).
- Remember XAH amounts are in drops. Use AMOUNT_TO_DROPS and integer comparisons to avoid float errors.

## Security & Best Practices

- Restrict state-modifying Invokes to the hook owner (compare hook_account vs sfAccount).
- Validate parameter lengths and handle errors gracefully with rollback messages.
- Use small, deterministic state keys.
- Test thoroughly on Testnet before Mainnet deployment.

## Getting Started

1. Browse the files in this directory and open a hook source file.
2. Edit parameters and logic as needed.
3. Compile in Hooks Builder, deploy to a test account, and exercise both Invoke and Payment paths.
4. Inspect logs and state to verify behavior.

## Related Collections

- See the parent [`Xahau-Hooks-101`](../README.md) for other categories (Basic_Native, Basic_Iou, Basic_Invoke_Parameters, etc.).

---

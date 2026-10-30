
# Xahau Hooks 101 – Basic State Hooks Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. These hooks demonstrate common on-chain state patterns: adding, updating, deleting state entries, counters, execution limits, and owner-only management using Invoke transactions and Payment triggers. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for more details.

## Overview

Hooks in this collection demonstrate:
- Owner-only state management via Invoke (ttINVOKE)
- Automatic state updates on Payment transactions (ttPAYMENT)
- Simple counters, limits, and CRUD-style state operations
- Parameter validation and safe state read/write patterns
- State usage: parameters and counters are stored in Hook state and persist until changed by another Invoke or Payment

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - `ttINVOKE` (Invoke transactions for owner/admin commands)
  - Both triggers must be set for full functionality

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| basic_state_add.c           | Owner-only Invoke: add arbitrary state key/value pairs ('KEY', 'VAL'). |
| basic_state_delete.c        | Owner-only Invoke: add or delete notes; tracks count under 'CNT'. |
| basic_state_manager.c       | Owner-only Invoke: add, update (UPD), or delete state entries; maintains entry counter. |
| basic_state_counter.c       | Payment counter: increments 'CNT' on payments; owner can set 'CNT' via Invoke. |
| basic_state_count_increase.c| Increments counter on Payment; rejects when counter >= 100. |
| basic_state_count_decrease.c| Decrements counter on Payment; starts at 100, rejects when <= 0. |
| basic_state_execution_limit.c| Execution limit example: starts at 5, decrements on Payment, rejects when 0. |
| basic_state_toggle.c        | Owner-only Invoke: toggle hook on/off ('TGL'); payments accepted with trace if enabled. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- State usage: storing, updating, and deleting key/value pairs in Hook state
- Owner/admin controls for state-modifying Invokes (compare hook_account vs sfAccount)
- Parameter validation and safe state read/write patterns
- Counters, toggles, and execution limits implemented via state
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
	- **Important:** Configure hook triggers for both `ttPAYMENT` and `ttINVOKE`. If you don’t set this, the hook will NOT be triggered!

5. **Test Transactions**
	- Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior

6. **Verify**
	- Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

Example Invoke to Delete a value:
  - Invoke from hook owner with parameter key 'DEL': `0000000000000001` (8 bytes uint64)
  - This Key : Value pair will be removed from the hook namespace

## Testing & Debugging

- Use TRACESTR, TRACEHEX, and TRACEVAR for runtime debugging
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
- Restricts state-modifying Invokes to the hook owner (compare hook_account vs sfAccount)
- Uses small, deterministic state keys
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

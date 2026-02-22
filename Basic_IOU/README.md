# Xahau Hooks 101 – Basic IOU Hooks Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. Each Hook is designed for clarity, learning, and practical experimentation. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for more details.

## Overview

Hooks in this collection demonstrate:
- Basic IOU (non-native token) payment validation and filtering
- Accepting or rejecting incoming/outgoing IOU payments based on amount, exact match, minimum, maximum, or multiples
- XFL (extended float) format conversion for IOU amounts
- No parameterization; all rules are hardcoded

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - Incoming and outgoing payments (direction varies by hook)

## Hook List

| File/Directory                  | Purpose/Description |
|---------------------------------|---------------------|
| Accept_Incoming_Payment/        | Accept incoming IOU payments based on criteria (amount, exact match, multiples, only IOU) |
| Accept_Outgoing_Payment/        | Accept outgoing IOU payments based on criteria (amount, exact match, multiples, only IOU) |
| Reject_Incoming_Payment/        | Reject incoming IOU payments based on criteria (amount, exact match, multiples, only IOU) |
| Reject_Outgoing_Payment/        | Reject outgoing IOU payments based on criteria (amount, exact match, multiples, only IOU) |
| IOU_Logging.c                   | Standalone hook for logging/tracing IOU payment details; accepts incoming IOU payments above minimum |

For detailed explanations, see comments in each `.c` file and README.md in each subdirectory.

## Technical Patterns

- Hardcoded validation logic (no parameters)
- XFL format conversion for IOU amounts
- Accept/rollback logic for transaction filtering
- No state usage, admin, or owner controls
- Only payment transactions are processed

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
   - **Important:** Configure hook triggers for `ttPAYMENT`. If you don’t set this, the hook will NOT be triggered!

5. **Test Transactions**
   - Use the “Test” section in Hooks Builder (or XRPLWin tools) to perform transactions and test hook behavior

6. **Verify**
   - Check results in Hooks Builder logs (TRACESTR/TRACEHEX Debug Stream) and/or in Xahau Explorer

## Testing & Debugging

- Use TRACESTR and TRACEHEX for execution tracing
- Check transaction logs and explorer for results
- **Common pitfalls:**
  - Invalid `sfAmount` access (use `uint8_t amount[48]` and check `otxn_field` returns)
  - Missing triggers (hook not firing)
  - IOU amounts in XFL format; use `float_int` for integer conversion
  - Non-payment transactions (ensure hooks are set for `ttPAYMENT`)
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook has a clear entry point, amount validation, and accept/rollback logic
- No state or parameter usage
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## License

See the root project LICENSE file for details.
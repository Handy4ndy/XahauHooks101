# Xahau Hooks 101 – Reject Outgoing IOU Payment Hooks

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. Each Hook is designed for clarity, learning, and practical experimentation. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

## Overview

Hooks in this collection demonstrate:
- Outgoing IOU payment rejection logic (non-native tokens)
- Filtering by exact amount, minimum, maximum, or multiples
- XFL (extended float) format conversion for IOU amounts
- Rejecting incoming payments and, in some cases, outgoing XAH payments
- No parameterization; all rules are hardcoded

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - Outgoing payments only (rejects incoming payments)
  - Some hooks also reject outgoing XAH payments

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| reject_outgoing_iou_exact.c | Rejects outgoing IOU payments that exactly match 10 IOU; accepts others; rejects outgoing XAH |
| reject_outgoing_iou_max.c   | Rejects outgoing IOU payments above 100 IOU; accepts at or below; rejects outgoing XAH |
| reject_outgoing_iou_min.c   | Rejects outgoing IOU payments below 1 IOU; accepts at or above; rejects outgoing XAH |
| reject_outgoing_iou_multi.c | Rejects outgoing IOU payments not multiples of 10 IOU; accepts multiples; rejects outgoing XAH |
| reject_outgoing_iou.c       | Rejects all outgoing IOU payments; accepts outgoing XAH |
| reject_outgoing_xah.c       | Rejects outgoing XAH payments; accepts outgoing IOU |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Hardcoded validation logic (no parameters)
- XFL format conversion for IOU amounts
- Accept/rollback logic for transaction filtering
- No state usage, admin, or owner controls
- Only payment transactions are processed

## Tools & Resources

- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks.
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments.
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging.

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

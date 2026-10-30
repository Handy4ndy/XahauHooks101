# Xahau Hooks 101 – Accept Incoming IOU Payment Hooks

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. Each Hook is designed for clarity, learning, and practical experimentation. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

## Overview

Hooks in this collection demonstrate:
- Accepting incoming IOU (non-native token) payments based on amount, multiples, or thresholds
- Outgoing payments and incoming XAH payments are generally accepted unless explicitly rejected
- IOU amounts are handled using XFL (extended float) format conversions
- Hardcoded validation logic for IOU payments

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (Payment transactions)
  - Only payment triggers are required for these hooks

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| accept_incoming_iou_exact.c  | Accepts incoming IOU payments that exactly match 10 IOU, outgoing payments, and incoming XAH payments. |
| accept_incoming_iou_max.c    | Accepts incoming IOU payments below 100 IOU, outgoing payments, and incoming XAH payments. |
| accept_incoming_iou_min.c    | Accepts incoming IOU payments above 1 IOU, outgoing payments, and incoming XAH payments. |
| accept_incoming_iou_multi.c  | Accepts incoming IOU payments that are multiples of 10 IOU, outgoing payments, and incoming XAH payments. |
| accept_incoming_iou_only.c   | Accepts only incoming IOU payments, rejects outgoing payments and incoming XAH payments. |
| accept_incoming_iou.c        | Accepts incoming IOU payments and outgoing payments, rejects incoming XAH payments. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- IOU amount validation using XFL conversion and integer comparison
- Hardcoded logic for payment validation (no install/invoke parameters)
- Accept or reject based on payment amount, type, or direction
- Outgoing payments are generally accepted unless explicitly rejected
- Use of Hook API for transaction inspection

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
  - Parameter length/type mismatches (should not occur, as logic is hardcoded)
  - Missing triggers (hook not firing)
  - Invalid or missing state access (should not occur)
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook has a clear entry point, account validation, and payment amount/type validation
- Accept or rollback based on conditions
- Use safe state handling (if any) and clear rollback messages
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome!

## License

See the root project LICENSE file for details.

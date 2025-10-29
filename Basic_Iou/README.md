# IOU Basic Hooks Collection

## About Xahau Hooks 101

**Xahau Hooks 101** is a growing collection of short, beginner-friendly Xahau Hook examples written in C, designed to explore the Xahau Network’s smart contract-like features. Each Hook is a concise, digestible snippet compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop) starter template, suitable for testing on the Xahau Testnet or deploying to Mainnet. This project creates a reference resource for developers to help others learn.

For more details, see the parent [`Xahau-Hooks-101`](../README.md).

This directory contains example hooks for basic IOU (non-native token) payment operations on the Xahau network. Each subdirectory contains a collection of hooks demonstrating specific behaviors for incoming and outgoing payments, including acceptance, rejection, and logging scenarios. These hooks are designed for learning, testing, and reference purposes.

## Contents

| Directory                        | Description |
|----------------------------------|-------------|
| **Accept_Incoming_Payment/**     | Collection of hooks that accept incoming IOU payments based on various criteria (e.g., amount thresholds, exact matches, multiples). |
| **Accept_Outgoing_Payment/**     | Collection of hooks that accept outgoing IOU payments based on various criteria (e.g., amount thresholds, exact matches, multiples). |
| **Reject_Incoming_Payment/**     | Collection of hooks that reject incoming IOU payments based on various criteria (e.g., amount thresholds, exact matches, multiples). |
| **Reject_Outgoing_Payment/**     | Collection of hooks that reject outgoing IOU payments based on various criteria (e.g., amount thresholds, exact matches, multiples). |
| **IOU_Logging.c**                | Standalone hook demonstrating logging and tracing of IOU payment details, including account info and payment amounts. Accepts incoming IOU payments above a minimum amount. |

## Overview

These hooks validate IOU payments while accepting or rejecting based on criteria like exact amounts, thresholds, or multiples. IOU amounts are handled using XFL (extended float) format conversions. Each subdirectory focuses on specific payment directions (incoming/outgoing) and actions (accept/reject), with examples for learning and customization.

## Hook Examples ~ Trigger on ttPAYMENT

### Accept Incoming Payment Hooks
- **accept_incoming_iou_exact.c**: Accepts incoming IOU payments exactly matching 10 IOU.
- **accept_incoming_iou_max.c**: Accepts incoming IOU payments below 100 IOU.
- **accept_incoming_iou_min.c**: Accepts incoming IOU payments above 1 IOU.
- **accept_incoming_iou_multi.c**: Accepts incoming IOU payments that are multiples of 10 IOU.
- **accept_incoming_iou_only.c**: Accepts only incoming IOU payments.
- **accept_incoming_iou.c**: Accepts all incoming IOU payments.

### Accept Outgoing Payment Hooks
- **accept_outgoing_iou_exact.c**: Accepts outgoing IOU payments exactly matching 10 IOU.
- **accept_outgoing_iou_max.c**: Accepts outgoing IOU payments below 100 IOU.
- **accept_outgoing_iou_min.c**: Accepts outgoing IOU payments above 1 IOU.
- **accept_outgoing_iou_multi.c**: Accepts outgoing IOU payments that are multiples of 10 IOU.
- **accept_outgoing_iou_only.c**: Accepts only outgoing IOU payments.
- **accept_outgoing_iou.c**: Accepts all outgoing IOU payments.

### Reject Incoming Payment Hooks
- **reject_incoming_iou_exact.c**: Rejects incoming IOU payments exactly matching 10 IOU.
- **reject_incoming_iou_max.c**: Rejects incoming IOU payments above 100 IOU.
- **reject_incoming_iou_min.c**: Rejects incoming IOU payments below 1 IOU.
- **reject_incoming_iou_multi.c**: Rejects incoming IOU payments not multiples of 10 IOU.
- **reject_incoming_iou.c**: Rejects all incoming IOU payments.
- **reject_incoming_xah.c**: Rejects incoming XAH payments, accepts IOU.

### Reject Outgoing Payment Hooks
- **reject_outgoing_iou_exact.c**: Rejects outgoing IOU payments exactly matching 10 IOU.
- **reject_outgoing_iou_max.c**: Rejects outgoing IOU payments above 100 IOU.
- **reject_outgoing_iou_min.c**: Rejects outgoing IOU payments below 1 IOU.
- **reject_outgoing_iou_multi.c**: Rejects outgoing IOU payments not multiples of 10 IOU.
- **reject_outgoing_iou.c**: Rejects all outgoing IOU payments.
- **reject_outgoing_xah.c**: Rejects outgoing XAH payments, accepts IOU.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation and Usage

1. **Develop, Deploy, and Test on Testnet (All in Hooks Builder)**:
   - Copy the desired `.c` hook code into the [Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop) basic starter template.
   - Compile to WASM.
   - Move to the [Deploy](https://hooks-builder.xrpl.org/deploy) section: Set the hook on a Testnet account, configure hook triggers (e.g., for payments), and add any install parameters if needed.
   - Move to [Testing](https://hooks-builder.xrpl.org/test): Perform transactions to test the hook behavior.

2. **Verify Results and Confirm Hook Behavior**:
   - Use [XRPLWin Xahau Testnet](https://xahau-testnet.xrplwin.com) to check detailed testing results and confirm the hook's behavior.

3. **Deploy to Mainnet**:
   - When satisfied with Testnet testing, use [XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/account/YOUR_WALLET_RADDRESS_HERE/manage/hooks) to deploy the hook to a Mainnet account (replace `YOUR_WALLET_RADDRESS_HERE` with your account address).

## Testing

1. **Setup Accounts**:
   - Fund a Testnet account using the Testnet Faucet.
2. **Deploy Hooks**:
   - Compile in Xahau Hooks Builder and deploy via Hooks Builder or XRPLWin Hook Management.
3. **Test Transactions**:
   - Send IOU Payments (specify currency and issuer) using XRPLWin Tools or Hooks Builder Test.
4. **Verify**:
   - Check results in Xahau Explorer or Hooks Builder logs (`TRACESTR`/`TRACEHEX`).

## Debugging Tips

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution (e.g., "AIP :: Accept Incoming Payment :: Called.").
- **Xahau Explorer**: Verify `TransactionType`, `Amount`, and `HookHash`.
- **Common Issues**:
  - `Execution failure (no exit type specified)`: Caused by invalid `sfAmount` access. Use `uint8_t amount[48]` and check `otxn_field` returns.
  - Non-Payment transactions: Ensure hooks are set for `ttPayment`.
  - IOU Amounts: IOU amounts are in XFL format; use `float_int` for integer conversion.

## Code Structure

Each hook follows a similar structure:
- Header with description and requirements.
- Hook function entry point.
- Account validation (hook vs originating account).
- Amount retrieval and type check (IOU vs XAH).
- IOU amount conversion using XFL.
- Validation logic for IOU amounts.
- Accept or rollback based on conditions.

## Important Notes

- IOU amounts are in XFL format; convert to integer drops for comparisons.
- Hooks execute before transaction processing.
- Rollback prevents transaction execution.
- Accept allows transaction to proceed.
- Use TRACE statements for debugging (remove in production).

## Usage

- Each subdirectory contains multiple hook files (`.c`) with specific logic variations.
- Review the README.md in each subdirectory for detailed descriptions of the hooks.
- Uncomment or modify sections in the source files to customize logic for your use case.
- Use these examples as templates for more advanced hook development.

## Getting Started

1. Browse the subdirectories to find hooks matching your requirements.
2. Read the subdirectory README.md for hook details and usage.
3. Edit the hooks as needed for your requirements.
4. Compile and deploy using the Xahau hook toolchain.

## Related Collections

- See the parent [`Xahau-Hooks-101`](../README.md) for more hook examples and documentation.
- Explore other directories for XAH, emit, and advanced hook logic.

---
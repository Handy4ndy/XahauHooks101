# Xahau Hooks 101 - Reject Incoming Payment Hooks

This directory contains a collection of Xahau hooks focused on rejecting incoming payments based on various criteria. These hooks are part of the Xahau Hooks 101 tutorial series, demonstrating basic payment rejection logic in smart contracts on the Xahau blockchain.

## Overview

Xahau hooks allow developers to create programmable logic that executes automatically on the blockchain. These hooks specifically handle payment transactions, enforcing rules on whether to reject incoming payments.

Hooks in this collection:
- Accept outgoing payments from the hook account.
- Reject incoming payments based on specific conditions.
- Use the Hook API for transaction inspection and control.

## Hook Descriptions

### 1. reject_incoming_exact.c
**Purpose**: Rejects incoming XAH payments that exactly match the specified amount (10 XAH).

**Key Features**:
- Rejects all outgoing payments.
- Rejects incoming XAH payments of exactly 10 XAH.
- Rejects incoming IOU payments.
- Accepts incoming XAH payments that do not match the amount.

**Use Case**: Preventing specific fixed-amount incoming payments.

### 2. reject_incoming_iou.c
**Purpose**: Rejects incoming IOU payments, accepts incoming XAH payments.

**Key Features**:
- Rejects all outgoing payments.
- Rejects any incoming IOU payments.
- Accepts incoming XAH payments.

**Use Case**: XAH-only incoming payments.

### 3. reject_incoming_max.c
**Purpose**: Rejects incoming XAH payments above a maximum threshold (100 XAH).

**Key Features**:
- Rejects all outgoing payments.
- Rejects incoming XAH payments exceeding 100 XAH.
- Rejects incoming IOU payments.
- Accepts incoming XAH payments at or below the threshold.

**Use Case**: Limiting maximum incoming payment sizes.

### 4. reject_incoming_min.c
**Purpose**: Rejects incoming XAH payments below a minimum threshold (1 XAH).

**Key Features**:
- Rejects all outgoing payments.
- Rejects incoming XAH payments below 1 XAH.
- Rejects incoming IOU payments.
- Accepts incoming XAH payments at or above the threshold.

**Use Case**: Filtering out micro-incoming payments or spam.

### 5. reject_incoming_multi.c
**Purpose**: Rejects incoming XAH payments that are not multiples of a fixed amount (10 XAH).

**Key Features**:
- Rejects all outgoing payments.
- Rejects incoming XAH payments that are not multiples of 10 XAH.
- Rejects incoming IOU payments.
- Accepts incoming XAH payments that are multiples of 10 XAH.

**Use Case**: Enforcing specific incoming payment increments.

### 6. reject_incoming_xah.c
**Purpose**: Rejects incoming XAH payments, accepts IOU payments.

**Key Features**:
- Rejects all outgoing payments.
- Rejects any incoming XAH payments.
- Accepts incoming IOU payments.

**Use Case**: IOU-only incoming payments.

## Tools
Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
  - **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy hooks to Testnet accounts.
  - **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions for testing.
- **[XRPLWin Xahau Testnet Tools](https://xahau-testnet.xrplwin.com/tools)**: Create and test transactions on the Testnet; ideal for reviewing detailed results or preparing for Mainnet deployment.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/account/YOUR_WALLET_RADDRESS_HERE/manage/hooks)**: Deploy and manage hooks on Testnet or Mainnet (replace `YOUR_WALLET_RADDRESS_HERE` with your account, e.g., `rTest123...`).
- **[Xahau Testnet Faucet](https://xahau-test.net/faucet)**: Fund Testnet accounts.
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details on Testnet.

## Installation and Usage

1. **Develop, Deploy, and Test on Testnet (All in Hooks Builder)**:
   - Copy the desired `.c` hook code into the [Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop) basic starter template.
   - Compile to WASM.
   - Move to the [Deploy](https://hooks-builder.xrpl.org/deploy) section: Set the hook on a Testnet account, configure hook triggers (e.g., for payments), and add any install parameters if needed.
   - Move to [Testing](https://hooks-builder.xrpl.org/test): Perform transactions to test the hook behavior.

2. **Verify Results and Confirm Hook Behavior**:
   - Use [XRPLWin Xahau Testnet Tools](https://xahau-testnet.xrplwin.com/tools) to check detailed testing results and confirm the hook's behavior.

3. **Deploy to Mainnet**:
   - When satisfied with Testnet testing, use [XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/account/YOUR_WALLET_RADDRESS_HERE/manage/hooks) to deploy the hook to a Mainnet account (replace `YOUR_WALLET_RADDRESS_HERE` with your account address).

## Debugging Tips
- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution (e.g., `TRACESTR("RIP :: Reject Incoming Payment :: Called.");`).
- **Xahau Explorer**: Verify `TransactionType`, `Amount`, and `HookHash`.
- **Common Issues**:
  - `Execution failure (no exit type specified)`: Caused by invalid `sfAmount` access. Use `uint8_t amount[48]` and check `otxn_field` returns.
  - Non-Payment transactions: Ensure hooks are set for `ttPayment`.

## Code Structure

Each hook follows a similar structure:
- Header with description and requirements.
- Hook function entry point.
- Account validation (hook vs originating account).
- Payment amount validation.
- Accept or rollback based on conditions.

## Important Notes

- All amounts are in XAH (1 XAH = 1,000,000 drops).
- Hooks execute before transaction processing.
- Rollback prevents transaction execution.
- Accept allows transaction to proceed.
- Use TRACE statements for debugging (remove in production).

## Dependencies

- Xahau Hook API (`hookapi.h`).
- Standard C libraries for buffer operations.

## Contributing

This is part of the Xahau Hooks 101 educational series. Contributions and improvements are welcome!
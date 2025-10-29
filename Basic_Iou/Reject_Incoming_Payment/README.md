# Xahau Hooks 101 - Reject Incoming IOU Payment Hooks

This directory contains a collection of Xahau hooks focused on rejecting incoming IOU (non-native token) payments based on various criteria. These hooks are part of the Xahau Hooks 101 tutorial series, demonstrating basic IOU payment rejection logic in smart contracts on the Xahau blockchain.

## Overview

These hooks validate incoming IOU payments while rejecting outgoing payments and, in some cases, incoming XAH payments. IOU amounts are handled using XFL (extended float) format conversions to perform comparisons. Each hook applies specific rules to incoming IOU transactions, such as exact amounts, thresholds, or multiples, while rejecting all outgoing transactions.

## Hook Examples

### 1. reject_incoming_iou_exact.c
**Purpose**: Rejects incoming IOU payments that exactly match 10 IOU, accepts other IOU payments, rejects incoming XAH payments.

**Key Features**:
- Validates IOU amount using XFL conversion.
- Accepts incoming IOU payments that do not match the exact amount.
- Rejects incoming XAH payments.
- Rejects all outgoing payments.

**Use Case**: Preventing specific fixed-amount incoming IOU payments.

### 2. reject_incoming_iou_max.c
**Purpose**: Rejects incoming IOU payments above 100 IOU, accepts IOU payments below the threshold, rejects incoming XAH payments.

**Key Features**:
- Checks IOU amount against a maximum threshold.
- Accepts incoming IOU payments at or below the threshold.
- Rejects incoming XAH payments.
- Rejects all outgoing payments.

**Use Case**: Limiting maximum incoming IOU payment sizes.

### 3. reject_incoming_iou_min.c
**Purpose**: Rejects incoming IOU payments below 1 IOU, accepts IOU payments above the threshold, rejects incoming XAH payments.

**Key Features**:
- Enforces a minimum IOU amount.
- Accepts incoming IOU payments at or above the threshold.
- Rejects incoming XAH payments.
- Rejects all outgoing payments.

**Use Case**: Filtering out micro-incoming IOU payments or spam.

### 4. reject_incoming_iou_multi.c
**Purpose**: Rejects incoming IOU payments that are not multiples of 10 IOU, accepts IOU payments that are multiples, rejects incoming XAH payments.

**Key Features**:
- Validates IOU amounts as multiples of a fixed value.
- Accepts incoming IOU payments that are multiples of 10 IOU.
- Rejects incoming XAH payments.
- Rejects all outgoing payments.

**Use Case**: Enforcing specific incoming IOU payment increments.

### 5. reject_incoming_iou.c
**Purpose**: Rejects incoming IOU payments, accepts incoming XAH payments.

**Key Features**:
- Rejects any incoming IOU payments.
- Accepts incoming XAH payments.
- Rejects all outgoing payments.

**Use Case**: XAH-only incoming payments.

### 6. reject_incoming_xah.c
**Purpose**: Rejects incoming XAH payments, accepts incoming IOU payments.

**Key Features**:
- Rejects any incoming XAH payments.
- Accepts incoming IOU payments.
- Rejects all outgoing payments.

**Use Case**: IOU-only incoming payments.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Xahau Testnet Tools](https://xahau-testnet.xrplwin.com/tools)**: For detailed testing results and confirming hook behavior.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/account/YOUR_WALLET_RADDRESS_HERE/manage/hooks)**: Deploy hooks to Mainnet (replace `YOUR_WALLET_RADDRESS_HERE` with your account address).
- **[Xahau Testnet Faucet](https://xahau-test.net/faucet)**: Fund Testnet accounts.
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

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

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution (e.g., "RII :: Reject Incoming IOU :: Called.").
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

## Dependencies

- Xahau Hook API (`hookapi.h`).
- Standard C libraries for buffer operations.

## License

MIT
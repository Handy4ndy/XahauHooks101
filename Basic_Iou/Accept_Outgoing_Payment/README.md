# Xahau Hooks 101 - Accept Outgoing IOU Payment Hooks

This directory contains a collection of Xahau hooks focused on accepting or rejecting outgoing IOU (non-native token) payments based on various criteria. These hooks are part of the Xahau Hooks 101 tutorial series, demonstrating basic IOU payment validation logic in smart contracts on the Xahau blockchain.

## Overview

These hooks validate outgoing IOU payments while accepting incoming payments and, in some cases, outgoing XAH payments. IOU amounts are handled using XFL (extended float) format conversions to perform comparisons. Each hook applies specific rules to outgoing IOU transactions, such as exact amounts, thresholds, or multiples, while allowing unrestricted incoming payments.

## Hook Examples

### 1. accept_outgoing_iou_exact.c
**Purpose**: Accepts outgoing IOU payments that exactly match 10 IOU, incoming payments, and outgoing XAH payments.

**Key Features**:
- Validates IOU amount using XFL conversion.
- Accepts all XAH payments without validation.
- Rejects IOU payments not exactly 10 IOU.

**Use Case**: Enforce exact IOU payment amounts for specific outgoing transactions.

### 2. accept_outgoing_iou_max.c
**Purpose**: Accepts outgoing IOU payments below 100 IOU, incoming payments, and outgoing XAH payments.

**Key Features**:
- Checks IOU amount against a maximum threshold.
- Accepts all XAH and incoming payments.
- Rejects IOU payments above 100 IOU.

**Use Case**: Limit outgoing IOU payments to prevent large transfers.

### 3. accept_outgoing_iou_min.c
**Purpose**: Accepts outgoing IOU payments above 1 IOU, incoming payments, and outgoing XAH payments.

**Key Features**:
- Enforces a minimum IOU amount.
- Accepts all XAH and incoming payments.
- Rejects IOU payments below 1 IOU.

**Use Case**: Ensure minimum IOU payment values for outgoing transactions.

### 4. accept_outgoing_iou_multi.c
**Purpose**: Accepts outgoing IOU payments that are multiples of 10 IOU, incoming payments, and outgoing XAH payments.

**Key Features**:
- Validates IOU amounts as multiples of a fixed value.
- Accepts all XAH and incoming payments.
- Rejects IOU payments that are not multiples or below the base amount.

**Use Case**: Require IOU payments in specific increments for outgoing transactions.

### 5. accept_outgoing_iou_only.c
**Purpose**: Accepts only outgoing IOU payments, rejects incoming payments and outgoing XAH payments.

**Key Features**:
- Strict IOU-only outgoing acceptance.
- Rejects all incoming and XAH outgoing.

**Use Case**: Send-only accounts for IOU tokens.

### 6. accept_outgoing_iou.c
**Purpose**: Accepts outgoing IOU payments and incoming payments, rejects outgoing XAH payments.

**Key Features**:
- Allows all IOU outgoing payments.
- Accepts incoming transactions.
- Rejects XAH outgoing.

**Use Case**: IOU-only outgoing payments with unrestricted incoming.

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
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

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution (e.g., `TRACESTR("AOI :: Accept Outgoing IOU :: Called.");`).
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


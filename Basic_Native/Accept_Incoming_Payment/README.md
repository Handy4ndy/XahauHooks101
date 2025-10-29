# Xahau Hooks 101 - Accept Incoming Payment Hooks

This directory contains a collection of Xahau hooks focused on accepting or rejecting incoming payments based on various criteria. These hooks are part of the Xahau Hooks 101 tutorial series, demonstrating basic payment validation logic in smart contracts on the Xahau blockchain.

## Overview

Xahau hooks allow developers to create programmable logic that executes automatically on the blockchain. These hooks specifically handle payment transactions, enforcing rules on whether to accept or reject incoming payments.

Hooks in this collection:
- Accept outgoing payments from the hook account.
- Validate incoming payments based on specific conditions.
- Use the Hook API for transaction inspection and control.

## Hook Descriptions

### 1. accept_all.c
**Purpose**: Accepts all payments, both incoming and outgoing.

**Key Features**:
- No validation on payment amounts or types.
- Accepts every transaction.

**Use Case**: Basic acceptance hook for testing or unrestricted accounts.


### 2. accept_incoming_exact.c
**Purpose**: Accepts incoming XAH payments that exactly match a specified amount (10 XAH).

**Key Features**:
- Requires exact payment of 10 XAH.
- Rejects any other amount.
- Accepts all outgoing payments.

**Use Case**: Fixed-price purchases or exact fee collections.


### 3. accept_incoming_max.c
**Purpose**: Accepts incoming XAH payments below a maximum threshold (100 XAH).

**Key Features**:
- Enforces maximum payment limit of 100 XAH.
- Rejects payments above the threshold.
- Accepts all outgoing payments.

**Use Case**: Limiting transaction sizes for security or regulatory compliance.


### 4. accept_incoming_min.c
**Purpose**: Accepts incoming XAH payments above a minimum threshold (1 XAH).

**Key Features**:
- Enforces minimum payment of 1 XAH.
- Rejects payments below the threshold.
- Accepts all outgoing payments.

**Use Case**: Minimum donation amounts or anti-spam payment filters.


### 5. accept_Incoming_multi.c
**Purpose**: Accepts incoming XAH payments that are multiples of a fixed amount (10 XAH).

**Key Features**:
- Validates XAH payments only.
- Checks if payment amount is a multiple of 10 XAH.
- Accepts exact 10 XAH payments immediately.
- Rejects payments less than 10 XAH or not exact multiples.

**Use Case**: Vending machines or services requiring specific payment increments.


### 6. accept_incoming_only.c
**Purpose**: Accepts only incoming payments, rejects all outgoing payments.

**Key Features**:
- Rejects any transaction originating from the hook account (outgoing).
- Accepts all incoming payments regardless of type or amount.
**Use Case**: Receive-only accounts that cannot send funds.


### 7. accept_incoming_xah.c
**Purpose**: Accepts incoming XAH payments and outgoing payments, rejects incoming IOU payments.

**Key Features**:
- Checks if the payment is in native XAH currency (amount length == 8).
- Accepts all outgoing transactions from the hook account.
- Rejects any incoming IOU (non-XAH) payments.

**Use Case**: Basic XAH-only payment acceptance.


## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configue hook on Testnet accounts.
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

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution (e.g., `TRACESTR("AIP :: Accept Incoming Payment :: Called.");`).
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

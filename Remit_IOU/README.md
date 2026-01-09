# Remit_IOU Hooks Collection

## About Xahau Hooks 101 — IOU Remit

**Xahau Hooks 101** is a collection of concise, beginner-friendly Xahau Hook examples written in C. This subdirectory demonstrates how to emit IOU token **remit** transactions from the hook account using various configuration methods: hardcoded values, install-time parameters, or runtime state set via Invoke transactions. Each hook is compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template and is intended for Testnet experimentation before Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## What is Remit? (vs Payment)

The **Remit** transaction type is a powerful alternative to Payment transactions that offers significant advantages for IOU issuance:

### Key Benefits of Remit:
- ✅ **Automatic trustline creation** - No pre-setup required by recipient
- ✅ **Account creation** - Can send to accounts that don't exist yet
- ✅ **Sender pays reserves** - Reserves paid by sender, not recipient
- ✅ **Single transaction** - Issuance + trustline setup in one step
- ✅ **Multi-currency support** - Can send multiple currencies in a single transaction

### Remit vs Payment Comparison:

| Feature | Payment (Emit_IOU) | Remit (Remit_IOU) |
|---------|-------------------|-------------------|
| Trustline Required | ✅ Yes (pre-existing) | ❌ No (auto-created) |
| Account Must Exist | ✅ Yes | ❌ No (auto-created) |
| Reserve Paid By | Recipient | Sender |
| Transaction Structure | PREPARE_PAYMENT_SIMPLE_TRUSTLINE | Custom Amounts Array |
| Complexity | Simple (macro-based) | Advanced (manual construction) |
| Use Case | Existing relationships | Token issuance, airdrops |

## Technical Implementation

Remit transactions use a specialized **Amounts array** structure that must be manually constructed following the exact binary format from RichardAH's production AMM hook:

```c
Binary Structure:
  Base Transaction: 229 bytes (standard Remit fields)
  + Amounts Array:
    0xF0 0x5C              ← sfAmounts array start
      0xE0 0x5B            ← sfAmountEntry object start
        0x61 + 48 bytes    ← sfAmount (currency/issuer/value)
      0xE1                 ← End sfAmountEntry object
    0xF1                   ← End sfAmounts array
  = ~284 bytes total for single currency issuance
```

This proven pattern ensures compatibility with `etxn_fee_base()` and successful transaction emission.

## Hooks ~ Trigger on ttPAYMENT && ttINVOKE

| File | Description |
|------|-------------|
| admin_iou_remit.c | Admin-triggered remit: admin invokes to trigger IOU remit from hook account. |
| hardcoded_iou_remit.c | Hardcoded IOU remit: remits IOU tokens to a fixed account on incoming payments. |
| hardcoded_multi_iou_remit.c | Hardcoded multi-IOU remit: remits to up to 2 fixed accounts on incoming payments. |
| install_iou_remit.c | Install param IOU remit: remits amount and account set at install time. |
| install_multi_iou_remit.c | Install param multi-IOU remit: remits to multiple accounts configured at install. |
| invoke_iou_remit.c | Invoke-set IOU remit: remits amount/account set via Invoke transaction state. |
| invoke_multi_iou_remit.c | Invoke-set multi-IOU remit: remits to multiple accounts via Invoke-set state. |

## Overview

These hooks illustrate IOU token remit patterns:
- **Hardcoded remits** for fixed, predictable IOU behavior with automatic trustline creation.
- **Install-time parameters** for static IOU configuration decided at deployment.
- **Invoke-set state** for dynamic runtime IOU configuration that can be updated.
- **Multi-account IOU remits** for distributing tokens to multiple recipients.
- **Admin-controlled IOU remits** for privileged token operations on-demand.

Typical behaviors:
- Accepts incoming XAH payments that match configured amounts (for payment-triggered remits).
- Accepts outgoing payments and IOU payments.
- **Emits IOU tokens from the hook account using Remit transaction type**.
- Uses manual Amounts array construction following RichardAH's AMM hook pattern.
- Validates parameters and rollbacks on errors with clear messages.

## Comparison with Emit_IOU

| Aspect | Emit_IOU (Payment) | Remit_IOU (Remit) |
|--------|-------------------|-------------------|
| **Best For** | Existing trustlines | Token issuance, airdrops |
| **Setup Required** | Recipient must have trustline | None |
| **Code Complexity** | Simple (uses macros) | Advanced (manual construction) |
| **Transaction Type** | ttPAYMENT (0x00) | ttREMIT (0x5F) |
| **Transaction Size** | ~283 bytes | ~284 bytes |
| **Implementation** | PREPARE_PAYMENT_SIMPLE_TRUSTLINE | Manual Amounts array |
| **Production Proven** | Standard pattern | RichardAH's AMM hook |
| **Multi-Currency** | Multiple transactions | Single transaction possible |

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

## Installation & Usage

1. Copy one of the .c hooks into the Hooks Builder starter template and compile to WASM.
2. Deploy the hook to a Testnet account:
   - For **install-param hooks**: provide install parameters during deployment:
     - Amounts as 8-byte uint64 (big-endian)
     - Accounts as 20-byte ACCOUNT_ID
     - Currency/Issuer as 20-byte values
   - For **invoke hooks**: send Invoke transaction after deployment to set state parameters.
   - For **admin hooks**: set ADMIN, CURRENCY, ISSUER at install; admin invokes with AMT and DEST.
3. Test by sending appropriate trigger transactions (XAH Payments or Invoke).
4. Verify remit transactions in Xahau Explorer.

## Parameter Encoding

Use [Hooks Services Tools](https://hooks.services/tools) for encoding:

**Example Install Parameters:**
- `AMT_IN`: `000000000000000A` (10 XAH as uint64)
- `AMT_OUT`: `0000000000000005` (5 IOU tokens as uint64)
- `F_ACC`: `58BF50228721CF7E6B3AE45ACEFEEEB71974CA571` (20-byte Account ID)
- `CURRENCY`: `0000000000000000000000005850(4E0000000000000` (XPN in standard format)
- `ISSUER`: `B959D6AC9D158917EC269EAC0570190(74F56D639` (Issuer Account ID)

**Example Invoke Parameters (same encoding):**
- Set state by sending Invoke from hook owner with desired key-value pairs
- State persists across executions for dynamic configuration

## When to Use Remit vs Payment

**Use Remit (this collection) when:**
- Issuing new tokens to recipients without existing trustlines
- Sending to accounts that might not exist yet
- Performing airdrops or mass token distribution
- You want sender to pay for trustline reserves
- You need the most flexible issuance mechanism

**Use Payment (Emit_IOU) when:**
- Trustlines already exist between parties
- Working with established token relationships
- You want simpler, more standard transaction structure
- Recipients are expected to manage their own trustlines

## Advanced: Multi-Currency Remits

To remit multiple currencies in a single transaction (like AMM withdrawals), the Amounts array structure supports multiple AmountEntry objects:

```c
// After first AmountEntry (before final 0xF1):
*amounts_ptr++ = 0xE0U;  // Start second AmountEntry
*amounts_ptr++ = 0x5BU;
amount_len = float_sto(amounts_ptr, 49, currency2, 20, issuer2, 20, amount2_xfl, sfAmount);
amounts_ptr += amount_len;
*amounts_ptr++ = 0xE1U;  // End second AmountEntry
*amounts_ptr++ = 0xF1U;  // End array (NOW write it)
```

This matches RichardAH's AMM hook pattern for dual-currency remits (~335 bytes total).

## Related Examples

- **[Emit_IOU](../Emit_IOU/)**: Payment-based IOU emission for existing trustlines
- **[Emit_Native](../Emit_Native/)**: Native XAH emission patterns
- **[Basic_IOU](../Basic_IOU/)**: IOU payment acceptance and validation

## References

- **[RichardAH's AMM Hook](https://github.com/RichardAH/ammhook)**: Production-proven Remit pattern
- **[Xahau Transaction Types](https://docs.xahau.network/)**: Official Remit documentation
- **[Hook API Reference](https://xrpl-hooks.readme.io/)**: Complete hook function documentation

## Contributing

This is a community learning project — contributions, suggestions, and feedback are always welcome! If you develop new remit patterns or discover optimizations, please share them with the community.

## License

See the parent repository [LICENSE](../LICENSE) file for details.

---

**Note**: This is an advanced collection building on the patterns from Emit_IOU. If you're new to Xahau Hooks, start with the simpler examples in Basic_Native, Basic_IOU, and Emit_Native before working with Remit transactions.

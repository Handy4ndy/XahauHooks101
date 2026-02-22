# Remit_URI Hooks Collection

## About Xahau Hooks 101 — URI Remit

**Xahau Hooks 101** is a collection of concise, beginner-friendly Xahau Hook examples written in C. This subdirectory demonstrates how to emit **URIToken remit** transactions from the hook account using various configuration methods: hardcoded values, install-time parameters, or runtime state set via Invoke transactions. Each hook is compiled to WebAssembly (WASM) using the Xahau Hooks Builder starter template and is intended for Testnet experimentation before Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## What are URI Remits? (NFT-like Tokens)

The **Remit** transaction type with URITokens creates NFT-like tokens with metadata URIs (typically IPFS links) that are automatically sent to recipients. This enables unique use cases like NFT minting, certificate distribution, and tokenized access systems.

### Key Benefits of URI Remits:
- ✅ **Automatic account creation** - No pre-setup required by recipient
- ✅ **NFT-like functionality** - Metadata URIs for rich token experiences
- ✅ **Sequential numbering** - Perfect for numbered collections (000001.json, 000002.json)
- ✅ **IPFS integration** - Decentralized metadata storage support
- ✅ **Instant minting** - Create and send URITokens in one transaction

### URI Remit vs Traditional NFTs:

| Feature | Traditional NFTs | URI Remits |
|---------|-----------------|------------|
| Account Setup | ✅ Pre-existing required | ❌ Auto-created |
| Transaction Type | Complex minting flow | Single Remit transaction |
| Metadata | On-chain or IPFS | URI field (IPFS recommended) |
| Distribution | Multi-step process | Payment → instant NFT |
| Use Case | Art, collectibles | Dynamic minting, access tokens |

## Hooks ~ Trigger on ttPAYMENT && ttINVOKE

| File | Description |
|------|-------------|
| harcoded_uri_remit.c | Hardcoded URI: mints URIToken with fixed IPFS URI on incoming payments. |
| harcoded_multi_uri_remit.c | Hardcoded multi-URI: mints 2 URITokens with fixed URIs on incoming payments. |
| install_uri_remit.c | Install param URI: mints URIToken with URI configured at install time. |
| install_multi_uri_remit.c | Install param multi-URI: mints multiple URITokens with install-configured URIs. |
| invoke_uri_remit.c | Invoke sequential URI: mints sequential URITokens using PREFIX/COUNT state parameters. |
| invoke_multi_uri_remit.c | Invoke multi-mint URI: batch mints 1-5 sequential URITokens per payment. |

## Overview

These hooks illustrate URIToken remit patterns:
- **Hardcoded remits** for fixed IPFS collections with predictable metadata.
- **Install-time parameters** for configurable URI collections decided at deployment.
- **Invoke-set sequential** for dynamic NFT collections with automatic numbering (000001.json format).
- **Multi-token remits** for batch minting multiple URITokens per payment.
- **Sequential numbering** for professional NFT collection management.

Typical behaviors:
- Accepts incoming XAH payments to trigger URIToken minting.
- Accepts outgoing payments and other transaction types without action.
- **Creates URITokens using Remit transaction type with URI metadata**.
- Supports sequential numbering for collection-style NFT projects.
- Validates parameters and rollbacks on errors with clear messages.

## Sequential Numbering Pattern

The invoke-based hooks demonstrate professional NFT collection patterns:

```c
Sequential URI Format:
  PREFIX: "ipfs://bafybe...collection/"  (base IPFS path)
  COUNT:  Total tokens in collection     (e.g., 10000)
  
  Generated URIs:
  → ipfs://bafybe...collection/000001.json
  → ipfs://bafybe...collection/000002.json
  → ipfs://bafybe...collection/000003.json
  ...continuing sequentially
```

Perfect for:
- **NFT Collections**: Professional numbered metadata
- **Event Tickets**: Sequential ticket numbering  
- **Certificates**: Ordered credential issuance
- **Gaming Assets**: Level-based or rarity-ordered items

## IPFS Metadata Integration

Example IPFS collection structure:
```
ipfs://bafybeifhrrj3pvotihooenwljj4rreskvdb6dahxwq7cskpao73ishk2p4/
├── 000001.json ← First minted token
├── 000002.json ← Second minted token
├── 000003.json
├── ...
└── 010000.json ← Collection complete
```

Each JSON file contains standard NFT metadata:
```json
{
  "name": "Collection Item #1",
  "description": "Sequential NFT from Xahau Hook",
  "image": "ipfs://bafybeiabc.../001.png",
  "attributes": [
    {"trait_type": "Number", "value": 1},
    {"trait_type": "Rarity", "value": "Common"}
  ]
}
```

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.
- **[IPFS](https://ipfs.io/)**: For hosting decentralized metadata collections.

## Installation & Usage

1. Copy one of the .c hooks into the Hooks Builder starter template and compile to WASM.
2. Deploy the hook to a Testnet account:
   - For **hardcoded hooks**: deploy as-is with fixed URIs in source.
   - For **install-param hooks**: provide URI parameters during deployment.
   - For **invoke hooks**: send Invoke transaction after deployment to set PREFIX/COUNT.
3. Test by sending XAH payments to trigger URIToken minting.
4. Verify URIToken remit transactions in Xahau Explorer.

## Parameter Encoding

Use [Hooks Services Tools](https://hooks.services/tools) for encoding:

**Install Parameter Examples:**
- `URI`: Convert your IPFS URI to hex for install parameters
- `URI1`, `URI2`, etc.: Multiple URIs for multi-mint hooks

**Invoke Parameter Examples:**
- `PREFIX`: IPFS base path (e.g., "ipfs://bafybe.../collection/")  
- `COUNT`: Total tokens available (e.g., 10000 as uint64)
- `MINT`: Tokens per payment (1-5, default 1)

## Testing Your URI Remits

1. **Deploy Hook**: Use Hooks Builder with appropriate parameters.
2. **Set Parameters**: For invoke hooks, send Invoke with PREFIX/COUNT.
3. **Test Payments**: Send XAH to trigger URIToken minting.
4. **Verify URITokens**: Check Xahau Explorer for new URIToken objects.
5. **Check Metadata**: Verify IPFS URIs resolve to proper JSON metadata.

## Use Cases & Examples

### NFT Collection Launch
1. Upload metadata to IPFS (000001.json → 010000.json)
2. Deploy `invoke_uri_remit.c` hook
3. Set PREFIX to IPFS base, COUNT to total supply
4. Users send payments → receive sequential NFTs

### Event Ticketing  
1. Create ticket metadata for each seat/tier
2. Deploy `install_multi_uri_remit.c` with ticket URIs
3. Users purchase → receive URIToken tickets
4. Validate tickets by checking URIToken ownership

### Certificate Issuance
1. Generate certificate metadata per recipient
2. Use sequential numbering for audit trail
3. Issue certificates via URIToken remits
4. Recipients verify authenticity via blockchain

## Code Structure

Each hook typically:
1. Reads URI configuration (hardcoded, install params, or invoke state).
2. Handles early accepts for non-payment transactions.
3. For ttPAYMENT: validates amount and prepares URIToken remit.
4. For ttINVOKE: sets state parameters (PREFIX, COUNT, MINT).
5. Constructs Remit transaction with URI field using `URI_TO_BUF` macro.
6. Calls `emit()` and `accept()` with success messages, or `rollback()` on errors.

## Advanced: Multi-Mint Batch Processing

The `invoke_multi_uri_remit.c` hook demonstrates efficient batch minting:
- **MINT parameter**: Configure 1-5 tokens per payment
- **Guard optimization**: Carefully managed loop iterations
- **Sequential numbering**: Maintains collection order across batches

Perfect for:
- **Bulk purchases**: Multiple tokens per transaction
- **Gas efficiency**: Reduced transaction costs per token
- **Collection management**: Organized sequential distribution

## Debugging Tips

- Verify Hook triggers are set correctly (ttPAYMENT & ttINVOKE).
- Check parameter encoding — URIs must be properly hex-encoded.
- Use TRACESTR to log URI construction and sequential numbering.
- Test IPFS URIs resolve correctly before deployment.
- For sequential hooks, verify state persistence across transactions.
- Monitor guard budget for multi-mint operations.

## Important Notes

- URITokens require properly formatted metadata URIs.
- IPFS is recommended for decentralized, permanent metadata storage.
- Sequential numbering maintains collection integrity and professional appearance.
- Test metadata accessibility before launching production collections.
- Consider gas costs for multi-mint operations.

## Related Collections

- See `Remit_IOU` for IOU token remitting patterns.
- See `Basic_State` for state management examples.
- See `Basic_Invoke_Parameters` for invoke→state patterns.
- See other collections in the repo for additional Xahau Hook patterns.

## Acknowledgments

Special thanks to the community developers who contributed code snippets and insights that helped make this URI Remit collection possible:

- **[Cbot](https://github.com/Cbot-XRPL)** - Provided essential URI remit patterns and technical guidance
- **[Andrei Rosseti](https://github.com/rosseti)** - Contributed key code snippets and implementation insights

These contributions demonstrate the collaborative spirit of the Xahau Hooks community and help make advanced functionality accessible to all developers.

## Contributing

This is a community learning project — contributions, suggestions, and feedback are always welcome! If you develop new URI remit patterns or discover optimizations, please share them with the community.

## License

See the parent repository [LICENSE](../LICENSE) file for details.

---

**Perfect for**: NFT collections, event tickets, certificates, gaming assets, membership tokens, and any use case requiring metadata-rich tokens with automatic distribution.
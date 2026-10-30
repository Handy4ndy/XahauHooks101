# Xahau Hooks 101 – Remit_URI Hooks Collection

## About This Collection

This directory is part of the **Xahau Hooks 101** educational series. It contains concise, beginner-friendly smart contract (Hook) examples written in C for the Xahau blockchain. These hooks demonstrate how to emit URIToken remit transactions (NFT-like tokens with metadata URIs, typically IPFS links) from the hook account using hardcoded values, install-time parameters, or runtime state set via Invoke transactions. All examples are compiled to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and are suitable for Testnet or Mainnet deployment.

See the parent [`Xahau-Hooks-101`](../README.md) for project context.

## Overview

Hooks in this collection demonstrate:
- NFT-like URIToken remittance with metadata URIs (IPFS recommended)
- Hardcoded, install-time, and invoke-set parameterization
- Multi-token batch minting and sequential numbering for collections
- State usage for dynamic configuration (e.g., PREFIX, COUNT, MINT)
- Parameter validation, safe state read/write, and Remit transaction logic
- Professional NFT collection, ticketing, and certificate issuance patterns

## Hook Triggers

- **Triggers:**
  - `ttPAYMENT` (incoming payment triggers URIToken minting)
  - `ttINVOKE` (invoke transactions for admin or state configuration)
  - Both triggers should be set for full functionality

## Hook List

| File                        | Purpose/Description |
|-----------------------------|---------------------|
| harcoded_uri_remit.c        | Hardcoded URI: mints URIToken with fixed IPFS URI on incoming payments. |
| harcoded_multi_uri_remit.c  | Hardcoded multi-URI: mints 2 URITokens with fixed URIs on incoming payments. |
| install_uri_remit.c         | Install param URI: mints URIToken with URI configured at install time. |
| install_multi_uri_remit.c   | Install param multi-URI: mints multiple URITokens with install-configured URIs. |
| invoke_uri_remit.c          | Invoke sequential URI: mints sequential URITokens using PREFIX/COUNT state parameters. |
| invoke_multi_uri_remit.c    | Invoke multi-mint URI: batch mints 1-5 sequential URITokens per payment. |

For detailed explanations, see comments in each `.c` file.

## Technical Patterns

- Hardcoded, install-time, and invoke-set parameterization for URI remittance
- State usage for dynamic configuration (invoke-set hooks)
- Sequential numbering for NFT collections (e.g., 000001.json, 000002.json)
- Multi-token batch minting with MINT parameter and guard optimization
- Remit transaction construction with URI metadata (IPFS recommended)
- Parameter validation, safe state handling, and clear rollback messages

## Tools & Resources

- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging
- **[IPFS](https://ipfs.io/)**: For hosting decentralized metadata collections

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
   - **Important:** Configure hook triggers for both `ttPAYMENT` and `ttINVOKE`. If you don’t set this, the hook will NOT be triggered!
   - For hardcoded hooks: deploy as-is with fixed URIs in source
   - For install-param hooks: provide URI parameters during deployment
   - For invoke hooks: send Invoke transaction after deployment to set PREFIX/COUNT/MINT

5. **Test Transactions**
   - Send XAH payments to trigger URIToken minting

6. **Verify**
   - Check URIToken remit transactions in Xahau Explorer

## Parameter Encoding

- Use [Hooks Services Tools](https://hooks.services/tools) for encoding
- **Install Parameter Examples:**
  - `URI`: Convert your IPFS URI to hex for install parameters
  - `URI1`, `URI2`, etc.: Multiple URIs for multi-mint hooks
- **Invoke Parameter Examples:**
  - `PREFIX`: IPFS base path (e.g., "ipfs://bafybe.../collection/")
  - `COUNT`: Total tokens available (e.g., 10000 as uint64)
  - `MINT`: Tokens per payment (1-5, default 1)

## Testing & Debugging

- Use TRACESTR and TRACEHEX for execution tracing
- Check transaction logs and explorer for results
- **Common pitfalls:**
  - Parameter length/type mismatches
  - Missing triggers (hook not firing)
  - Invalid or missing state access
  - Permissions/admin/owner errors
  - Improperly encoded URIs or metadata
  - Not removing debug traces before production

## Code Structure & Best Practices

- Each hook has a clear entry point, parameter validation, and accept/rollback logic
- Uses safe state handling and clear rollback messages
- Sequential numbering and batch minting for professional NFT collections
- Remove debug traces before production deployment

## Dependencies

- Requires `hookapi.h` and standard C libraries

## Contributing

This is an open educational resource. Contributions and improvements are welcome! If you develop new URI remit patterns or discover optimizations, please share them with the community.

## License

See the parent repository [LICENSE](../LICENSE) file for details.

---

**Perfect for**: NFT collections, event tickets, certificates, gaming assets, membership tokens, and any use case requiring metadata-rich tokens with automatic distribution.
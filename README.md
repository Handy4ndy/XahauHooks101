# Xahau Hooks 101

Welcome to **Xahau Hooks 101** — the definitive educational resource for learning smart contract (Hook) development on the Xahau Network! This repository contains **101 real, working Hook examples** written in C, each designed to be concise, clear, and practical. Every example is a fully functional contract, thoroughly commented, and focused on a specific concept or pattern.

Whether you’re new to smart contracts, an experienced developer exploring Xahau, or just curious about decentralized logic, you’ll find these examples approachable and hands-on. All contracts are ready to compile to WebAssembly (WASM) using the [Xahau Hooks Builder](https://builder.xahau.network/develop) and can be tested on the Xahau Testnet or deployed to Mainnet.

---

## Overview

Xahau Hooks are lightweight WASM modules that add custom logic to Xahau accounts, enabling transaction validation, automation, and data management. This comprehensive collection demonstrates the full spectrum of Hook capabilities, from basic transaction handling to advanced concepts like state management, multi-currency operations, and NFT-like functionality.

---

## Repository Structure

The repository is organized into 9 major collections, each with its own README and set of focused examples:

| Collection                        | Description |
|------------------------------------|-------------|
| [Basic_Install_Parameters/](Basic_Install_Parameters/) | Install-time configuration patterns (5 hooks) |
| [Basic_Invoke_Parameters/](Basic_Invoke_Parameters/)   | Runtime parameter setting via Invoke (5 hooks) |
| [Basic_State/](Basic_State/)                           | State management and persistence (8 hooks) |
| [Basic_IOU/](Basic_IOU/)                               | IOU token payment handling and validation (28 hooks) |
| [Basic_Native/](Basic_Native/)                         | Native XAH payment processing (28 hooks) |
| [Emit_IOU/](Emit_IOU/)                                 | IOU token emission using Payment transactions (7 hooks) |
| [Emit_Native/](Emit_Native/)                           | Native XAH emission and distribution (7 hooks) |
| [Remit_IOU/](Remit_IOU/)                               | IOU token remitting with automatic trustline creation (7 hooks) |
| [Remit_URI/](Remit_URI/)                               | NFT-like URIToken creation and distribution (6 hooks) |

**Total: 101 working Hook examples across 9 organized collections**

---

## Key Features Covered

**Transaction Types:**
- Payment processing (XAH and IOU)
- Invoke transactions for configuration
- Remit transactions for token creation
- Multi-transaction orchestration

**Advanced Concepts:**
- Sequential numbering for NFT collections
- Multi-currency operations
- Batch processing and efficiency optimization
- State persistence across transactions
- Parameter validation and error handling
- Admin/owner controls and permission patterns
- Emission and remittance (including NFT-like URITokens)
- Error handling, rollback, and guard budget management

---

## Tools & Resources

- **[Xahau Hooks Builder](https://builder.xahau.network/develop)**: Develop, compile, deploy, and test hooks
- **[Deploy](https://builder.xahau.network/deploy)**: Deploy and configure hooks on Testnet accounts
- **[Test](https://builder.xahau.network/test)**: Create accounts, fund them, and perform transactions
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore hook executions and manage deployments
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)**: For hex conversion and debugging

---

## Getting Started

**New to Hooks?** Start with these collections in order:
1. **[Basic_Install_Parameters/](Basic_Install_Parameters/)** - Learn basic concepts
2. **[Basic_State/](Basic_State/)** - Understand state management
3. **[Basic_Native/](Basic_Native/)** - Work with XAH payments
4. **[Emit_Native/](Emit_Native/)** - Create your first emissions

**Ready for Advanced Features?**
1. **[Basic_IOU/](Basic_IOU/)** - Multi-currency operations  
2. **[Remit_IOU/](Remit_IOU/)** - Automatic trustline creation
3. **[Remit_URI/](Remit_URI/)** - NFT-like token creation

---

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

---

## Debugging Tips

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution
- **Xahau Explorer**: Verify `TransactionType` and `HookHash`
- **Common Issues**:
  - Incorrect HookOn triggers: check against [hookon calc](https://richardah.github.io/xrpl-hookon-calculator/)
  - Buffer errors: Use `uint8_t amount[48]` for `sfAmount`
  - Invalid fields: Check `otxn_field` return values (e.g., `< 0`)
- **Testnet Node**: `wss://testnet.xahau.network`

---

## Dependencies

- Requires `hookapi.h` and standard C libraries

---

## Community & Acknowledgments

This collection represents the collaborative effort of the Xahau Hooks community. Special thanks to all contributors who provided code snippets, testing, feedback, and guidance that made this comprehensive resource possible.

**Notable Contributors:**
- **[Cbot](https://github.com/Cbot-XRPL)** - URI remit patterns and technical guidance
- **[Andrei Rosseti](https://github.com/rosseti)** - Code snippets and implementation insights
- **Community developers** who tested, debugged, and improved these examples

---

## Contributing

This project is complete at **101 working examples**, but the Xahau ecosystem continues to evolve! Contributions for:
- Bug fixes and optimizations
- Documentation improvements  
- New advanced patterns as they emerge
- Community feedback and testing results

Fork the repo, suggest improvements, or share your experience. Submit issues or PRs to help maintain Xahau Hooks 101 as the definitive learning resource for the community.

---

## License

This project is released under an open source license. See [LICENSE](LICENSE) for details.

---

**🎉 Congratulations!** You now have access to the most comprehensive collection of Xahau Hook examples available. Start exploring, building, and creating amazing applications on the Xahau Network!

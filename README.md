
# Xahau Hooks 101

Welcome to **Xahau Hooks 101** — the complete educational resource for learning smart contract development on the Xahau Network! This repository contains **101 real, working smart contracts (Hooks)**, each written in C and designed to be as concise and clear as possible. Every example is a fully functional contract, thoroughly commented and intentionally focused to help developers understand core concepts quickly and effectively.

Whether you're a developer new to smart contracts, an experienced coder exploring Xahau, or just curious about decentralized logic, you'll find these examples approachable, practical, and immediately useful. The goal is to make learning Xahau Hooks accessible, hands-on, and comprehensive.

All 101 contracts are ready to compile to WebAssembly (WASM) using the [Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop) and can be tested on the Xahau Testnet or deployed to Mainnet. Dive in, experiment, and use these as the foundation for your own projects!

## Overview

Xahau Hooks are lightweight WASM modules that add custom logic to Xahau accounts, enabling transaction validation, automation, and data management. This comprehensive collection demonstrates the full spectrum of Hook capabilities, from basic transaction handling to advanced concepts like state management, multi-currency operations, and NFT-like functionality.

**Complete Collection Features:**
- **101 working Hook examples** covering all major use cases
- **9 organized collections** from basic concepts to advanced patterns
- **Progressive learning path** from simple accepts to complex multi-transaction logic
- **Production-ready patterns** used in real Xahau applications
- **Comprehensive documentation** with usage examples and testing guides

## Project Achievement

🎯 **Mission Accomplished!** This project has achieved its goal of providing a complete set of short, easy-to-understand Hook examples that:
- ✅ **Showcase every major Xahau Hook capability** in focused, practical snippets
- ✅ **Serve as the definitive learning resource** for C and Xahau development  
- ✅ **Provide comprehensive references** for all types of Hook implementations
- ✅ **Use online tools exclusively** for accessibility and Mainnet compatibility
- ✅ **Cover the entire Hook ecosystem** from basic to advanced patterns

Every contract is kept brief, thoroughly commented, and follows consistent patterns to maximize clarity and learning value. This collection represents the most complete resource for Xahau Hook development available.

## Complete Hook Collections

### Foundation (Basic Concepts)
- **[Basic_Install_Parameters/](Basic_Install_Parameters/)** (5 hooks) - Install-time configuration patterns
- **[Basic_Invoke_Parameters/](Basic_Invoke_Parameters/)** (5 hooks) - Runtime parameter setting via Invoke
- **[Basic_State/](Basic_State/)** (8 hooks) - State management and persistence
- **[Basic_IOU/](Basic_IOU/)** (28 hooks) - IOU token payment handling and validation
- **[Basic_Native/](Basic_Native/)** (28 hooks) - Native XAH payment processing

### Advanced Patterns (Emission & Creation)
- **[Emit_IOU/](Emit_IOU/)** (7 hooks) - IOU token emission using Payment transactions
- **[Emit_Native/](Emit_Native/)** (7 hooks) - Native XAH emission and distribution
- **[Remit_IOU/](Remit_IOU/)** (7 hooks) - IOU token remitting with automatic trustline creation
- **[Remit_URI/](Remit_URI/)** (6 hooks) - NFT-like URIToken creation and distribution

**Total: 101 working Hook examples across 9 organized collections**

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

**Production Patterns:**
- Admin-controlled operations
- Install vs runtime configuration
- Hardcoded vs dynamic behavior
- Multi-emit batch operations
- Guard budget management

## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.

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

## Testing & Deployment

1. **Setup Accounts**:
   - Fund a Testnet account using the Testnet Faucet
2. **Deploy Hooks**:
   - Copy any .c file into Hooks Builder starter template
   - Compile to WASM and deploy to your account
3. **Test Transactions**:
   - Use Hooks Builder Test or XRPLWin tools for transactions
4. **Verify Results**:
   - Check execution in Xahau Explorer and Hook logs

## Debugging Tips

- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution
- **Xahau Explorer**: Verify `TransactionType` and `HookHash`
- **Common Issues**:
  - Incorrect HookOn triggers: check against [hookon calc](https://richardah.github.io/xrpl-hookon-calculator/)
  - Buffer errors: Use `uint8_t amount[48]` for `sfAmount`
  - Invalid fields: Check `otxn_field` return values (e.g., `< 0`)
- **Testnet Node**: `wss://testnet.xahau.network`

## Community & Acknowledgments

This collection represents the collaborative effort of the Xahau Hooks community. Special thanks to all contributors who provided code snippets, testing, feedback, and guidance that made this comprehensive resource possible.

**Notable Contributors:**
- **[Cbot](https://github.com/Cbot-XRPL)** - URI remit patterns and technical guidance
- **[Andrei Rosseti](https://github.com/rosseti)** - Code snippets and implementation insights
- **Community developers** who tested, debugged, and improved these examples

## Contributing

This project is complete at **101 working examples**, but the Xahau ecosystem continues to evolve! Contributions for:
- Bug fixes and optimizations
- Documentation improvements  
- New advanced patterns as they emerge
- Community feedback and testing results

Fork the repo, suggest improvements, or share your experience. Submit issues or PRs to help maintain Xahau Hooks 101 as the definitive learning resource for the community.

## License

This project is released under an open source license. See [LICENSE](LICENSE) for details.

---

**🎉 Congratulations!** You now have access to the most comprehensive collection of Xahau Hook examples available. Start exploring, building, and creating amazing applications on the Xahau Network!

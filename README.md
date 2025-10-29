
# Xahau Hooks 101

Welcome to Xahau Hooks 101 — an educational and experimental resource for anyone looking to get familiar with smart contract technology on the Xahau Network. This repository is a curated collection of real, working smart contracts (Hooks), each written in C and designed to be as short and clear as possible. Every example here is a contract in its own right, fully commented and intentionally concise to help everyone understand the core concepts quickly.

Whether you're a developer new to smart contracts, an experienced coder exploring Xahau, or just curious about decentralized logic, you'll find these examples approachable and practical. The goal is to make learning Xahau Hooks accessible, hands-on, and fun.

All contracts are ready to compile to WebAssembly (WASM) using the [Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop) and can be tested on the Xahau Testnet or deployed to Mainnet. Dive in, experiment, and feel free use these as a foundation for your own projects!


## Overview
Xahau Hooks are lightweight WASM modules that add custom logic to Xahau accounts, such as validating transactions or managing data. This project demonstrates a wide range of features, starting with basic transaction handling and gradually exploring advanced concepts like state management and namespaces. As a living resource, it continues to grow with new, well-documented examples.


## Project Goal
Our aim is to provide a set of short, easy-to-understand Hook examples that:
- Showcase Xahau Hook capabilities in small, focused snippets.
- Serve as a learning tool for C and Xahau development.
- Provide a reusable reference for future projects.
- Use online tools for accessibility and Mainnet compatibility.

Every contract is kept brief and is thoroughly commented to maximize clarity and learning value. This project will continue to evolve, adding new Hooks as the technology and community grow.


## Tools

Use these online tools to work with these hooks—no local setup required:
- **[Hex visualizer](https://transia-rnd.github.io/xrpl-hex-visualizer/)** and **[Hooks.Services](https://hooks.services/tools)** for conversion.
- **[Xahau Hooks Builder](https://hooks-builder.xrpl.org/develop)**: Primary platform for developing, compiling, deploying, and testing hooks on Testnet using the starter template.
- **[Deploy](https://hooks-builder.xrpl.org/deploy)**: Deploy and configure hooks on Testnet accounts.
- **[Test](https://hooks-builder.xrpl.org/test)**: Create accounts, fund them, and perform transactions directly within the platform.
- **[XRPLWin Hook Management](https://xahau-testnet.xrplwin.com/)**: Explore Hook executions in detail (Great for Debugging)
- **[Xahau Explorer](https://test.xahauexplorer.com/en)**: Verify transactions and hook details.


## Current Status
This project now contains a broad set of examples, each in its own folder and fully documented. These Hooks cover everything from basic transaction processing to more advanced features, and are intended as a foundation for learning and experimentation. See the individual folders and their `README.md` files for details on each contract.



## What's Next? (And Future Plans)

The next stage of Xahau Hooks 101 is already underway and it's more exciting than ever! We're moving beyond the basics to incorporate:

- **Native and IOU emits**: Learn how to trigger and handle emits for both native and IOU assets, opening up new possibilities for on-chain event-driven logic.
- **Processing and emitting additional transactions**: See how Hooks can not only react to incoming transactions, but also process and emit new transactions as part of their logic, enabling powerful automation and composability.
- **Advanced transaction validation** (e.g., filtering by amount or account).
- **Namespaces for organized data**.
- And much more — always keeping each example short, clear, and well-commented.

We're pushing the boundaries of what's possible with Xahau Hooks, making this the perfect place to learn, experiment, and get inspired. Stay tuned for new examples and features as we continue to explore the cutting edge of smart contract technology!


## Testing
1. **Setup Accounts**:
   - Fund a Testnet account (e.g., `rTest123...`) using the Testnet Faucet.
2. **Deploy Hooks**:
   - Compile Hooks in Xahau Hooks Builder (starter template) and deploy via Hooks Builder or XRPLWin Hook Management.
3. **Test Transactions**:
   - Send Payments (e.g., XAH or IOU) using XRPLWin Tools.
4. **Verify**:
   - Check results in Xahau Explorer or Hooks Builder logs (`TRACESTR`/`TRACEHEX`).


## Debugging Tips
- **Logs**: Use `TRACESTR` and `TRACEHEX` to track execution.
- **Xahau Explorer**: Verify `TransactionType` and `HookHash`.
- **Common Issues**:
  - Incorrect HookOn triggers: check against [hookon calc](https://richardah.github.io/xrpl-hookon-calculator/)
  - Buffer errors: Use `uint8_t amount[48]` for `sfAmount`.
  - Invalid fields: Check `otxn_field` return values (e.g., `< 0`).
- **Testnet Node**: `wss://testnet.xahau.network`.


## Contributing
This is a community learning project — contributions, suggestions, and feedback are always welcome! Fork the repo, suggest new Hook ideas, or share your experience. Submit issues or PRs to help grow Xahau Hooks 101 and make smart contract technology more accessible to everyone.

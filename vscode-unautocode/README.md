# UnAutoCode for VS Code

This extension provides comprehensive language support for the **UnAutoCode** language, a strict 1952 Autocode interpreter designed for the Ferranti Mark 1 architecture.

## Features

- **Syntax Highlighting**: Beautiful and strict highlighting for `.uac` files. It natively recognizes:
  - `CONTROL` and `IF` jump statements.
  - The flat-mapped registers `c0` through `c25`.
  - Math operators `+`, `-`, `*`, `/`.
  - Relational operators `==`, `<`, `>`, `<=`, `>=`.
  - Numeric literals.
- **Commenting Support**: Native support for toggling line comments (`;`) via standard VS Code shortcuts (e.g., `Ctrl+/`).
- **Code Snippets**: Scaffolding templates built-in for fast development:
  - `init`: Scaffold a quick variable initialization.
  - `loop`: Scaffold the decrement/CONTROL conditional jump structure.
  - `fib`: Instantly inject the full Fibonacci sequence boilerplate.

## Usage

Create a new file with the `.uac` extension and start writing! Use the snippet shortcuts like `fib` and press `Tab` to generate the boilerplates.

## License

This project is licensed under the MIT License - see the LICENSE file for details. Copyright (c) 2026 LunnaGuy13.

# UnAutoCode Interpreter

An optimized(not really but anyway lol), production-ready C11 interpreter strictly mirroring the original 1952 Autocode developed by Alick Glennie for the Ferranti Mark 1 computer. It features a complete lexer, EBNF-compliant recursive descent parser, and a fast runtime virtual machine.
(yes i did spend 10 hours on this and I am a not a programming GOD like other folks on this fine platform, i am a beginner, so cut me some slack)
## Architecture

The project is built entirely in C and adheres strictly to the historical limitations of early computing:
- **Flat Memory Map:** All variables are mapped directly to physical registers (`c0` through `c25`).
- **Rigid Three-Address Limitation:** The interpreter only allows exactly two operands and one operator per line (e.g., `c5 = c0 + c1`). Nesting operations is treated as a syntax error.
- **Literal Line Number Jumps:** Loops and branching are controlled by exact, 1-indexed source code line numbers (e.g., `CONTROL 7 IF c2 > c0`).

## Building and Running

You can compile the engine using the included `Makefile`:

```bash/shell
# Compile the main engine
make

# Run the Fibonacci script
./unautocode fibonacci.uac

```

## Language EBNF Grammar

```EBNF
program            = { line } ;
line               = [ statement ] , newline ;
statement          = assignment_statement | control_statement ;
assignment_statement = register , "=" , expression ;
control_statement    = "CONTROL" , line_number , "IF" , condition ;
expression         = binary_operation | operand ;
binary_operation   = operand , operator , operand ;
condition          = operand , rel_operator , operand ;
operand            = register | number ;
register           = ( "c" | "C" ) , digit , { digit } ;
operator           = "+" | "-" | "*" | "/" ;
rel_operator       = ">" | "<" | "==" | ">=" | "<=" ;
number             = [ "-" ] , digit , { digit } , [ "." , { digit } ] ;
line_number        = digit , { digit } ;
digit              = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
newline            = "\n" | "\r\n" ;
```

Comments begin with `;` and continue to the end of the line.

## VS Code Extension
This repository includes a VS Code extension in the `vscode-unautocode/` directory which provides syntax highlighting and snippets for `.uac` files. You can copy this folder to your `.vscode/extensions/` directory to use it natively!

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details. Copyright (c) 2026 LunnaGuy13.

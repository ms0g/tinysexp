# tinysexp
`tinysexp` is a minimalist Lisp compiler that targets the x86-64 architecture. It takes Lisp source code written in a simple s-expression syntax and compiles it down to NASM-compatible x86 assembly code.
## Features
A subset of Lisp, including:
### Operators
- Arithmetic Operators: `+`, `-`, `*`, `/`.
- Comparison Operators: `=`, `/=`, `>`, `<`, `>=`, `<=`.
- Logical Operators: `and`, `or`, `not`
### Condition
- `if`,`when` and `cond`
### Loop
- `dotimes` and `loop`
### Functions
- `defun`
### Variables
- Local Variables: with `let` and `setq`.
- Global Variables: with `defvar`.
- Constant Variables: with `defconstant`.
### System V AMD64 ABI Compliance
The output assembly follows the System V AMD64 ABI, ensuring compatibility with Linux and other UNIX-like operating systems on AMD64 systems.
## Usage
```bash
➜  ~ tinysexp -h
OVERVIEW: Lisp compiler for x86-64 architecture

USAGE: tinysexp <input lisp file> [options]

OPTIONS:
  -o, --output          The output file name
  -h, --help            Display available options
  -v, --version         Display the version of this program
```
## License
This project is licensed under the GPL-2.0 License. See the LICENSE file for details.

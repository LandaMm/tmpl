# Cheetah (Work-in-Progress)

**Cheetah** (temporary name) is a high-performance automation scripting language, designed to run fast with optional C/C++ acceleration.

## Introduction

At first this project aimed to automate simple but time taking tasks that have similar patterns, for instance project generation according to the desired structure, file cleaning, apps configuration using TUI, small and robust scripts etc. It was first implemented as an interpreted language with custom syntax that allowed to easily describe console application usage. Apart from that it was powered by C/C++ dynamic library loading capability which theoretically made it turing complete. The interpreter is fast and robust thanks to a decision to use C++ for it's implementation, and therefore platform independent due to absence of compilation procedure to target machine binary executable. It's uniqueness was expressed in it's type safety and stability through compile-time type checking.

But after discovering more of a system programming and better code semantics I've came to the decision to rewrite this project completely and make it compiled instead of being interpreted. Except that I also decided to change it's syntax and therefore it's goal - not just some automation scripts or dumb project generator, but fully independent system low-mid level compiled language with it's own semantics, rules and approach to solving different technical issues.

## Syntax Definition

At the moment the syntax of the language looks like following:

```c
// external module loading
#import "std"

// external variable
errno: int #foreign;

// external function
sigf :: (x: float) -> float #foreign;

// custom types
byte :: u8;

// global variables
buffer: byte = 255;

// enumeration types
NodeType :: enum {
	Unknown = 0;
	Binary;
	Literal;
}

// structure types
BinaryNode :: struct {
	left: *Node;
	right: *Node;
	op: BinaryOp;
}

// functions
evaluate_expr :: (node: *Node) -> int {
	return map node.type -> int {
		::Literal {
			return <LiteralNode>(node).value;
		}
		::Binary {
			expr := <BinaryNode>(node);

			left := evaluate_expr(expr.left);
			right := evaluate_expr(expr.right);

			return map expr.op -> int {
				::Add {
					return left.value + right.value;
				}
				::Subtract {
					return left.value - right.value;
				}
				::Multiply {
					return left.value * right.value;
				}
				::Divide {
					return left.value / right.value;
				}
			}
		}
	}
}

// entry-point (main) function
main :: (argc: int, argv: *char) -> int {
	// inferred typed variables
	x := sigf(3.1415);
	n: NodeType = ::Literal;
	return 0;
}

```

## Key features

- Distinguish between compile-time and runtime code.
- Built-in linking with other libraries written in C/C++ or Rust.
- Low level memory management through manual definition and layout of primitive and derived types.
- Smart and high level AST parser and traverser.

## Getting Started

```bash
git clone https://github.com/adalspace/cheetah.git
git switch v2
mkdir build && cd build
cmake ..
make
./cheetah <path/to/script.tmpl>
```

Check out `tests/` folder for language usage examples and inspiration.

## Linking with C/C++ modules at compile-time

No runtime dynamic library loading anymore. Now the language will be able to link with external functions or other symbols at the compilation step when bumped into object files.


## Goals & Motivation

The reason I'm implementing own complex programming language is to advance my knowledge about compilers and how they work in complex tasks by implementing real programming language and using it in my future projects as primary or secondary language. The projects I'm planning to implement with this language are for example following:

- The compiler of this language itself (self-hosted compiler)
- Desktop UI application (probably a Code Editor)
- Networking (HTTP + own protocols) primarily for my home lab.
- Gamedev both 2D and 3D (integrating into my [game engine](https://github.com/adalspace/noname-engine))

## Contributing

I would really appreciate any kind of commitment (pull request, issue reporting, donation etc.) into this project and be happy to communicate. 


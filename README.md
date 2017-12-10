# Snail Shell

This is a custom shell that I created for my CS283 class at Drexel. We had the option to create
a shell from a given template that passed a certain amount of given tests, instead of taking the
final. If your shell passed all the tests 100% correctly, you got a 100% on the final. If not,
you had to take the final anyway. My shell worked.

## Getting Started

The shell, and the accompanying C files can all be compiled by running
```
make
```
To clean up the compilation, and delete the compiled binaries from the shell and C programs, run
```
make clean
```

## Contributing

The Makefile, and the C files and trace files that are used to test the shell, as well as the basic
structure of the shell (including some helper methods) were all provided by the instructor of the class.
The functionality of the shell was all written by me.

## File Descriptions

Makefile	# Compiles your shell program and runs the tests
README.md		# This file
snsh.c		# The shell program
/referenceShell/tshref		# The reference shell binary.

# The remaining files are used to test your shell
sdriver.pl	# The trace-driven shell driver
/traces/trace*.txt	# The 15 trace files that control the shell driver
/referenceShell/tshref.out 	# Example output of the reference shell on all 15 traces

# Little C programs that are called by the trace files
myspin.c	# Takes argument <n> and spins for <n> seconds
mysplit.c	# Forks a child that spins for <n> seconds
mystop.c        # Spins for <n> seconds and sends SIGTSTP to itself
myint.c         # Spins for <n> seconds and sends SIGINT to itself

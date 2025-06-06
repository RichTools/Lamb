# Lamb

> [!WARNING]
> This language is in developement and is unfinished

## What is Lamb?
Lamb is a lambda calculus based programming language without all the bells and whistles, implemeted purely in raw lambda calculus, along with modules for reuseability.
The parser and interpreter archicture is written in C. 

## Useage

### Compile from Scratch
1. Use any compiler you wish to compile `build/richBuild.c`, for instance:
    ```bash
    gcc build/richBuild.c -o richBuild
    ```
2. run the `richBuild` executable

### Use the Build Executable
On a unix Operating system run the `richBuild` execuable to generate a `Lamb` executable.

### Lamb Executable
`./Lamb`
- Launches the REPL
  
`./Lamb -i inputfile.l`
- Interprets a passed in file

### Debugging
Edit `build/richBuild.c` to add debugging flags to cflags
- `-DLOGGING`: logs reduction steps during Computation
- `-LOGTREES`: log parse trees for expressions during excution.

## What is Lambda Calculus?
Lambda Calculus is one of the simplest models for computation designed by Alonzo Church (1936).
In this model we consider a function $f(x) := x + 3$, which can in Lambda notation be written as $f := (\lambda x. x+3)$.


### Grammar 
The Grammar of Lamb follows closely with the lambda claculus Grammar.

```bnf
<assignment>  ::= <variable> `:=` <expression> 
<expression>  ::= <name> | <function> | <application>
<function>    ::= (`λ` <name>`.`<expression>)
<application> ::= (<expression> <expression>)
<variable>    ::= <name> | <variable> <name>
<name>        ::= [Aa-Zz]
```
### Rules of Computation
**Beta Reduction**

$$\frac{A((\lambda x. M)N)B \quad free(N)\cap bound(M)= \emptyset }{AM[N/x]B}(\beta \ \text{reduction})$$

**Alpha Conversion**

$$\frac{A(\lambda x.M)B \quad y \notin M}{A(\lambda y.M[y/x])B}(\alpha \ \text{conversion})$$

**Eta Conversion**

$$\frac{A(\lambda x. (M\ x))B \quad x \notin \text{free}(M)}{AMB}  (\eta \text{-conversion})$$

## Language Reference

### Basic Syntax

### Writing Programs

### Modules

## StdLamb - Standard Library

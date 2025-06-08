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

Multiple arguments are support through function application, or Currying, therefore the Lambda calculus equivalent of $f(x, y) = 2x + y$ is $f := (\lamba x . (\lambda y . 2 \times x + y))$.

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
Lambda Calculus has 2 main rules of computation, these are known as Beta Reduction and Alpha conversion.

**Key Terms**:
- $[N/x]$ means we are substituting all x for N.
- Each rule is a syllogism:
    - Consisting of a premise above the line.
    - And a conclusion below the line.
    - The line means therefore.

**Beta Reduction**

$$\frac{A((\lambda x. M)N)B \quad free(N)\cap bound(M)= \emptyset }{AM[N/x]B}(\beta \ \text{reduction})$$

- We can run this syllogism under the condition $free(N) \cap bound(M) = \emptyset$, meaning that there are no free varaibles N, that are bound in M.
- In Beta Reduction we substitute all x's for N, for example in $(\lambda x . x + 1)$ we discard $\lambda x$ swap the $x$ for $N$ resulting in $(N + 1)$.
- Note that $A$ and $B$ remain unchanged.


**Alpha Conversion**

$$\frac{A(\lambda x.M)B \quad y \notin M}{A(\lambda y.M[y/x])B}(\alpha \ \text{conversion})$$

- Alpha conversion is used to avoid variable name clashes.
- We can swap $y$ with $x$ as long as $x$ is not already in the expression and the overall meaning is retained.

**Eta Reduction**

$$\frac{A(\lambda x. (M\ x))B \quad x \notin \text{free}(M)}{AMB}  (\eta \text{-reduction})$$

- Eta Reduction is an optimisation, which simplifies expressions which do nothing but apply another function.
- The expression $\lambda x . (M x)$ can be reduced to just $M$ if $x$ does not appear freely in $M$.
- This means the function doesn't actually use $x$ in any meaningful way — it's just passing it along — so the wrapper is unnecessary.

## Language Reference

### Basic Syntax
### Functions
We can define functions using the `\` Syntax for Lambda.
```
(\x . x)
```
This is the identity function, it takes a variable `x` and returns a variable `x`.

### Application 
Functions can be applied in a similar fashion:
```
(\x . x)(\y . y)
```
There we apply the function `(\y . y)` to the identity function resulting in `(\y . y)` 
This works because the function `(\y . y)` gets substituted in place of the bound varaible `x` ($[(\lambda y . y) / x]$).The argument definition (`\x`) gets dropped and we end with `(\y . y)`. A worked example can be found [here](https://github.com/RichTools/Lamb/blob/main/examples/example.md).

### Applying multiple Variables 
We can create expressions of multiple arguments using Currying. 
```
(\x y . x y)
```
Here is the Lamb equivalent of $(\lambda x (\lambda y . x y))$, the above syntax omits the need for a second lambda symbol, using the lambda calculus scope convention, this means where brackets are omitted the expression binds as far right as possible. 

Similarly brackets are not needed for application, expressions are evaluated greedily from the left, the following two statements are equivalent:

```
(AND TRUE FALSE) 

((AND TRUE) FALSE)
```

This is also known as reverse polish notation which can be found in stack based languages such as Forth.

### Writing Programs
#### Variables
A key part of witing programs is being able to reuse expressions. To do this in Lamb we use the `:=` operator to bind an expression to a name.

```
True := (\t f . t)
False := (\t f . f)
And := (\a b . a b F)

(AND TRUE FALSE) -- FALSE

```
#### Commments
Comments are defined using the Haskell Style `--` synax. 

```
-- This is a Comment 
(\x . x x a)(\a b . b) -- this is an expression
```

### Modules

Definitions can be reused via modules. Any `.l` file which contains only definition statements (`VAR := expr`) and comments can be treated as a module. 

```
#import "path_to_file/module.l"

(VAR)(\x . x)
```

All definitions from the module become available in the file.

> [!WARNING]
> Relative imports are not currently supported, so the full path needs to be provided.

## StdLamb - Standard Library



# Lamb

> [!WARNING]
> This language is in developement and is unfinished

## Contents
- [What is Lamb?](#what-is-lamb)
- [Usage](#usage)
  - [Compile from Scratch](#compile-from-scratch)
  - [Use the Build Executable](#use-the-build-executable)
  - [Lamb Executable](#lamb-executable)
  - [Debugging](#debugging)
- [What is Lambda Calculus?](#what-is-lambda-calculus)
  - [Grammar](#grammar)
  - [Rules of Computation](#rules-of-computation)
    - [Beta Reduction](#beta-reduction)
    - [Alpha Conversion](#alpha-conversion)
    - [Eta Reduction](#eta-reduction)
- [Call-by-Value (CBV)](#call-by-value-cbv)
- [Language Reference](#language-reference)
  - [Basic Syntax](#basic-syntax)
  - [Functions](#functions)
  - [Application](#application)
  - [Applying multiple Variables](#applying-multiple-variables)
  - [Writing Programs](#writing-programs)
    - [Variables](#variables)
    - [Commments](#commments)
  - [Modules](#modules)
- [StdLamb - Standard Library](#stdlamb---standard-library)

## What is Lamb?
Lamb is a lambda calculus based programming language without all the bells and whistles, implemeted purely in raw lambda calculus.
The parser and interpreter archicture is written in C. 

## Usage

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

Multiple arguments are support through function application, or Currying, therefore the Lambda calculus equivalent of $f(x, y) = 2x + y$ is $f := (\lambda x . (\lambda y . 2 \times x + y))$.

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

## Call-by-Value (CBV)

- What it is: arguments are evaluated before function application, and in Lamb function bodies inside abstractions are also reduced eagerly.
  - Effect: an application `(F X)` first reduces `F` and `X` to values, then substitutes.
- How it applies to Lamb: classic `Y`-combinator or naive conditionals can diverge under CBV.
  - Use CBV-safe patterns: a CBV fixpoint like `Z`, and thunked branches for `IF` (pass `\_ . ...` then force with `ID`).
- Writing Lamb code under CBV: define non-strict constructs explicitly.
  - Prefer CBV-friendly encodings (e.g., `IS_ZERO := (\n . n (\_ . F) T)`, `MUL := (\m n f x . m (n f) x)`).
- Alternative (Call-by-Name / Need): substitutes arguments without evaluating them first.
  - Pros: classic `Y` and unthunked `IF` work; Cons: may duplicate work (CBN); call-by-need adds sharing but changes evaluator design.
  
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

Import once at the top of your file:

```lamb
#import "stdLamb.l"
```

### Booleans
- **T**, **F**: Church booleans.
  - **T** being True and **F** being False
  > Example
  ```lamb
  (T a b) -> a
  (F a b) -> b
  ```

### Logic
#### `AND a b`
- returns `a` if `a` is true, otherwise `F`.
  > Example
  ```lamb
  (AND T F) -> F
  ```

#### `OR a b`
- returns `T` if `a` is true, else `b`.
  > Example
  ```lamb
  (OR F T) -> T
  ```

### `NOT a`:
- boolean negation.
  > Example
  ```lamb
  (NOT T) -> F
  ```

### Conditionals
#### `IF b x y` 
- selects between branches using a Church boolean `b`.
  - CBV-safe usage: pass thunks and force with `ID`.
  > Example (CBV)
  ```lamb
  (IF (IS_ZERO ZERO) (\_ . ONE) (\_ . TWO) ID) -> ONE
  ```
#### `IS_ZERO n`: 
- `T` if Church numeral `n` is zero; else `F`.
  > Example
  ```lamb
  (IS_ZERO ZERO) -> T
  (IS_ZERO ONE)  -> F
  ```

### Church numerals (numbers)
#### `ZERO ... TEN`
- Church numerals 0..10.
  > Example
  ```lamb
  (ZERO f x) -> x
  (ONE f x)  -> (f x)
  ```

### Arithmetic
#### `S n`
- The successor function
  > Example
  ```lamb
  (S ZERO) -> ONE
  ```
### `PLUS ab`
- addition.
  > Example
  ```lamb
  (PLUS TWO THREE) -> FIVE
  ```
#### `MUL a b`
- multiplication.
  > Example
  ```lamb
  (MUL TWO THREE) -> SIX
  ```
#### `PRED n`
- predecessor (clamped at ZERO).
  > Example
  ```lamb
  (PRED THREE) -> TWO
  ```

### Comparison
- **GTE x y**: `T` if `x ≥ y`, else `F`.
  > Example
  ```lamb
  (GTE THREE TWO) -> T
  ```
- **EQ x y**: `T` if `x = y`, else `F`.
  > Example
  ```lamb
  (EQ TWO TWO) -> T
  ```

### Pairs (tuples)
#### `PAIR x y`
- Church pair; apply to a consumer to choose.
#### `FST p `/ `SND p`
- First/second projection.
  > Example
  ```lamb
  (FST (PAIR A B)) -> A
  (SND (PAIR A B)) -> B
  ```

### Combinators and identities
#### `Y f`:
- classic fixpoint combinator (works in call-by-name). Under CBV it may diverge; prefer CBV-safe patterns (thunks or a CBV fixpoint) when needed.
  > Example (conceptual)
  ```lamb
  (Y FACTF) ONE -> factorial(1)
  ```
> [!NOTE]
> It is suggested to use an interative approach for most things for now. See [fatorial](./examples/factorial.l).

#### `ID x`
- Identity function
  > Example
  ```lamb
  (ID X) -> X
  ```
#### `IDENTITY x`
- self-application helper `(\x . x x)`; apply with a binary function-style argument.
  > Example
  ```lamb
  (IDENTITY (\y . y)) -> ((\y . y) (\y . y))
  ```

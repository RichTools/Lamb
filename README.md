# Lamb

> [!WARNING]
> This language is in developement and is unfinished

## What is Lamb?
Lamb is a lambda calculus based programming language without all the bells and whistles, implemeted purely in raw lambda calculus.
The parser and interpreter archicture is written in C. 

## What is Lambda Calculus?

### Grammar 
The Grammar of Lamb follows closely with the lambda claculus Grammar.

```bnf
<assignment>  ::= <variable> ":=" <expression> 
<expression>  ::= <name> | <function> | <application>
<function>    ::= (λ <name>.<expression>)
<application> ::= (<expression> <expression>)
<variable>    ::= <name> | <variable> <name>
<name>        ::= [Aa-Zz]
```
### Rules
**Beta Reduction**

$$\frac{A((\lambda x. M)N)B \quad free(N)\cap bound(M)= \emptyset }{AM[N/x]B}(\beta \ \text{reduction})$$

**Alpha Conversion**

$$\frac{A(\lambda x.M)B \quad y \notin M}{A(\lambda y.M[y/x])B}(\alpha \ \text{conversion})$$

**Eta Conversion**

$$\frac{A(\lambda x. (M\ x))B \quad x \notin \text{free}(M)}{AMB}  (\eta \text{-conversion})$$

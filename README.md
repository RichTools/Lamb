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
<assignment>  ::= <variable> "=" <expression> 
<expression>  ::= <name> | <function> | <application>
<function>    ::= (λ <name>.<expression>)
<application> ::= (<expression> <expression>)
<variable>    ::= <name> | <variable> <name>
<name>        ::= [Aa-Zz]
```

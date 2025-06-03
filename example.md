### Example 1
```
λ > (true (λx. xy))

    δ > expanded 'true' into '(λt. (λf. t))'

Δ > (λt. (λf. t)) (λx. xy)
    β > (λt. (λf. ((λx. xy))))
    β > (λf. (λx. xy))
>>> (λf. (λx. xy))
```

### Example 2
```
true  := (λt. (λf. t))
false := (λt. (λf. f))
and   := (λa. (λb. ((a b) a)))

(and true false)

-- Expand in 3 Steps:

δ> ((λa. (λb. ((a b) a))) true false)
δ> ((λa. (λb. ((a b) a))) (λt. (λf. t)) false)
δ> ((λa. (λb. ((a b) a))) (λt. (λf. t)) (λt. (λf. f)))

-- Beta Reduction: 

β>
   
    (λa. (λb. ((a b) a))) (true) (false)        
    (λa. (λb. ((a b) a))) (λt. (λf. t)) (false)        -- replace all a with true [true / a]
        → (λb. (((λt. (λf. t)) b)(λt. (λf. t))) (false)


β> 
    (λb. (((λt. (λf. t)) b)(λt. (λf. t))) (false)  
    (λb. (((λt. (λf. t)) b)(λt. (λf. t))) (λt. (λf. f))      -- [false / b]
    (λb. ((true b) true)) (false)                           
        -> (((λt. (λf. t)) (λt. (λf. f)))) 
        -> ((true false) true)

β>  
    (λt. (λf. t)) (λt. (λf. f)) (λt. (λf. t))
    ((λt. (λf. t)) (λt. (λf. f)) (true)              -- [false / t]
        -> (λf. (λt. (λf. f))) (true)
        -> ((λf. false) true)


β>  
    ((λf. (λt. (λf. f))) true)    -- [true / f]
                                  -- f is free in terms of the t expression
                                  -- true gets discarded
    -> (λt. (λf. f))
    → false

β > ((λb. (((λt. (λf. t)) b) (λt. (λf. t)))) 
β > ((λb. ((λf. b) (λt. (λf. t)))) (λt. (λf. f)))
β > ((λb. b) (λt. (λf. f)))


```




#import "stdLamb.l";

T := (\t f . t) -- true
F := (\t f . f) -- false

-- Define Logic
AND := (\a b . a b F) 
OR := (\a b . a T b)
NOT := (\a . a F T)

-- Evaluate an Expression
(AND T F)


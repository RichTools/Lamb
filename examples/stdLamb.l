T := (\t f . t) -- true
F := (\t f . f) -- false

-- Define Logic

IDENTITY := (\x . x x) -- this the identity function

AND := (\a b . a b F) 
OR := (\a b . a T b)
NOT := (\a . a F T)

-- Define Booleans

T := (\t f . t) -- true
F := (\t f . f) -- false

-- Define Logic

AND := (\a b . a b F) 
OR := (\a b . a T b)
NOT := (\a . a F T)


IDENTITY := (\x . x x) -- this the identity function

-- Operations 

IF := (\b x y . b x y)

-- Numbers 

ZERO := (\s z . z)

S := (\w y x . y (w y x))


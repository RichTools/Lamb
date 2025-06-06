-- Define Booleans

T := (\t f . t) -- true
F := (\t f . f) -- false

-- Define Logic

AND := (\a b . a b F) 
OR := (\a b . a T b)
NOT := (\a . a F T)

IDENTITY := (\x . x x) -- this the identity function

-- Operations 


-- P = \lambda nfx.n(\lambda gh.h(gf))(\lambda u.x)(\lambda u.u)
PRED := (\n f x . n(\g h . h (g f))(\u . x)(\u . u))

IF := (\b x y . b x y)
IS_ZERO := (\n . n F T)

-- G = \lambda xy.Z(xPy)
GTE := (\x y . IS_ZERO (x PRED y)) 


-- Numbers 

ZERO  := (\s z . z)
ONE   := (\s z . s(z))
TWO   := (\s z . s(s(z)))
THREE := (\s z . s(s(s(z))))
FOUR  := (\s z . s(s(s(s(z)))))
FIVE  := (\s z . s(s(s(s(s(z))))))
SIX   := (\s z . s(s(s(s(s(s(z)))))))
SEVEN := (\s z . s(s(s(s(s(s(s(z))))))))
EIGHT := (\s z . s(s(s(s(s(s(s(s(z)))))))))
NINE  := (\s z . s(s(s(s(s(s(s(s(s(z))))))))))
TEN   := (\s z . s(s(s(s(s(s(s(s(s(s(z)))))))))))

S := (\w y x . y (w y x)) -- successor function

PLUS  := (\a b . a S b)
TIMES := (\x y z . x (y z))

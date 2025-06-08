-- Define Booleans

T := (\t f . t) -- true
F := (\t f . f) -- false

-- Define Logic

AND := (\a b . a b F) 
OR := (\a b . a T b)
NOT := (\a . a F T)

IDENTITY := (\x . x x) -- this the identity function

-- Operations 


PRED := (\n f x . n(\g h . h (g f))(\u . x)(\u . u))

IF := (\b x y . b x y)
IS_ZERO := (\n . n F T)

GTE := (\x y . IS_ZERO (x PRED y)) 

EQ := (\x y . AND (GTE (x PRED y))(GTE (y PRED x)))

-- Y Combinator 
Y := (\y . (\x . y (x x))(\x . y (x x)))

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
MUL := (\x y z . x (y z))


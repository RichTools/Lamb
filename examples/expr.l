-- defining true
T := (\t f . t)
F := (\t f . f)
AND := (\a b . a b F)
OR := (\a b . a T b)
NOT := (\a . a F T)
(AND T F)

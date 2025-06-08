#import "../examples/stdLamb.l"

F := (\f n . IF (IS_ZERO n) ONE (MULT n (f (PRED n)))) 
Z := (\f x . (f (\v . x x v)) (\x . f (\v . x x v)))

-- needs lazy eval
((Z F) ONE)

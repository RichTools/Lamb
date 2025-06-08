#import "../examples/stdLamb.l"

-- Evaluate an Expression
(AND T F)

-- \wedge (\vee F(\lnot T))T = F
(AND (OR F (NOT T)) T)

(S (S THREE))

(TIMES ONE TWO)

(IS_ZERO ZERO)

(EQ ONE ONE)


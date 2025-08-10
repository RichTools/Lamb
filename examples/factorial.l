#import "stdLamb.l"

-- Iteration step: (i, acc) -> (i+1, acc * (i+1))
PHI := (\p . PAIR (S (FST p)) (MUL (S (FST p)) (SND p)))

-- Iterative factorial: SND (n PHI (PAIR ZERO ONE))
FACT := (\n . SND (n PHI (PAIR ZERO ONE)))

(FACT THREE)


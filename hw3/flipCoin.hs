flipCoin' ::  String -> Integer  -> [Integer]
flipCoin' [] k = [0]
flipCoin' [x] k
    | x == 'H' = [0]
	| otherwise = [k, 0]
flipCoin' (x:y:xs) k
	| x == y = flipCoin' (y:xs) (k+1)
	| otherwise = k : flipCoin' (y:xs) (k+1)

flipCoin ::  String -> [Integer]
flipCoin x = flipCoin' x 1



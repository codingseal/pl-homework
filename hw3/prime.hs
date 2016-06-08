primes :: [Integer]
primes =  sieve [2..]
  where sieve (p:xs) = p : sieve [x | x <- xs, x `mod` p > 0]

findPrimes n m = findPrimes' n m primes
findPrimes' n m (x:xs)
    | n > x = findPrimes' n m (xs)
	| otherwise = x : take (m-1) (xs)


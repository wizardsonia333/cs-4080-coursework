-- haskell ex. of grouping operations for one type together
-- counter takes in an int as x and makes a counter where the functions share that same x
-- in this example x is 5 so getvalue returns 5 and double returns 10

-- record that keeps the operations for this type together
-- each field is a function
data Counter = Counter
  { getValue :: () -> Int
  , double   :: () -> Int
  }

counter :: Int -> Counter
counter x = Counter
  -- both functions close over the same underlying value x
  { getValue = \() -> x
  , double   = \() -> x * 2
  }

main = do
  -- create one counter and give it 5 as the input value
  let c = counter 5

  -- call both functions stored in the record
  -- expected output is 5 and then 10
  print (getValue c ())
  print (double c ())
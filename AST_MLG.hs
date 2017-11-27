-- MLG - my language good 
-- This file it is AST for my language MLG

-- x

-- MLGProgram [MLGExprList (Mstring "x")]
data MLGProgram = MLGProgram [MLGOperation] deriving (Show)

data MLGOpertor = Madd                
                | Msub 
                | Mmul
                | Mdiv
                | Mxor deriving (Show, Eq)

data MLGOperNotMat = Meq
                   | MBolse
                   | MMenshe 
                   | MdoubleEq deriving (Show,Eq)

data MLGRval = Mint Int
             | Mstring String
             | Moper MLGRval MLGOpertor MLGRval deriving (Show,Eq)
             
data MLGOperation = MifMelse MLGRval MLGOperNotMat MLGRval
                  | Mfor MLGRval MLGOperNotMat MLGRval 
                  | MfunckOpred String [String] [MLGOperation] 
                  | MfunckColl String [MLGRval]
                  | Return MLGRval  
                  | Mopred String MLGRval deriving (Show, Eq)


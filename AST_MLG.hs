-- MLG - my language good 
-- This file it is AST for my language MLG

-- MLG - my language good -- This file it is AST for my language MLG
data MLG.Program = MLG.Program ExprList deriving (Show, Eq)
data MLG.ExprList = MLG.ExprList MLG.Expr MLG.ExprList 
                    | MLG.ExprNull     deriving(Show, Eq)
data MLG.Expr = MLG.Expr Srting MLG.Rval deriving (Show, Eq)
data MLG.Opertor = Madd                
                   | Msub
                   | Mmul
                   | Mdiv
                   | Meq
                   | Mxor deriving (Show, Eq)
data MLG.Rval = Mint Int
                | Mstring Srting
                | MifMelse MLG.Rval MLG.Rval MLG.Rval
                | Moper MLG.Rval MLG.Operator MLG.Rval
                | Mfor MLG.Rval MLG.Operator MLG.Rval deriving(Show, Eq)
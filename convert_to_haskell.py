import convert_string as C

s = input()
s = s.split(' ')
p = 0

def parse():
    global p
    term = s[p]
    assert p < len(s)
    p += 1
    if term[0] == "I":
        x = 0
        for c in term[1:].encode('ascii'):
            x = x * 94 + c - 33
        return f"(IntV {x})"
    if term[0] == "S":
        text = C.from_icfp(term)
        text = text.replace("\\", "\\\\").replace('\n', '\\n')
        return f'(StrV "{text}")'
    if term[0] == "T":
        return f"(BoolV True)"
    if term[0] == "F":
        return f"(BoolV False)"
    if term[0] == "U":
        x = parse()
        if term[1] == "-":
            return f"(IntNeg {x})"
        if term[1] == "!":
            return f"(BoolNot {x})"
        if term[1] == "#":
            return f"(Stoi {x})"
        if term[1] == "$":
            return f"(Itos {x})"
    if term[0] == "B":
        x = parse()
        y = parse()
        if term[1] == "+":
            return f"(IntAdd {x} {y})"
        if term[1] == "-":
            return f"(IntSub {x} {y})"
        if term[1] == "*":
            return f"(IntMul {x} {y})"
        if term[1] == "/":
            return f"(IntDiv {x} {y})"
        if term[1] == "%":
            return f"(IntMod {x} {y})"
        if term[1] == "<":
            return f"(IntLt {x} {y})"
        if term[1] == ">":
            return f"(IntGt {x} {y})"
        if term[1] == "=":
            return f"(IsEqual {x} {y})"
        if term[1] == "|":
            return f"(BoolOr {x} {y})"
        if term[1] == "&":
            return f"(BoolAnd {x} {y})"
        if term[1] == ".":
            return f"(StrCat {x} {y})"
        if term[1] == "T":
            return f"(StrTake {x} {y})"
        if term[1] == "D":
            return f"(StrDrop {x} {y})"
        if term[1] == "$":
            return f"(ApplyV {x} {y})"
    if term[0] == "?":
        x = parse()
        y = parse()
        z = parse()
        return f"(IfCond {x} {y} {z})"
    if term[0] == "L":
        x = 0
        for c in term[1:].encode('ascii'):
            x = x * 94 + c - 33
        y = parse()
        return f'(LambdaV "v_{x}" {y})'
    if term[0] == "v":
        x = 0
        for c in term[1:].encode('ascii'):
            x = x * 94 + c - 33
        return f'(VarV "v_{x}")'
    raise ValueError(f"Unkonw {term}")
    


code = parse()
assert len(s) == p
print("""
import Data.Char
import Data.List
import Data.Maybe

data Expr = IntV Integer | StrV String | BoolV Bool | VarV String
          | LambdaV String Expr | ApplyV Expr Expr
          | IntNeg Expr | BoolNot Expr | Stoi Expr | Itos Expr
          | IntAdd Expr Expr | IntSub Expr Expr | IntMul Expr Expr
          | IntDiv Expr Expr | IntMod Expr Expr
          | IntLt Expr Expr | IntGt Expr Expr
          | IsEqual Expr Expr | BoolOr Expr Expr | BoolAnd Expr Expr
          | IfCond Expr Expr Expr
          | StrCat Expr Expr | StrTake Expr Expr | StrDrop Expr Expr
          deriving(Show)

alpha :: String -> Expr -> Expr -> Expr
alpha name value expr = case expr of
  IntV _ -> expr
  StrV _ -> expr
  BoolV _ -> expr
  VarV x -> if x == name then value else expr
  LambdaV x e -> if x == name then expr else (LambdaV x (alpha name value e))
  ApplyV e1 e2 -> ApplyV (alpha name value e1) (alpha name value e2)
  IntNeg e -> IntNeg (alpha name value e)
  BoolNot e -> BoolNot (alpha name value e)
  Stoi e -> Stoi (alpha name value e)
  Itos e -> Itos (alpha name value e)
  IntAdd e1 e2 -> IntAdd (alpha name value e1) (alpha name value e2)
  IntSub e1 e2 -> IntSub (alpha name value e1) (alpha name value e2)
  IntMul e1 e2 -> IntMul (alpha name value e1) (alpha name value e2)
  IntDiv e1 e2 -> IntDiv (alpha name value e1) (alpha name value e2)
  IntMod e1 e2 -> IntMod (alpha name value e1) (alpha name value e2)
  IntLt e1 e2 -> IntLt (alpha name value e1) (alpha name value e2)
  IntGt e1 e2 -> IntGt (alpha name value e1) (alpha name value e2)
  IsEqual e1 e2 -> IsEqual (alpha name value e1) (alpha name value e2)
  BoolOr e1 e2 -> BoolOr (alpha name value e1) (alpha name value e2)
  BoolAnd e1 e2 -> BoolAnd (alpha name value e1) (alpha name value e2)
  IfCond e1 e2 e3 -> IfCond (alpha name value e1) (alpha name value e2) (alpha name value e3)
  StrCat e1 e2 -> StrCat (alpha name value e1) (alpha name value e2)
  StrTake e1 e2 -> StrTake (alpha name value e1) (alpha name value e2)
  StrDrop e1 e2 -> StrDrop (alpha name value e1) (alpha name value e2)


convmap = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\\"#$%&'()*+,-./:;<=>?@[\\\\]^_`|~ \\n"
string_to_int :: Integer -> String -> Integer
string_to_int num [] = num
string_to_int num (x : xs) = string_to_int (num * 94 + toInteger (fromJust $ elemIndex x convmap)) xs
int_to_string :: Integer -> String
int_to_string val = if val == 0 then "" else int_to_string (div val 94) ++ take 1 (reverse (take (fromIntegral (mod val 94) + 1) convmap))


eval expr = case expr of
  IntV _ -> expr
  StrV _ -> expr
  BoolV _ -> expr
  VarV _ -> expr
  LambdaV _ _ -> expr
  ApplyV e1 e2 -> case (eval e1) of
    LambdaV name e -> eval (alpha name e2 e)
    _ -> undefined
  IntNeg e -> case (eval e) of
    IntV x -> IntV (-x)
  BoolNot e -> case (eval e) of
    BoolV x -> BoolV (not x)
  Stoi e -> case (eval e) of
    StrV x -> IntV (string_to_int 0 x)
  Itos e -> case (eval e) of
    IntV x -> StrV (int_to_string x)
  IntAdd e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> IntV (x + y)
  IntSub e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> IntV (x - y)
  IntMul e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> IntV (x * y)
  IntDiv e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> if x >= 0 then IntV (div x y) else IntV (-((-x) `div` y))
  IntMod e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> if x >= 0 then IntV (mod x y) else IntV (-((-x) `mod` y))
  IntLt e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> BoolV (x < y)
  IntGt e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> BoolV (x > y)
  IsEqual e1 e2 -> case (eval e1, eval e2) of
    (IntV x, IntV y) -> BoolV (x == y)
    (BoolV x, BoolV y) -> BoolV (x == y)
    (StrV x, StrV y) -> BoolV (x == y)
  BoolOr e1 e2 -> case (eval e1) of
    (BoolV x) -> if x then (BoolV True) else (eval e2)
  BoolAnd e1 e2 -> case (eval e1) of
    (BoolV x) -> if x then (eval e2) else (BoolV False)
  IfCond e1 e2 e3 -> case (eval e1) of
    (BoolV True) -> eval e2
    (BoolV False) -> eval e3
  StrCat e1 e2 -> case (eval e1, eval e2) of
    (StrV x, StrV y) -> StrV (x ++ y)
  StrTake e1 e2 -> case (eval e1, eval e2) of
    (IntV x, StrV y) -> (StrV (take (fromIntegral x) y))
  StrDrop e1 e2 -> case (eval e1, eval e2) of
    (IntV x, StrV y) -> (StrV (drop (fromIntegral x) y))

""")
print(f"code = {code}")
print(f"main = do")
print(f"    print (eval code)")

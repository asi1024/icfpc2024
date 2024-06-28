#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum UnaryOp {
    Neg,
    Not,
    StrToInt,
    IntToStr,
}

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
pub enum BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Lt,
    Gt,
    Eq,
    Or,
    And,
    Concat,
    TakeFirst,
    DropFirst,
    Apply,
}

#[derive(Debug, PartialEq, Eq, Clone)]
pub enum Token {
    Boolean(bool),
    Integer(i64),
    String(String),
    Unary(UnaryOp),
    Binary(BinaryOp),
    Variable(i64),
    If,
    Lambda(i64),
}

fn parse_base94(b: &[u8]) -> i64 {
    let mut n = 0;
    for &d in b {
        n = n * 94 + (d - 33) as i64;
    }
    n
}

fn from_base94(n: i64) -> String {
    if n == 0 {
        return String::from("a");
    }

    let mut v = Vec::new();
    let mut n = n;
    while n > 0 {
        v.push((n % 94 + 33) as u8);
        n /= 94;
    }
    v.reverse();
    String::from_utf8(v).unwrap()
}

const CONVERT_MAP: &'static [u8] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n".as_bytes();


fn to_human_readable(s: &str) -> String {
    s.bytes().map(|c| CONVERT_MAP[c as usize - 33] as char).collect::<String>()
}

fn from_human_readable(s: &str) -> String {
    // TODO: this is slow
    s.chars().map(|c| CONVERT_MAP.iter().position(|&x| x == c as u8).unwrap() as u8 + 33).collect::<Vec<u8>>().iter().map(|&c| c as char).collect::<String>()
}

pub fn parse_token(s: &str) -> Token {
    let b = s.bytes().collect::<Vec<u8>>();

    match b[0] as char {
        'T' => Token::Boolean(true),
        'F' => Token::Boolean(false),
        'I' => Token::Integer(parse_base94(&b[1..])),
        'S' => Token::String(to_human_readable(&s[1..])),
        'U' => Token::Unary(match b[1] as char {
            '-' => UnaryOp::Neg,
            '!' => UnaryOp::Not,
            '#' => UnaryOp::StrToInt,
            '$' => UnaryOp::IntToStr,
            _ => panic!("Invalid unary operator"),
        }),
        'B' => Token::Binary(match b[1] as char {
            '+' => BinaryOp::Add,
            '-' => BinaryOp::Sub,
            '*' => BinaryOp::Mul,
            '/' => BinaryOp::Div,
            '%' => BinaryOp::Mod,
            '<' => BinaryOp::Lt,
            '>' => BinaryOp::Gt,
            '=' => BinaryOp::Eq,
            '|' => BinaryOp::Or,
            '&' => BinaryOp::And,
            '.' => BinaryOp::Concat,
            'T' => BinaryOp::TakeFirst,
            'D' => BinaryOp::DropFirst,
            '$' => BinaryOp::Apply,
            _ => panic!("Invalid binary operator"),
        }),
        '?' => Token::If,
        'L' => Token::Lambda(parse_base94(&b[1..])),
        'v' => Token::Variable(parse_base94(&b[1..])),

        // syntax sugar
        's' => Token::String(s[1..].to_owned()),
        'i' => Token::Integer(s[1..].parse().unwrap()),

        _ => panic!("Invalid token: {}", s),
    }
}

pub fn parse_program(s: &str) -> Vec<Token> {
    s.split_whitespace().map(parse_token).collect()
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Expr {
    Boolean(bool),
    Integer(i64),
    String(String),
    Neg(Box<Expr>),
    Not(Box<Expr>),
    StrToInt(Box<Expr>),
    IntToStr(Box<Expr>),
    Add(Box<Expr>, Box<Expr>),
    Sub(Box<Expr>, Box<Expr>),
    Mul(Box<Expr>, Box<Expr>),
    Div(Box<Expr>, Box<Expr>),
    Mod(Box<Expr>, Box<Expr>),
    Lt(Box<Expr>, Box<Expr>),
    Gt(Box<Expr>, Box<Expr>),
    Eq(Box<Expr>, Box<Expr>),
    Or(Box<Expr>, Box<Expr>),
    And(Box<Expr>, Box<Expr>),
    Concat(Box<Expr>, Box<Expr>),
    TakeFirst(Box<Expr>, Box<Expr>),
    DropFirst(Box<Expr>, Box<Expr>),
    Apply(Box<Expr>, Box<Expr>),
    If(Box<Expr>, Box<Expr>, Box<Expr>),
    Lambda(i64, Box<Expr>),
    Variable(i64),
}

fn parse_tokens_impl(toks: &mut &[Token]) -> Expr {
    assert!(!toks.is_empty());

    let first_tok = &toks[0];
    *toks = &toks[1..];

    match first_tok {
        &Token::Boolean(b) => Expr::Boolean(b),
        &Token::Integer(n) => Expr::Integer(n),
        Token::String(s) => Expr::String(s.clone()),
        &Token::Unary(op) => {
            let e = parse_tokens_impl(toks);
            match op {
                UnaryOp::Neg => Expr::Neg(Box::new(e)),
                UnaryOp::Not => Expr::Not(Box::new(e)),
                UnaryOp::StrToInt => Expr::StrToInt(Box::new(e)),
                UnaryOp::IntToStr => Expr::IntToStr(Box::new(e)),
            }
        }
        &Token::Binary(op) => {
            let e1 = parse_tokens_impl(toks);
            let e2 = parse_tokens_impl(toks);
            match op {
                BinaryOp::Add => Expr::Add(Box::new(e1), Box::new(e2)),
                BinaryOp::Sub => Expr::Sub(Box::new(e1), Box::new(e2)),
                BinaryOp::Mul => Expr::Mul(Box::new(e1), Box::new(e2)),
                BinaryOp::Div => Expr::Div(Box::new(e1), Box::new(e2)),
                BinaryOp::Mod => Expr::Mod(Box::new(e1), Box::new(e2)),
                BinaryOp::Lt => Expr::Lt(Box::new(e1), Box::new(e2)),
                BinaryOp::Gt => Expr::Gt(Box::new(e1), Box::new(e2)),
                BinaryOp::Eq => Expr::Eq(Box::new(e1), Box::new(e2)),
                BinaryOp::Or => Expr::Or(Box::new(e1), Box::new(e2)),
                BinaryOp::And => Expr::And(Box::new(e1), Box::new(e2)),
                BinaryOp::Concat => Expr::Concat(Box::new(e1), Box::new(e2)),
                BinaryOp::TakeFirst => Expr::TakeFirst(Box::new(e1), Box::new(e2)),
                BinaryOp::DropFirst => Expr::DropFirst(Box::new(e1), Box::new(e2)),
                BinaryOp::Apply => Expr::Apply(Box::new(e1), Box::new(e2)),
            }
        }
        &Token::If => {
            let e1 = parse_tokens_impl(toks);
            let e2 = parse_tokens_impl(toks);
            let e3 = parse_tokens_impl(toks);
            Expr::If(Box::new(e1), Box::new(e2), Box::new(e3))
        }
        &Token::Lambda(v) => {
            let e = parse_tokens_impl(toks);
            Expr::Lambda(v, Box::new(e))
        }
        &Token::Variable(v) => {
            Expr::Variable(v)
        }
    }
}

pub fn parse_tokens(toks: &[Token]) -> Expr {
    let mut toks = toks;
    let ret = parse_tokens_impl(&mut toks);
    assert!(toks.is_empty());
    ret
}

#[derive(PartialEq, Eq, Debug)]
pub enum Value {
    Boolean(bool),
    Int(i64),
    String(String),
    Closure(i64, Box<Expr>),
}

impl Value {
    pub fn as_bool(&self) -> bool {
        match self {
            Value::Boolean(b) => *b,
            _ => panic!("Expected boolean"),
        }
    }

    pub fn as_int(&self) -> i64 {
        match self {
            Value::Int(n) => *n,
            _ => panic!("Expected integer"),
        }
    }

    pub fn as_string(self) -> String {
        match self {
            Value::String(s) => s,
            _ => panic!("Expected string"),
        }
    }
}

fn substitute(expr: &Expr, v: i64, e: &Expr) -> Expr {
    match expr {
        Expr::Boolean(b) => Expr::Boolean(*b),
        Expr::Integer(n) => Expr::Integer(*n),
        Expr::String(s) => Expr::String(s.clone()),
        Expr::Neg(e1) => Expr::Neg(Box::new(substitute(e1, v, e))),
        Expr::Not(e1) => Expr::Not(Box::new(substitute(e1, v, e))),
        Expr::StrToInt(e1) => Expr::StrToInt(Box::new(substitute(e1, v, e))),
        Expr::IntToStr(e1) => Expr::IntToStr(Box::new(substitute(e1, v, e))),
        Expr::Add(e1, e2) => Expr::Add(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Sub(e1, e2) => Expr::Sub(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Mul(e1, e2) => Expr::Mul(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Div(e1, e2) => Expr::Div(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Mod(e1, e2) => Expr::Mod(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Lt(e1, e2) => Expr::Lt(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Gt(e1, e2) => Expr::Gt(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Eq(e1, e2) => Expr::Eq(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Or(e1, e2) => Expr::Or(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::And(e1, e2) => Expr::And(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Concat(e1, e2) => Expr::Concat(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::TakeFirst(e1, e2) => Expr::TakeFirst(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::DropFirst(e1, e2) => Expr::DropFirst(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::Apply(e1, e2) => Expr::Apply(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e))),
        Expr::If(e1, e2, e3) => Expr::If(Box::new(substitute(e1, v, e)), Box::new(substitute(e2, v, e)), Box::new(substitute(e3, v, e))),
        Expr::Lambda(v1, e1) => {
            if *v1 == v {
                // shadowing
                Expr::Lambda(*v1, e1.clone())
            } else {
                Expr::Lambda(*v1, Box::new(substitute(e1, v, e)))
            }
        }
        Expr::Variable(v1) => {
            if *v1 == v {
                e.clone()
            } else {
                Expr::Variable(*v1)
            }
        }
    }
}

pub fn evaluate(expr: &Expr) -> Value {
    match expr {
        Expr::Boolean(b) => Value::Boolean(*b),
        Expr::Integer(n) => Value::Int(*n),
        Expr::String(s) => Value::String(s.clone()),
        Expr::Neg(e) => Value::Int(-evaluate(e).as_int()),
        Expr::Not(e) => Value::Boolean(!evaluate(e).as_bool()),
        Expr::StrToInt(e) => {
            let s = from_human_readable(&evaluate(e).as_string());
            Value::Int(parse_base94(s.as_bytes()))
        }
        Expr::IntToStr(e) => {
            let n = evaluate(e).as_int();
            Value::String(to_human_readable(&from_base94(n)))
        }
        Expr::Add(e1, e2) => Value::Int(evaluate(e1).as_int() + evaluate(e2).as_int()),
        Expr::Sub(e1, e2) => Value::Int(evaluate(e1).as_int() - evaluate(e2).as_int()),
        Expr::Mul(e1, e2) => Value::Int(evaluate(e1).as_int() * evaluate(e2).as_int()),
        Expr::Div(e1, e2) => Value::Int(evaluate(e1).as_int() / evaluate(e2).as_int()),
        Expr::Mod(e1, e2) => Value::Int(evaluate(e1).as_int() % evaluate(e2).as_int()),
        Expr::Lt(e1, e2) => Value::Boolean(evaluate(e1).as_int() < evaluate(e2).as_int()),
        Expr::Gt(e1, e2) => Value::Boolean(evaluate(e1).as_int() > evaluate(e2).as_int()),
        Expr::Eq(e1, e2) => {
            let v1 = evaluate(e1);
            let v2 = evaluate(e2);

            match (v1, v2) {
                (Value::Boolean(b1), Value::Boolean(b2)) => Value::Boolean(b1 == b2),
                (Value::Int(n1), Value::Int(n2)) => Value::Boolean(n1 == n2),
                (Value::String(s1), Value::String(s2)) => Value::Boolean(s1 == s2),
                _ => panic!("Invalid comparison"),
            }
        }
        Expr::Or(e1, e2) => Value::Boolean(evaluate(e1).as_bool() || evaluate(e2).as_bool()),
        Expr::And(e1, e2) => Value::Boolean(evaluate(e1).as_bool() && evaluate(e2).as_bool()),
        Expr::Concat(e1, e2) => Value::String(evaluate(e1).as_string() + &evaluate(e2).as_string()),
        Expr::TakeFirst(e1, e2) => {
            let n = evaluate(e1).as_int() as usize;
            let s = evaluate(e2).as_string();
            Value::String(s[..n].to_string())
        }
        Expr::DropFirst(e1, e2) => {
            let n = evaluate(e1).as_int() as usize;
            let s = evaluate(e2).as_string();
            Value::String(s[n..].to_string())
        }
        Expr::Apply(e1, e2) => {
            let v = evaluate(e1);
            match v {
                Value::Closure(v, e) => {
                    evaluate(&substitute(&e, v, e2))
                }
                _ => panic!("Expected closure"),
            }
        }
        Expr::If(e1, e2, e3) => {
            if evaluate(e1).as_bool() {
                evaluate(e2)
            } else {
                evaluate(e3)
            }
        }
        Expr::Lambda(v, e) => {
            Value::Closure(*v, e.clone())
        }
        Expr::Variable(v) => {
            panic!("Free variable is not expected: {}", v);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_token() {
        assert_eq!(parse_token("T"), Token::Boolean(true));
        assert_eq!(parse_token("I/6"), Token::Integer(1337));
    }

    #[test]
    fn test_parse_program() {
        assert_eq!(parse_program("B+ I# I$"), vec![
            Token::Binary(BinaryOp::Add),
            Token::Integer(2),
            Token::Integer(3),
        ]);
        assert_eq!(parse_program("B- I$ I#"), vec![
            Token::Binary(BinaryOp::Sub),
            Token::Integer(3),
            Token::Integer(2),
        ]);
    }

    #[test]
    fn test_evaluate_program() {
        let expr = parse_tokens(&parse_program("B+ I# I$"));
        assert_eq!(evaluate(&expr), Value::Int(5));

        let expr = parse_tokens(&parse_program("B- I$ I#"));
        assert_eq!(evaluate(&expr), Value::Int(1));

        let expr = parse_tokens(&parse_program("U# S4%34"));
        assert_eq!(evaluate(&expr), Value::Int(15818151));

        let expr = parse_tokens(&parse_program("U$ I4%34"));
        assert_eq!(evaluate(&expr), Value::String(String::from("test")));

        let expr = parse_tokens(&parse_program("BT I$ S4%34"));
        assert_eq!(evaluate(&expr), Value::String(String::from("tes")));

        let expr = parse_tokens(&parse_program("B$ B$ L# L$ v# B. SB%,,/ S}Q/2,$_ IK"));
        assert_eq!(evaluate(&expr), Value::String(String::from("Hello World!")));

        let expr = parse_tokens(&parse_program(r#"B$ B$ L" B$ L# B$ v" B$ v# v# L# B$ v" B$ v# v# L" L# ? B= v# I! I" B$ L$ B+ B$ v" v$ B$ v" v$ B- v# I" I%"#));
        assert_eq!(evaluate(&expr), Value::Int(16));
    }
}

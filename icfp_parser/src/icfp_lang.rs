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
    Lambda,  // TODO
}

fn parse_base94(b: &[u8]) -> i64 {
    let mut n = 0;
    for &d in b {
        n = n * 94 + (d - 33) as i64;
    }
    n
}

const CONVERT_MAP: &'static [u8] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`|~ \n".as_bytes();


pub fn parse_token(s: &str) -> Token {
    let b = s.bytes().collect::<Vec<u8>>();

    match b[0] as char {
        'T' => Token::Boolean(true),
        'F' => Token::Boolean(false),
        'I' => Token::Integer(parse_base94(&b[1..])),
        'S' => Token::String(b[1..].iter().map(|&c| CONVERT_MAP[c as usize - 33] as char).collect::<String>()),
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
        'L' => Token::Lambda,
        'v' => Token::Variable(parse_base94(&b[1..])),
        _ => panic!("Invalid token: {}", s),
    }
}

pub fn parse_program(s: &str) -> Vec<Token> {
    s.split_whitespace().map(parse_token).collect()
}

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
    // Lambda(i64, Box<Expr>),
    // Variable(i64),
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
        &Token::Lambda => {
            todo!();
        }
        &Token::Variable(_v) => {
            todo!();
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
    // Closure,
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

pub fn evaluate(expr: &Expr) -> Value {
    match expr {
        Expr::Boolean(b) => Value::Boolean(*b),
        Expr::Integer(n) => Value::Int(*n),
        Expr::String(s) => Value::String(s.clone()),
        Expr::Neg(e) => Value::Int(-evaluate(e).as_int()),
        Expr::Not(e) => Value::Boolean(!evaluate(e).as_bool()),
        Expr::StrToInt(e) => Value::Int(evaluate(e).as_string().parse().unwrap()),
        Expr::IntToStr(e) => Value::String(evaluate(e).as_int().to_string()),
        Expr::Add(e1, e2) => Value::Int(evaluate(e1).as_int() + evaluate(e2).as_int()),
        Expr::Sub(e1, e2) => Value::Int(evaluate(e1).as_int() - evaluate(e2).as_int()),
        Expr::Mul(e1, e2) => Value::Int(evaluate(e1).as_int() * evaluate(e2).as_int()),
        Expr::Div(e1, e2) => Value::Int(evaluate(e1).as_int() / evaluate(e2).as_int()),
        Expr::Mod(e1, e2) => Value::Int(evaluate(e1).as_int() % evaluate(e2).as_int()),
        Expr::Lt(e1, e2) => Value::Boolean(evaluate(e1).as_int() < evaluate(e2).as_int()),
        Expr::Gt(e1, e2) => Value::Boolean(evaluate(e1).as_int() > evaluate(e2).as_int()),
        Expr::Eq(e1, e2) => Value::Boolean(evaluate(e1).as_int() == evaluate(e2).as_int()),
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
        Expr::Apply(_e1, _e2) => {
            todo!();
        }
        Expr::If(e1, e2, e3) => {
            if evaluate(e1).as_bool() {
                evaluate(e2)
            } else {
                evaluate(e3)
            }
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

        let expr = parse_tokens(&parse_program("BT I$ S4%34"));
        assert_eq!(evaluate(&expr), Value::String(String::from("tes")));
    }
}
